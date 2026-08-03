#include "quals.hpp"
#include "game.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>
#include <QRandomGenerator>

YAML::Node _qualconf() {
    QFile file(":/data/quals.yml");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    return YAML::Load(file.readAll().toStdString());
}
YAML::Node qualconf() {
    static const auto out = _qualconf();
    return out;
}

std::unordered_map<QString, qualityTyp> _quals() {
    std::unordered_map<QString, qualityTyp> out;
    for (const auto& q : qualconf()) {
        qualityTyp parts;
        for (const auto& item : q.second) {
            if (item.first.as<std::string>() == "good") continue;
            auto vec = item.second.as<std::vector<std::string>>();
            parts.insert({ item.first.as<int>(), {vec.begin(), vec.end()} });
        }
        out.insert({QString::fromStdString(q.first.as<std::string>()), parts});
    }
    return out;
}
const std::unordered_map<QString, qualityTyp>& qualities() {
    static const auto out = _quals();
    return out;
}

std::set<QString> _qkeys() {
    std::set<QString> out;
    for (const auto& [k, _] : qualities()) {
        out.insert(k);
    }
    return out;
}
const std::set<QString>& qualkeys() {
    static const auto out = _qkeys();
    return out;
}

std::unordered_set<QString> _goodqs() {
    std::unordered_set<QString> out;
    for (const auto& q : qualconf()) {
        qualityTyp parts;
        if (q.second["good"].as<bool>())
            out.insert(QString::fromStdString(q.first.as<std::string>()));
    }
    return out;
}
const std::unordered_set<QString>& goodquals() {
    static const auto out = _goodqs();
    return out;
}

double scoreQualities(std::set<QString> quals) {
    if (MG == nullptr) return 0.5;
    auto ctx = MG->conv->getContext();
    auto allquals = qualities();
    double sco = 0.0;
    double numscos = 0.0;
    for (const auto& q : quals) {
        int tsco = 0;
        for (const auto& [k, li] : allquals.at(q)) {
            for (const auto& it : li) {
                if (ctx.find(it) != ctx.end()) {
                    tsco += k;
                    break;
                }
            }
        }
        sco += double((tsco > 0) - (tsco < 0)); // Gets the sign; -1, 0, or 1
        numscos++;
    }
    if (numscos == 0) return 0.5;
    return (sco/numscos)+0.5;
}

std::pair<std::vector<QString>, int> bestQualities(std::set<QString> quals) {
    if (MG == nullptr) return {};
    auto ctx = MG->conv->getContext();
    auto allquals = qualities();
    auto goods = goodquals();
    std::vector<QString> best;
    int bestsco = std::numeric_limits<int>::min();
    for (const auto& q : quals) {
        if (goods.find(q) == goods.end()) continue;
        int tsco = QRandomGenerator::global()->bounded(-1, 1);
        for (const auto& [k, li] : allquals.at(q)) {
            for (const auto& it : li) {
                if (ctx.find(it) != ctx.end()) {
                    tsco += k;
                    break;
                }
            }
        }
        if (tsco > bestsco) {
            best.clear();
            bestsco = tsco;
        }
        if (tsco >= bestsco) {
            best.push_back(q);
        }
    }
    return {best, bestsco};
}
