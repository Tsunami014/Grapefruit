#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
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
            bk->setIconSize(QSize(48, 44));
            connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            left->addWidget(bk, 0, Qt::AlignHCenter);
            {auto labl = new QLabel("All\ntasks");
            resizeFont(labl, 1.25);
            labl->setAlignment(Qt::AlignCenter);
            left->addWidget(labl);}
        topsect->addLayout(left);
        addLine();

        auto tclcont = new QWidget();
        tclcont->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        auto tcatlay = new FlowLayout(tclcont);
        setTasksCatsLay(tcatlay, tclcont);
        topsect->addWidget(tclcont);

        addLine();
        auto rnam = new QPushButton();
        rnam->setProperty("fancy", true);
        rnam->setProperty("backbtn", true);
        rnam->setIcon(QIcon(":/assets/UI/rename.svg"));
        rnam->setIconSize(QSize(48, 44));
        rnam->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        connect(rnam, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
        topsect->addWidget(rnam);
        auto right = new QVBoxLayout();
        right->setSpacing(8);
            auto plus = new QPushButton();
            plus->setProperty("fancy", true);
            plus->setProperty("backbtn", true);
            plus->setIcon(QIcon(":/assets/UI/plus.svg"));
            plus->setIconSize(QSize(48, 44));
            connect(plus, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            right->addWidget(plus);

            auto bin = new QPushButton();
            bin->setProperty("fancy", true);
            bin->setProperty("backbtn", true);
            bin->setIcon(QIcon(":/assets/UI/bin.svg"));
            bin->setIconSize(QSize(48, 44));
            connect(bin, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            right->addWidget(bin);
        topsect->addLayout(right);
    mtlay->addLayout(topsect);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}

    auto tbbllay = new QVBoxLayout();
    setTasksLay(tbbllay, tlay, this);
    mtlay->addLayout(tbbllay);

    mtlay->addStretch();
}
