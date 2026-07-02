#include "game.hpp"
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
        bk->setProperty("tinybtn", true);
        bk->setIcon(QIcon(":/assets/back.svg"));
        bk->setIconSize(QSize(48, 48));
        connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
        topsect->addWidget(bk);
        {auto labl = new QLabel("Settings");
        labl->setAlignment(Qt::AlignCenter);
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        topsect->addWidget(labl);}
    slay->addLayout(topsect);

    {auto opts = new QVBoxLayout();
        {auto labl = new QLabel("Bottom section");
        opts->addWidget(labl);}
    auto sp = new Spoiler("Title");
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    {auto opts = new QVBoxLayout();
        {auto labl = new QLabel("Bottom section2");
        opts->addWidget(labl);}
    auto sp = new Spoiler("Title2");
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    slay->addStretch();
}
