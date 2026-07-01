#include "game.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <qboxlayout.h>

void MainGame::generateTasks() {
    auto tlay = new QVBoxLayout(tasks);

    auto topsect = new QHBoxLayout();
        auto left = new QVBoxLayout();
            auto bk = new QPushButton();
            bk->setProperty("isbtn", true);
            bk->setIcon(QIcon(":/assets/back.svg"));
            bk->setIconSize(QSize(48, 48));
            connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            left->addWidget(bk);
            {auto labl = new QLabel("All\ntasks");
            labl->setAlignment(Qt::AlignCenter);
            left->addWidget(labl);}
        topsect->addLayout(left);
        topsect->addStretch();
    tlay->addLayout(topsect);

    tlay->addStretch();
}
