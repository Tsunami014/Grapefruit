#include "converse.hpp"
#include "externs.hpp"
#include <yaml-cpp/yaml.h>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QFile>

template<typename F> const auto& cached(F&& init) {
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
/// A mapping of groups to their contents
const std::unordered_map<std::string, std::unordered_set<std::string>>& groups() {
    return cached([]{
        std::unordered_map<std::string, std::unordered_set<std::string>> gs;
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
const std::unordered_map<std::string, std::string>& getgroup() {
    return cached([]{
        std::unordered_map<std::string, std::string> ggp;
        for (const auto& [grp, conts] : groups()) {
            for (const auto& item : conts) {
                ggp.insert({item, grp});
            }
        }
        return ggp;
    });
}
/// A list of external groups
const std::unordered_map<std::string, std::string>& externs() {
    return cached([]{
        std::unordered_map<std::string, std::string> gs;
        for (const auto& entry : config()["groups"]) {
            if (!entry.second.IsSequence()) {
                gs.insert({entry.first.as<std::string>(), entry.second.as<std::string>()});
            }
        }
        return gs;
    });
}
/// A list of all tags to keep on reset
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

const QRegularExpression groupsRe("%([a-zA-Z_]+)%?");

const QRegularExpression getRe(R"(^(\d+) ?\* *(.+))");
QString Conversation::getSentence(std::vector<std::string> sents) {
    QStringList choices;
    auto exts = externs();
    for (const auto& str : sents) {
        auto qstr = QString::fromStdString(str);
        // Check all context group tags exist
        auto it = groupsRe.globalMatch(qstr);
        bool good = true;
        while (it.hasNext() && good) {
            auto m = it.next();
            std::string group = m.captured(1).toStdString();
            if (exts.find(group) == exts.end()) {
                good = false;
                auto grps = groups();
                auto it2 = grps.find(group);
                if (it2 == grps.end()) break;
                for (const auto& val : it2->second) {
                    if (context.find(val) != context.end()) good = true; break;
                }
            }
        }
        if (!good) continue;
        // Duplicate the sentence if specified
        auto m = getRe.match(qstr);
        if (m.hasMatch()) {
            bool ok;
            uint amnt = m.captured(1).toUInt(&ok);
            if (!ok) {
                choices << qstr;
            } else {
                QString add = m.captured(2);
                for (uint i = 0; i < amnt; i++) choices << add;
            }
        } else {
            choices << qstr;
        }
    }
    uint ln = choices.size();
    if (ln == 0) return {};
    uint sidx = QRandomGenerator::global()->bounded(ln);
    return choices[sidx];
}

const QRegularExpression polishSynonymRe("{([^}]+)}");
const QRegularExpression polishSplRe(R"((?<!\\)(?:\\\\)*\/)");
QString Conversation::polishSentence(QString sent) {
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
    it = groupsRe.globalMatch(sent);
    offs = 0;
    auto exts = externs();
    while (it.hasNext()) {
        auto m = it.next();
        std::string group = m.captured(1).toStdString();
        QString repl;
        auto it = exts.find(group);
        if (it != exts.end()) {
            repl = runExtern(it->second);
        } else {
            // Find first context tag applied in the group
            repl = m.captured(0);
            for (const auto& it : groups().at(group)) {
                if (context.find(it) != context.end()) {
                    repl = QString::fromStdString(it); break;
                }
            }
        }

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
        auto match = tmpl.first.as<std::string>();
        bool good = true;
        size_t last = 0; size_t next = 0;
        while (good) {
            size_t next = match.find(", ", last);
            std::string req;
            if (next == std::string::npos) {
                req = match.substr(last);
            } else {
                req = match.substr(last, next - last);
            }

            if (req == "=") {
                if (!sents.empty()) {
                    good = false; break;
                }
            } else if (req != "*" && req != "") { switch (req[0]) {
                case '+':
                    // Only match if any key from this group is present
                    good = false;
                    for (const auto& val : groups().at(req.substr(1))) {
                        if (context.find(val) != context.end()) {
                            good = true; break;
                        }
                    }
                    break;
                case '-':
                    // Only match if no key from this group is present
                    for (const auto& val : groups().at(req.substr(1))) {
                        if (context.find(val) != context.end()) {
                            good = false; break;
                        }
                    }
                    break;
                case '!':
                    // Only match if the key is not present
                    good = context.find(req.substr(1)) == context.end();
                    break;
                default:
                    // Only match if the key is present
                    good = context.find(req) != context.end();
                    break;
            }}
            if (next == std::string::npos) break;
            last = next + 2;
        }
        if (good) {
            if (tmpl.second.IsScalar()) {
                sents.push_back(tmpl.second.as<std::string>());
            } else {
                auto moresents = tmpl.second.as<std::vector<std::string>>();
                sents.insert(sents.end(), moresents.begin(), moresents.end());
            }
        }
    }
    QString sent = getSentence(sents);
    if (sent.isNull()) {
        display("No sentence options avaliable!");
        return;
    }
    if (sent.startsWith("> ")) {
        purpose = polishSentence(sent.sliced(2)).toStdString();
        return refresh();
    }
    sent = polishSentence(sent);

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
            polishSentence(QString::fromStdString(item[0].as<std::string>())),
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
