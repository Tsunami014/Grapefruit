#include "converse.hpp"
#include <yaml-cpp/yaml.h>
#include <QFile>

static YAML::Node config() {
    QFile file(":/data/conv.yml");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from internal files
    return YAML::Load(file.readAll().toStdString());
}

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {
        refresh();
    }

void Conversation::newTopic() {
    refresh();
}
void Conversation::onclick(Option o) {
    refresh();
}

void Conversation::refresh() {
    auto root = config();
    auto allopts = root["purposes"][purpose]["options"];
    if (!allopts) {
        display("Something went wrong fetching options list!");
        return;
    }
    auto opts = allopts[0];
    if (!opts) {
        display("No options avaliable!");
        return;
    }

    optList outopts;
    for (const auto& item : opts) {
        outopts.push_back({QString::fromStdString(item[0].as<std::string>())});
    }
    display("Hello!", outopts);
}

void Conversation::display(QString title, optList opts) {
    curtxt->setText(title);
    setOpts(olay, opts, [=](Option o){ onclick(o); });
}
