#include "scenes.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>

template<typename F> const auto& cached(F&& init) {
    static const auto value = init();
    return value;
}
const YAML::Node& sconfig() {
    return cached([]{
        QFile file(":/data/scenes.yml");
        bool ok = file.open(QIODevice::ReadOnly);
        // Should always be ok because we're loading from a preset internal file
        return YAML::Load(file.readAll().toStdString());
    });
}

/// A mapping of items to their true paths
const std::unordered_map<std::string, QString>& paths() {
    return cached([]{
        std::unordered_map<std::string, QString> pths;
        for (const auto& entry : sconfig()["assets"]) {
            QString nam = QString::fromStdString(entry.first.as<std::string>()) + '/';
            for (const auto& item : entry.second) {
                auto str = item.as<std::string>();
                pths.insert({str, nam+QString::fromStdString(str)});
            }
        }
        return pths;
    });
}

QStringList getSceneItems(QString scene) {
    auto its = sconfig()["scenes"][scene.toStdString()];
    if (!its) return {};
    QStringList out;
    for (const auto& item : its) {
        out << paths().at(item[0].as<std::string>());
    }
    return out;
}
