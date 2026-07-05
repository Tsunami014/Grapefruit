#pragma once
#include <QWidget>

inline void resizeFont(QWidget* wid, float perc) {
    QFont f = wid->font();
    f.setPointSize(f.pointSize() * perc);
    wid->setFont(f);
}
