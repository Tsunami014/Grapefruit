#include "converse.hpp"
#include <yaml-cpp/yaml.h>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QFile>

template<typename F>
const auto& cached(F&& init) {
    static const auto value = init();
    return value;
}

const YAML::Node& config() {
    return cached([]{
        QFile file(":/data/conv.yml");
        bool ok = file.open(QIODevice::ReadOnly);
        // Should always be ok because we're loading from internal files
        return YAML::Load(file.readAll().toStdString());
    });
}
const std::map<std::string, std::unordered_set<std::string>>& groups() {
    return cached([]{
        std::map<std::string, std::unordered_set<std::string>> gs;
        for (const auto& entry : config()["groups"]) {
            std::unordered_set<std::string> conts;
            if (entry.second.IsSequence()) {
                for (const auto& item : entry.second) {
                    conts.insert(item.as<std::string>());
                }
            }
            gs.insert({entry.first.as<std::string>(), conts});
        }
        return gs;
    });
}
/// A mapping from items to their groups
const std::map<std::string, std::string>& getgroup() {
    return cached([]{
        std::map<std::string, std::string> ggp;
        for (const auto& [grp, conts] : groups()) {
            for (const auto& item : conts) {
                ggp.insert({item, grp});
            }
        }
        return ggp;
    });
}
const std::unordered_set<std::string>& keeps() {
    return cached([]{
        std::unordered_set<std::string> kps;
        for (const auto& entry : config()["groups"]) {
            std::string key = entry.first.as<std::string>();
            if (key.size() > 0 && key.at(0) == '=' && entry.second.IsSequence()) {
                for (const auto& item : entry.second) {
                    kps.insert(item.as<std::string>());
                }
            }
        }
        return kps;
    });
}

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {
        auto ppses = config()["initial"].as<std::vector<std::string>>();
        purpose = ppses[QRandomGenerator::global()->bounded(uint(ppses.size()))];
        refresh();
    }

void Conversation::newTopic() {
    // Remove all context keys unless in keep
    auto kp = keeps();
    for (auto it = context.begin();it != context.end();) {
        if (kp.find(*it) == kp.end()) {
            it = context.erase(it);
        } else { ++it; }
    }
    auto ppses = config()["reset"].as<std::vector<std::string>>();
    purpose = ppses[QRandomGenerator::global()->bounded(uint(ppses.size()))];
    refresh();
}
void Conversation::onclick(Option o) {
    if (o.newpurp != "") {
        purpose = o.newpurp;
    }
    auto ggp = getgroup();
    auto grps = groups();
    for (const auto& chng : o.changes) {
        std::string g;
        bool clear = chng.at(0) == '-';
        if (clear) { g = chng.substr(1); }
        else { g = ggp.at(chng); }
        for (const auto& val : grps.at(g)) {
            auto it = context.find(val);
            if (it != context.end()) {
                context.erase(it); break;
            }
        }
        if (!clear) context.insert(chng);
    }
    refresh();
}

const QRegularExpression polishSynonymRe("{([^}]+)}");
const QRegularExpression polishSplRe(R"((?<!\\)(?:\\\\)*\/)");
QString polishSentence(QString sent) {
    // Replace synonym choices in {brackets/braces}
    auto it = polishSynonymRe.globalMatch(sent);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QStringList opts = m.captured(1).split(polishSplRe);
        QString repl = opts[QRandomGenerator::global()->bounded(opts.length())];

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        sent.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    return sent;
}

void Conversation::refresh() {
    auto ppse = config()["purposes"][purpose];
    if (!ppse) {
        display("Purpose '"+QString::fromStdString(purpose)+"' does not exist!");
        return;
    }
    std::vector<std::string> sents;
    for (const auto& tmpl : ppse["templates"]) {
        std::string group = tmpl.first.as<std::string>();
        for (const auto& conts : tmpl.second) {
            std::string key;
            if (conts.first.IsNull()) { key = "~"; }
            else { key = conts.first.as<std::string>(); }

            if (key == "~") {
                // Only show if no key from this group is present
                bool good = true;
                for (const auto& val : groups().at(group)) {
                    if (context.find(val) != context.end()) {
                        good = false; break;
                    }
                }
                if (!good) continue;
            } else if (key == "+") {
                // Only show if no other key matched for is present
                // But ensure at least one key from this group is present
                std::unordered_set<std::string> badvals;
                for (const auto& conts2 : tmpl.second) {
                    badvals.insert(conts2.first.as<std::string>());
                }
                bool good = false;
                for (const auto& val : groups().at(group)) {
                    if (context.find(val) == context.end()) continue;
                    good = badvals.find(val) == badvals.end();
                    break; // There should only be one group tag active at a time
                }
                if (!good) continue;
            } else {
                // If the key is not present then don't use
                if (context.find(key) == context.end()) continue;
            }
            for (const auto& sent : conts.second) {
                sents.push_back(sent.as<std::string>());
            }
        }
    }
    if (sents.size() == 0) {
        display("No sentence options avaliable!");
        return;
    }
    uint sidx = QRandomGenerator::global()->bounded(uint(sents.size()));
    QString sent = polishSentence(QString::fromStdString(sents[sidx]));

    auto allopts = ppse["options"];
    if (!allopts || allopts.size() == 0) {
        display(sent);
        return;
    }
    uint oidx = QRandomGenerator::global()->bounded(uint(allopts.size()));

    optList outopts;
    for (const auto& item : allopts[oidx]) {
        std::string npurp = "";
        if (item.size() > 2) npurp = item[2].as<std::string>();
        outopts.push_back({
            QString::fromStdString(item[0].as<std::string>()),
            item[1].as<std::unordered_set<std::string>>(),
            npurp
        });
    }
    display(sent, outopts);
}

void Conversation::display(QString title, optList opts) {
    curtxt->setText(title);
    setOpts(olay, opts, [=](Option o){ onclick(o); });
}
