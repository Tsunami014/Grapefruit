#include "game.hpp"
#include "base/taskload.hpp"
#include "wids/taskbbl.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

void MainGame::taskOverlay(std::shared_ptr<Task> t) {
}

void MainGame::generateTasks() {
    tlay = new QGridLayout(tasks);
    auto mtlay = new QVBoxLayout();
    tlay->addLayout(mtlay, 0, 0);

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
    mtlay->addLayout(topsect);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}

    for (const auto& t : taskslist) {
        auto bub = new TaskBubble(t, this);
        connect(bub, &TaskBubble::clicked, this, [=](){ taskOverlay(t); });
        mtlay->addWidget(bub);
    }

    mtlay->addStretch();
}
