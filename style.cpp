#include "game.hpp"
#include <QFile>

QString styleConts() {
    QFile file(":/style.qss");
    bool ok = file.open(QIODevice::ReadOnly);
    // Should always be ok because we're loading from a preset internal file
    return QString::fromUtf8(file.readAll());
}

void StyleWidget(QWidget* wid) {
    static const QString& sc = styleConts();
    wid->setStyleSheet(sc);
}
