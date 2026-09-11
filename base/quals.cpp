#include "quals.hpp"
#include "game.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>
#include <QRandomGenerator>

QMap<QString, qualityTyp> _quals() {
    QFile file(":/data/quals.yml");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    auto conf = YAML::Load(file.readAll().toStdString());

    QMap<QString, qualityTyp> out;
    for (const auto& q : conf) {
        qualityTyp::_qualityListTyp parts;
        bool fact = false;
        for (const auto& item : q.second) {
            if (item.first.as<std::string>() == "fact") {
                fact = item.second.as<bool>();
                continue;
            }
            auto vec = item.second.as<std::vector<std::string>>();
            parts.insert({ item.first.as<int>(), {vec.begin(), vec.end()} });
        }
        out.insert(QString::fromStdString(q.first.as<std::string>()),
            { fact, parts }
        );
    }
    return out;
}
const QMap<QString, qualityTyp>& qualities() {
    static const auto out = _quals();
    return out;
}

bool isQualFact(QString qnam) {
    auto qs = qualities();
    for (auto it = qs.cbegin(); it != qs.cend(); ++it) {
        if (it.key() == qnam) return it.value().fact;
    }
    return false;
}

std::set<QString> _qkeys() {
    std::set<QString> out;
    auto qs = qualities();
    for (auto it = qs.cbegin(); it != qs.cend(); ++it) {
        out.insert(it.key());
    }
    return out;
}
const std::set<QString>& qualkeys() {
    static const auto out = _qkeys();
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
        for (const auto& [k, li] : allquals.value(q).map) {
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
