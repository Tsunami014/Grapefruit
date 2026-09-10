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
    } else if (name == "col") {
        MG->base = QColor::fromString(conts);
    } else if (name == "cth") {
        bool ok;
        int num = conts.toUInt(&ok);
        if (ok) {
            if (num >= Themes::Count) {
                qWarning() << "Colour theme value out of range:" << num;
            } else {
                MG->colthm = static_cast<Themes::Theme>(num);
            }
        } else {
            qWarning() << "Colour theme value not an unsigned number:" << conts;
        }
    } else {
        qWarning() << "Unknown setting key:" << name;
    }
}
std::unordered_map<QString, QString> saveSettings() {
    std::unordered_map<QString, QString> out;
    out["thm"] = QString::number(MG->theme+1);
    out["col"] = MG->base.name();
    out["cth"] = QString::number(MG->colthm);
    return out;
}
