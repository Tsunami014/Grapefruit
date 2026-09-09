#include "setts.hpp"
#include "game.hpp"

void loadSetting(QString name, QString conts) {
    if (name == "thm") {
        bool ok;
        int num = conts.toInt(&ok);
        if (ok) {
            if (num < 0 || num > 2) {
                qWarning() << "Theme value out of range:" << num;
            } else {
                MG->theme = num-1;
            }
        } else {
            qWarning() << "Theme value not a number:" << conts;
        }
    } else {
        qWarning() << "Unknown setting key:" << name;
    }
}
std::unordered_map<QString, QString> saveSettings() {
    std::unordered_map<QString, QString> out;
    out["thm"] = QString::number(MG->theme+1);
    return out;
}
