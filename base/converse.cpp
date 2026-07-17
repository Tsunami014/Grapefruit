#include "converse.hpp"
#include "externs.hpp"
#include <yaml-cpp/yaml.h>
#include <regex>
#include <QRandomGenerator>
#include <QRegularExpression>
#include <QFile>

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
        for (const auto& entry : cconfig()["groups"]) {
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
        for (const auto& entry : cconfig()["groups"]) {
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
        auto ppses = cconfig()["initial"].as<std::vector<std::string>>();
        purpose = ppses[QRandomGenerator::global()->bounded(uint(ppses.size()))];
        refresh();
    }
void Conversation::newTopic() {
    // Remove all context keys unless in keep
    const auto& kp = keeps();
    for (auto it = context.begin();it != context.end();) {
        if (kp.find(*it) == kp.end()) {
            it = context.erase(it);
        } else { ++it; }
    }
    auto ppses = cconfig()["reset"].as<std::vector<std::string>>();
    purpose = ppses[QRandomGenerator::global()->bounded(uint(ppses.size()))];
    refresh();
}

void Conversation::onclick(Option o) {
    if (o.newpurp != "") {
        purpose = o.newpurp;
    }
    const auto& ggp = getgroup();
    const auto& grps = groups();
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


const std::regex chooseReStd(R"(^((\d+) ?\* *).*)");
std::pair<std::string, int> choose(const std::vector<std::string>& opts) {
    std::vector<std::pair<int, int>> choices; // idx, prefix len
    int idx = 0;
    for (const auto& str : opts) {
        std::smatch matches;
        if (std::regex_search(str, matches, chooseReStd)) {
            int amnt = static_cast<int>(std::stoul(matches[2]));
            int pln = matches[1].length();
            for (int i = 0; i < amnt; i++) choices.push_back({idx, pln});
        } else {
            choices.push_back({idx, 0});
        }
        idx++;
    }
    int ln = choices.size();
    if (ln == 0) return {{}, -1};
    int out = QRandomGenerator::global()->bounded(ln);
    return {opts[out].substr(choices[out].second), choices[out].first};
}
const QRegularExpression chooseReQ(R"(^((\d+) ?\* *).*)");
std::pair<QString, int> choose(const std::vector<QString>& opts) {
    std::vector<std::pair<int, int>> choices; // idx, prefix len
    int idx = 0;
    for (const auto& str : opts) {
        auto m = chooseReQ.match(str);
        if (m.hasMatch()) {
            uint amnt = m.captured(2).toInt();
            int pln = m.captured(1).length();
            for (uint i = 0; i < amnt; i++) choices.push_back({idx, pln});
        } else {
            choices.push_back({idx, 0});
        }
        idx++;
    }
    int ln = choices.size();
    if (ln == 0) return {{}, -1};
    int out = QRandomGenerator::global()->bounded(ln);
    auto c = choices[out];
    return {opts[c.first].mid(c.second), c.first};
}
std::pair<QString, int> choose(const QStringList& opts) {
    return choose(std::vector(opts.constBegin(), opts.constEnd()));
}

const QRegularExpression groupsRe("%([a-zA-Z_]+)%?");
const QRegularExpression dictRe("\\$([a-zA-Z0-9_]+)\\$?");

const QRegularExpression polishRe("{([^}]+)}");
QString Conversation::polishSentence(QString sent) {
    // Replace synonym choices in {brackets/braces}
    auto it = dictRe.globalMatch(sent);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        QString repl = QString::fromStdString(cconfig()["dictionary"][m.captured(1).toStdString()].as<std::string>());

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        sent.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    it = polishRe.globalMatch(sent);
    offs = 0;
    while (it.hasNext()) {
        auto m = it.next();
        auto opts = m.captured(1).split('/');
        QString repl = choose(opts).first;

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        sent.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }
    it = groupsRe.globalMatch(sent);
    offs = 0;
    const auto& exts = externs();
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
    auto ppse = cconfig()["purposes"][purpose];
    if (!ppse) {
        display("Purpose '"+QString::fromStdString(purpose)+"' does not exist!");
        return;
    }
    /// [(sentence, idx of options in opts list), ...]
    std::vector<std::pair<std::string, uint>> sents;
    auto matches = [&](const std::string& req) {
        if (req == "=") {
            return sents.empty();
        } else if (req == "*" || req == "") {
            return true;
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
    std::vector<optList> opts;
    uint idx = 0;
    for (const auto& opt : ppse) {
        // Compile the options into a list
        optList outopts;
        for (const auto& item : opt["opts"]) {
            QString title = polishSentence(QString::fromStdString(item[0].as<std::string>()));
            if (title.isEmpty()) continue;
            std::string npurp = "";
            if (item.size() > 2) npurp = item[2].as<std::string>();
            outopts.push_back({ title,
                item[1].as<std::unordered_set<std::string>>(),
                npurp
            });
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
                    good = matches(token);
                }
                if (next == std::string::npos) break;
            }
            if (good) {
                if (tmpl.second.IsScalar()) {
                    sents.push_back({tmpl.second.as<std::string>(), idx});
                } else {
                    for (const auto& it : tmpl.second) {
                        sents.push_back({it.as<std::string>(), idx});
                    }
                }
            }
        }
        idx++;
    }

    const auto& exts = externs();
    std::vector<QString> outsents;
    std::vector<uint> outoptidxs;
    for (const auto& sent : sents) {
        auto qstr = QString::fromStdString(sent.first);
        // Check all context group tags exist
        auto it = groupsRe.globalMatch(qstr);
        bool good = true;
        while (it.hasNext() && good) {
            auto m = it.next();
            std::string group = m.captured(1).toStdString();
            if (exts.find(group) == exts.end()) { // Ensure the group is not external
                good = false;
                const auto& grps = groups();
                const auto& it2 = grps.find(group);
                if (it2 == grps.end()) break;
                for (const auto& val : it2->second) {
                    if (context.find(val) != context.end()) good = true; break;
                }
            }
        }
        if (!good) { continue; }
        outsents.push_back(qstr);
        outoptidxs.push_back(sent.second);
    }

    auto out = choose(outsents);
    if (out.second == -1) {
        display("No sentences avaliable!");
        return;
    }
    if (out.first.startsWith("> ")) {
        purpose = polishSentence(out.first.sliced(2)).toStdString();
        return refresh();
    }
    display(polishSentence(out.first), opts.at(outoptidxs.at(out.second)));
}

void Conversation::display(QString title, optList opts) {
    curtxt->setText(title);
    setOpts(olay, opts, [=](Option o){ onclick(o); });
}
