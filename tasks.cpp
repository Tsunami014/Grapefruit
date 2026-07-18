#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
#include "wids/taskbbl.hpp"
#include "wids/taskOverlay.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

void MainGame::generateTasks() {
    tlay = new QGridLayout(tasks);
    tlay->setContentsMargins(0, 0, 0, 0);
    auto mtlay = new QVBoxLayout();
    mtlay->setContentsMargins(9, 9, 9, 9);
    tlay->addLayout(mtlay, 0, 0);

    auto topsect = new QHBoxLayout();
    auto addLine = [&](){
        QFrame* line = new QFrame();
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Plain);
        topsect->addWidget(line);
    };
    topsect->setSpacing(16);
        auto left = new QVBoxLayout();
        left->setSpacing(8);
            auto bk = new QPushButton();
            bk->setProperty("fancy", true);
            bk->setProperty("backbtn", true);
            bk->setIcon(QIcon(":/assets/UI/back.svg"));
            bk->setIconSize(QSize(48, 40));
            connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            left->addWidget(bk, 0, Qt::AlignHCenter);
            {auto labl = new QLabel("All\ntasks");
            resizeFont(labl, 1.25);
            labl->setAlignment(Qt::AlignCenter);
            left->addWidget(labl);}
        topsect->addLayout(left);
        addLine();

        topsect->addStretch();

        addLine();
        auto right = new QVBoxLayout();
        right->setSpacing(8);
            auto plus = new QPushButton();
            plus->setProperty("fancy", true);
            plus->setProperty("backbtn", true);
            plus->setIcon(QIcon(":/assets/UI/plus.svg"));
            plus->setIconSize(QSize(48, 40));
            connect(plus, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            right->addWidget(plus);

            auto bin = new QPushButton();
            bin->setProperty("fancy", true);
            bin->setProperty("backbtn", true);
            bin->setIcon(QIcon(":/assets/UI/bin.svg"));
            bin->setIconSize(QSize(48, 40));
            connect(bin, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            right->addWidget(bin);
        topsect->addLayout(right);
    mtlay->addLayout(topsect);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}

    for (const auto& t : taskslist) {
        auto bub = new TaskBubble(t, this);
        connect(bub, &TaskBubble::clicked, this, [=](){
            tlay->addWidget(new TaskOverlay(t, this), 0, 0);
        });
        mtlay->addWidget(bub);
    }

    mtlay->addStretch();
}
