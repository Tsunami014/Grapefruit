#include "converse.hpp"
#include "externs.hpp"
#include "choose.hpp"
#include "taskload.hpp"
#include "importance.hpp"
#include <yaml-cpp/yaml.h>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QFile>
#include <QTimer>

template<typename F> const auto& cached(F&& init) {
    static const auto value = init();
    return value;
}
const YAML::Node& cconfig() {
    return cached([]{
        QFile file(":/data/conv.yml");
        bool ok = file.open(QIODevice::ReadOnly);
        // Should always be ok because we're loading from a preset internal file
        return YAML::Load(file.readAll().toStdString());
    });
}
/// A mapping of groups to their contents
const std::unordered_map<std::string, std::unordered_set<std::string>>& groups() {
    return cached([]{
        std::unordered_map<std::string, std::unordered_set<std::string>> gs;
        for (const auto& entry : cconfig()["groups"]) {
            std::unordered_set<std::string> conts;
            for (const auto& item : entry.second) {
                conts.insert(item.as<std::string>());
            }
            auto nam = entry.first.as<std::string>();
            if (nam.empty()) continue;
            if (nam[0] == '=' || nam[0] == '~') nam = nam.substr(1);
            gs.insert({nam, conts});
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
/// A list of all tags to keep on reset
const std::unordered_set<std::string>& keeps() {
    return cached([]{
        std::unordered_set<std::string> kps;
        for (const auto& entry : cconfig()["groups"]) {
            std::string key = entry.first.as<std::string>();
            if (key.size() > 0 && key.at(0) == '=') {
                for (const auto& item : entry.second) {
                    kps.insert(item.as<std::string>());
                }
            }
        }
        return kps;
    });
}
/// A list of all tags to remove at the start of each new conversation
const std::unordered_set<std::string>& removs() {
    return cached([]{
        std::unordered_set<std::string> rms;
        for (const auto& entry : cconfig()["groups"]) {
            std::string key = entry.first.as<std::string>();
            if (key.size() > 0 && key.at(0) == '~') {
                for (const auto& item : entry.second) {
                    rms.insert(item.as<std::string>());
                }
            }
        }
        return rms;
    });
}

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {
        auto ppses = cconfig()["initial"].as<std::vector<std::string>>();
        purpose = choose(ppses).first;
        QTimer::singleShot(0, [this](){ refresh(); });
    }
void Conversation::newTopic() {
    resetVariety();
    // Remove all context keys unless in keep
    const auto& kp = keeps();
    for (auto it = context.begin();it != context.end();) {
        if (kp.find(*it) == kp.end()) {
            it = context.erase(it);
        } else { ++it; }
    }
    auto ppses = cconfig()["reset"].as<std::vector<std::string>>();
    auto sze = ppses.size();
    if (sze == 0) {
        purpose = "blank";
    } else if (sze == 1) {
        purpose = ppses.at(0);
    } else {
        // Don't new topic onto the current purpose if possible
        if (auto it = std::find(ppses.begin(), ppses.end(), purpose); it != ppses.end()) {
            ppses.erase(it);
        }
        purpose = choose(ppses).first;
    }
    refresh();
}

void Conversation::onclick(Option o) {
    if (o.title.isNull()) {
        refresh();
        return;
    }
    if (o.newpurp != "") {
        purpose = polishSentence(QString::fromStdString(o.newpurp)).toStdString();
    }
    // Remove all context keys in removs
    const auto& rms = removs();
    for (auto it = context.begin();it != context.end();) {
        if (rms.find(*it) != rms.end()) {
            it = context.erase(it);
        } else { ++it; }
    }
    // Add new context tags
    const auto& ggp = getgroup();
    const auto& grps = groups();
    for (const auto& chng : o.changes) {
        QChar first = chng.at(0);
        if (first == '$') {
            externFunc(chng.substr(1));
            continue;
        }
        if (first == '~') {
            if (auto it = context.find(chng.substr(1)); it != context.end()) {
                context.erase(it); break;
            }
            continue;
        }
        std::string g;
        bool clear = first == '-';
        if (clear) { g = chng.substr(1); }
        else { g = ggp.at(chng); }
        for (const auto& val : grps.at(g)) {
            if (auto it = context.find(val); it != context.end()) {
                context.erase(it); break;
            }
        }
        if (!clear) context.insert(chng);
    }

    refresh();
}

constexpr int maxRecursion = 10;

const QRegularExpression groupsRe("%([a-zA-Z_]+)%?");
QString Conversation::polishSentence(QString sent) {
    // Replace %taggroups
    {auto it = groupsRe.globalMatch(sent);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        std::string group = m.captured(1).toStdString();
        QString repl;
        auto it = externList.find(group);
        if (it != externList.end()) {
            repl = runExtern(group);
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
    }}
    // Replace synonym choices in {brackets/braces} (allows some recursion)
    for (int i = 0; i < maxRecursion; i++) {
        // Custom parser because it's too complex with recursion
        QString end;
        qsizetype it = 0;
        QString part;
        QStringList opts;
        uint indent = 0;
        while (it < sent.length()) {
            QChar c = sent.at(it++);
            if (c == '{') {
                if (indent++ > 0) { part += c; }
                else {
                    end += part;
                    part = {};
                }
            } else if (indent > 0 && c == '}') {
                if (--indent > 0) { part += c; }
                else {
                    opts.push_back(part);
                    end += choose(opts).first;
                    part = {};
                    opts = {};
                }
            } else if (indent == 1 && c == '/') {
                opts.push_back(part);
                part = {};
            } else { part += c; }
        }
        opts.push_back(part);
        end += choose(opts).first;
        sent = end;
    }
    return sent;
}

const QRegularExpression dictRe("\\$([a-zA-Z0-9_]+)\\$?");
const QRegularExpression failsafeRe("<([^<>|]+)(?:\\|([^<>|]*))?>");
QString Conversation::basicParseSent(QString sent) {
    auto checkCtx = [&](std::string group){
        if (externList.find(group) != externList.end()) {
            return evalExtern(group, sent);
        } else {
            bool good = false;
            const auto& grps = groups();
            const auto& it2 = grps.find(group);
            if (it2 == grps.end()) return false;
            for (const auto& val : it2->second) {
                if (context.find(val) != context.end()) { good = true; break; }
            }
            return good;
        }
    };
    // Replace dictionary $references (allows some references in references)
    // We do this here so it also checks for groups and other things
    for (int i = 0; i < maxRecursion; i++) {
        auto it = dictRe.globalMatch(sent);
        if (!it.hasNext()) break;
        int offs = 0;
        while (it.hasNext()) {
            auto m = it.next();
            QString repl = QString::fromStdString(cconfig()["dictionary"][m.captured(1).toStdString()].as<std::string>());

            int start = m.capturedStart(0) + offs;
            int end = m.capturedEnd(0) + offs;
            sent.replace(start, end - start, repl);
            offs += repl.length() - (end - start);
        }
    }
    // Parse <> tags
    {auto it = failsafeRe.globalMatch(sent);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString repl = m.captured(1);
        // Check if all context group tags exist, and if not use backup
        auto it = groupsRe.globalMatch(repl);
        while (it.hasNext()) {
            auto m = it.next();
            if (!checkCtx(m.captured(1).toStdString())) {
                repl = m.captured(2);
            }
        }
        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        sent.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }}
    // Check all context group tags exist
    auto it = groupsRe.globalMatch(sent);
    while (it.hasNext()) {
        auto m = it.next();
        if (!checkCtx(m.captured(1).toStdString())) return QString();
    }
    return sent;
}


void Conversation::refresh() {
    auto ppse = cconfig()["purposes"][purpose];
    if (!ppse) {
        display("Purpose '"+QString::fromStdString(purpose)+"' does not exist!");
        return;
    }
    resetExterns();
    std::vector<QString> sents;
    std::vector<uint> optidxs;
    auto matches = [&](const std::string& req) {
        if (req == "*" || req == "") {
            return true;
        } else if (req == "=") {
            return sents.empty();
        } else if (req == "+") {
            return !sents.empty();
        } else { switch (req[0]) {
            case '+': {
                // Only match if any key from this group is present
                bool good = false;
                for (const auto& val : groups().at(req.substr(1))) {
                    if (context.find(val) != context.end()) {
                        good = true; break;
                    }
                }
                return good;}
            case '-': {
                bool good = true;
                // Only match if no key from this group is present
                for (const auto& val : groups().at(req.substr(1))) {
                    if (context.find(val) != context.end()) {
                        good = false; break;
                    }
                }
                return good;}
            case '!':
                // Only match if the key is not present
                return context.find(req.substr(1)) == context.end();
            default:
                // Only match if the key is present
                return context.find(req) != context.end();
        }}
    };
    auto parseSentence = [&](QString sent) {
        if (sent.isNull()) return QString();
        auto idx = sent.lastIndexOf('#');
        if (idx == -1) return basicParseSent(sent);
        uint ln = 1;
        if (idx > 0 && sent.at(idx-1) == ' ') { idx--; ln++; }
        for (const QString& req : sent.mid(idx+ln).split(", ")) {
            bool good = false;
            for (const QString& tok : req.split('?')) {
                good |= matches(tok.toStdString());
            }
            if (!good) return QString();
        }
        return basicParseSent(sent.left(idx));
    };
    std::vector<optList> opts;
    uint idx = 0;
    for (const auto& opt : ppse) {
        // Compile the options into a list
        optList outopts;
        for (const auto& item : opt["opts"]) {
            auto o = parseSentence(QString::fromStdString(item[0].as<std::string>()));
            if (o.isNull()) continue;
            QString title = polishSentence(o);
            if (title.isEmpty()) continue;
            std::string npurp = "";
            if (item.size() > 2) npurp = item[2].as<std::string>();
            auto vec = item[1].as<std::vector<std::string>>();
            outopts.push_back({ title, {vec.begin(), vec.end()}, npurp });
        }
        if (opt["shuffle"] && opt["shuffle"].as<bool>()) {
            std::shuffle(outopts.begin(), outopts.end(), *QRandomGenerator::global());
        }
        opts.push_back(outopts);
        // Get all the templates
        for (const auto& tmpl : opt["templates"]) {
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
                    last = next + 2;
                }
                good = false;
                std::stringstream ss(req);
                std::string token;
                while (!good && std::getline(ss, token, '?')) {
                    good |= matches(token);
                }
                if (next == std::string::npos) break;
            }
            if (good) {
                if (tmpl.second.IsScalar()) {
                    if (auto ns = parseSentence(QString::fromStdString(tmpl.second.as<std::string>())); !ns.isNull()) {
                        sents.push_back(ns);
                        optidxs.push_back(idx);
                    }
                } else {
                    for (const auto& it : tmpl.second) {
                        if (auto ns = parseSentence(QString::fromStdString(it.as<std::string>())); !ns.isNull()) {
                            sents.push_back(ns);
                            optidxs.push_back(idx);
                        }
                    }
                }
            }
        }
        idx++;
    }

    auto out = choose(sents);
    if (out.second == -1) {
        display("No sentences avaliable!");
        return;
    }
    auto sent = out.first;
    if (sent.startsWith("> ")) {
        purpose = polishSentence(sent.sliced(2)).toStdString();
        return refresh();
    }
    if (sent.startsWith("@ ")) {
        resetBest();
        sent = sent.sliced(2);
    }
    display(polishSentence(sent), opts.at(optidxs.at(out.second)));
}

void Conversation::display(QString title, optList opts) {
    curtxt->setText(title);
    setOptsLay(olay, opts, [=](Option o){ onclick(o); });
}
