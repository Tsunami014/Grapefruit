#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
#include "wids/renameOverl.hpp"
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
    auto addBtn = [&](QString asset){
        auto btn = new QPushButton();
        btn->setProperty("fancy", true);
        btn->setProperty("backbtn", true);
        btn->setIcon(QIcon(asset));
        btn->setIconSize(QSize(48, 44));
        return btn;
    };
    topsect->setSpacing(16);
        auto left = new QVBoxLayout();
        left->setSpacing(8);
            auto bk = addBtn(":/assets/UI/back.svg");
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
        topsect->addWidget(tclcont);

        addLine();
        auto rnam = addBtn(":/assets/UI/rename.svg");
        rnam->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
        connect(rnam, &QPushButton::clicked, this, [this](){
            tlay->addWidget(new RenameOverlay("hi", [](QString){}), 0, 0);
        });
        topsect->addWidget(rnam);
        auto right = new QVBoxLayout();
        right->setSpacing(8);
            auto plus = addBtn(":/assets/UI/plus.svg");
            connect(plus, &QPushButton::clicked, this, [this](){
                tlay->addWidget(new RenameOverlay("hi", [](QString){}), 0, 0);
            });
            right->addWidget(plus);

            auto bin = addBtn(":/assets/UI/bin.svg");
            connect(bin, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            right->addWidget(bin);
        topsect->addLayout(right);
    mtlay->addLayout(topsect);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}

    auto tbbllay = new QVBoxLayout();
    mtlay->addLayout(tbbllay);
    mtlay->addStretch();

    std::function<void()> redo;
    redo = [tbbllay, this, &redo, tcatlay, tclcont](){
        setTasksLay(tbbllay, tlay, this);
        setTasksCatsLay(tcatlay, redo, tclcont);
    };
    redo();
}
