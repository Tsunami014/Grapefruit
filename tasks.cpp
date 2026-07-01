#include "game.hpp"
#include "wids/taskbbl.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

void MainGame::generateTasks() {
    auto tlay = new QVBoxLayout(tasks);

    auto topsect = new QHBoxLayout();
    topsect->setSpacing(16);
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
        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Plain);
        topsect->addWidget(line);}
        topsect->addStretch();
    tlay->addLayout(topsect);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    tlay->addWidget(line);}

    {auto bub = new TaskBubble("Hi", "bye", this);
    tlay->addWidget(bub);}
    {auto bub2 = new TaskBubble("Hi2", "bye2", this);
    tlay->addWidget(bub2);}

    tlay->addStretch();
}
