#include "converse.hpp"
#include <yaml-cpp/yaml.h>
#include <QRandomGenerator>
#include <QFile>

static YAML::Node config() {
    QFile file(":/data/conv.yml");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from internal files
    return YAML::Load(file.readAll().toStdString());
}
static std::unordered_set<std::string> keeps() {
    std::unordered_set<std::string> kps;
    auto groups = config()["groups"];
    for (const auto& entry : groups) {
        std::string key = entry.first.as<std::string>();
        if (key.size() > 0 && key.at(0) == '=' && entry.second.IsSequence()) {
            for (const auto& item : entry.second) {
                kps.insert(item.as<std::string>());
            }
        }
    }
    return kps;
}

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {
        refresh();
    }

void Conversation::newTopic() {
    auto kp = keeps();
    context.erase(
        std::remove_if(context.begin(), context.end(),
            [&](std::string s) {
                return kp.find(s) == kp.end();
            }
        ), context.end()
    );
    refresh();
}
void Conversation::onclick(Option o) {
    refresh();
}

void Conversation::refresh() {
    auto ppse = config()["purposes"][purpose];
    if (!ppse) {
        display("Purpose '"+QString::fromStdString(purpose)+"' does not exist!");
        return;
    }
    auto allopts = ppse["options"];
    if (!allopts || allopts.size() == 0) {
        display("No options avaliable!");
        return;
    }
    uint idx = QRandomGenerator::global()->bounded(uint(allopts.size()));

    optList outopts;
    for (const auto& item : allopts[idx]) {
        outopts.push_back({QString::fromStdString(item[0].as<std::string>())});
    }
    display("Hello!", outopts);
}

void Conversation::display(QString title, optList opts) {
    curtxt->setText(title);
    setOpts(olay, opts, [=](Option o){ onclick(o); });
}
