#include "quals.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>

std::unordered_map<QString, qualityTyp> _quals() {
    QFile file(":/data/quals.yml");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    const auto config = YAML::Load(file.readAll().toStdString());

    std::unordered_map<QString, qualityTyp> out;
    for (const auto& q : config) {
        qualityTyp parts;
        for (const auto& item : config) {
            parts.insert({item.first.as<int>(), item.second.as<std::unordered_set<std::string>>()});
        }
        out.insert({QString::fromStdString(q.first.as<std::string>()), parts});
    }
    return out;
}
std::unordered_map<QString, qualityTyp> qualities() {
    static const auto out = _quals();
    return out;
}
std::set<QString> qualkeys() {
    std::set<QString> out;
    for (const auto& [k, _] : qualities()) {
        out.insert(k);
    }
    return out;
}
