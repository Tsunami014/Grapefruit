#include "scenes.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>
#include <QDebug>

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

std::vector<SceneItem> gsis(QStringList parts, YAML::Node its, std::vector<SceneItem> out = {}) {
    auto pths = paths();
    for (const auto& item : its) {
        if (item.IsMap()) {
            auto key = QString::fromStdString(item.begin()->first.as<std::string>());
            bool inv = key[0] == '^';
            if (inv) key = key.mid(1);
            if (parts.contains(key) ^ inv) {
                out = gsis(parts, item.begin()->second, out);
            }
        } else {
            auto nam = item[0].as<std::string>();
            bool flip = nam[0] == '~';
            if (flip) nam = nam.substr(1);
            QPointF pos(item[1].as<float>(), item[2].as<float>());
            auto p = pths.find(nam);
            if (p == pths.end()) {
                qFatal() << "Object not defined:" << nam;
                return {};
            }
            out.push_back({p->second, pos, flip});
        }
    }
    return out;
}

std::vector<SceneItem> getSceneItems(QString scene) {
    QStringList parts = scene.split('#');
    auto its = sconfig()["scenes"][parts[0].toStdString()];
    if (!its) return {};
    return gsis(parts.sliced(1), its);
}
