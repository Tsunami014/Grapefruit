#include "quals.hpp"
#include "font.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>
#include <QPushButton>

std::unordered_map<QString, qualityTyp> _quals() {
    QFile file(":/data/quals.yml");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    const auto config = YAML::Load(file.readAll().toStdString());

    std::unordered_map<QString, qualityTyp> out;
    for (const auto& q : config) {
        qualityTyp parts;
        for (const auto& item : q.second) {
            parts.insert({item.first.as<int>(), item.second.as<std::unordered_set<std::string>>()});
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

void addQualityBtns(QLayout* lay, QWidget* parent) {
    for (const auto& k : qualkeys()) {
        auto btn = new QPushButton(k, parent);
        resizeFont(btn, 1.3);
        btn->setProperty("fancy", true);
        btn->setProperty("tinybtn", true);
        btn->setFocusPolicy(Qt::NoFocus);
        lay->addWidget(btn);
    }
}
