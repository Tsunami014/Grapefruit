#include "itemopts.hpp"
#include <QLayoutItem>
#include <QLabel>
#include <QPushButton>

void GenerateOpts(QWidget* parent, QBoxLayout* lay, bool focus) {
    QLayoutItem *item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    if (focus) {
        QSize sze(40,40);

        auto bk = new QPushButton(parent);
        bk->setProperty("fancy", true);
        bk->setProperty("tinybtn", true);
        bk->setIcon(QIcon(":/assets/back.svg"));
        bk->setIconSize(sze);
        bk->setFocusPolicy(Qt::NoFocus);
        lay->addWidget(bk);
    }
    lay->addStretch();
}
