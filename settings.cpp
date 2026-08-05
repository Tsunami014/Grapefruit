#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
#include "wids/secret.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

void MainGame::generateSettings() {
    auto slay = new QVBoxLayout(setts);
    slay->setSpacing(16);

    auto topsect = new QHBoxLayout();
        auto bk = new QPushButton();
        bk->setProperty("fancy", true);
        bk->setProperty("backbtn", true);
        bk->setIcon(QIcon(":/assets/UI/back.svg"));
        bk->setIconSize(QSize(48, 40));
        connect(bk, &QPushButton::clicked, this, [this](){ toMain(); });
        topsect->addWidget(bk, 0, Qt::AlignHCenter);
        {auto labl = new QLabel("Settings");
        resizeFont(labl, 1.5);
        labl->setAlignment(Qt::AlignCenter);
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        topsect->addWidget(labl);}
    slay->addLayout(topsect);

    {auto sp = new Spoiler("Title", this);
    auto opts = new QVBoxLayout();
        {auto labl = new QLabel("Bottom section", sp);
        opts->addWidget(labl);}
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    {auto sp = new Spoiler("Debug", this);
    auto opts = new QVBoxLayout();
        auto labl = new QLabel("--Task info--", sp);
        opts->addWidget(labl);

        auto btn = new QPushButton("Reload", sp);
        btn->connect(btn, &QPushButton::pressed, labl, [=](){
            labl->setText(getAllTasksDebugInfo());
            sp->updateHeights();
        });
        opts->addWidget(btn);
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    slay->addStretch();
}
