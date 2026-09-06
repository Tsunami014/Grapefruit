#include "game.hpp"
#include <QFile>

const QColor base(236, 161, 211);
const bool light = true;


enum PalleteOpts {
    PRIMARY,
    SECONDARY,
    TERTIARY,
    NEUTRAL,
    NEUTRALVARIANT,
};

QString getCol(float bhue, PalleteOpts palstyl, float tone) {
    switch (palstyl) {
        case PRIMARY:
            return QColor::fromHslF(bhue, 0.48, tone).name();
        case SECONDARY:
            return QColor::fromHslF(bhue, 0.16, tone).name();
        case TERTIARY:
            return QColor::fromHslF(std::fmod(bhue+60.0, 360.0), 0.24, tone).name();
        case NEUTRAL:
            return QColor::fromHslF(bhue, 0.4, tone).name();
        case NEUTRALVARIANT:
            return QColor::fromHslF(bhue, 0.8, tone).name();
    }
    return {};
}

void MainGame::genStyle() {
    float bhue = base.hueF();

    QFile file(":/style.qss");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    QString styl = QString::fromUtf8(file.readAll())
        .replace("$txt", getCol(bhue, PRIMARY, light? 0.10:0.90))
        .replace("$invtxt", getCol(bhue, PRIMARY, light? 1.0:0.20))
        .replace("$bg", getCol(bhue, NEUTRAL, light? 0.98:0.06))
        .replace("$cardbg", getCol(bhue, NEUTRAL, light? 0.94:0.12))
    ;
    setStyleSheet(styl);
}
