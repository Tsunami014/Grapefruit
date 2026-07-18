#include "game.hpp"
#include "base/taskload.hpp"
#include "wids/taskOverlay.hpp"
#include "wids/renameOverl.hpp"
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
        btn->setIcon(QIcon(asset));
        btn->setIconSize(QSize(48, 44));
        return btn;
    };
    topsect->setSpacing(16);
        auto left = new QVBoxLayout();
        left->setSpacing(8);
            auto bk = addBtn(":/assets/UI/back.svg");
            bk->setProperty("backbtn", true);
            connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
            left->addWidget(bk);

            auto bin = addBtn(":/assets/UI/bin.svg");
            connect(bin, &QPushButton::clicked, this, [this](){
                if (deleteCategory(this)) redoTasks();
            });
            left->addWidget(bin);
        topsect->addLayout(left);
        addLine();

        auto tclcont = new QWidget();
        tclcont->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        tcatlay = new FlowLayout(tclcont);
        topsect->addWidget(tclcont);

        addLine();
        auto right = new QVBoxLayout();
        right->setSpacing(8);
            auto plus = addBtn(":/assets/UI/plus.svg");
            connect(plus, &QPushButton::clicked, this, [this](){
                overlay = new RenameOverlay("New category", "", [this](QString s){
                    newCategory(this, s);
                    redoTasks();
                });
                tlay->addWidget(overlay, 0, 0);
            });
            right->addWidget(plus);

            auto rnam = addBtn(":/assets/UI/rename.svg");
            rnam->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Ignored);
            connect(rnam, &QPushButton::clicked, this, [this](){
                QString cur = getCurrent();
                overlay = new RenameOverlay("Rename category '" + cur + "'", cur, [this](QString s){
                    if (renameCategory(this, s)) redoTasks();
                });
                tlay->addWidget(overlay, 0, 0);
            });
            right->addWidget(rnam);
        topsect->addLayout(right);
    mtlay->addLayout(topsect);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}

    tbbllay = new QVBoxLayout();
    mtlay->addLayout(tbbllay);
    mtlay->addStretch();

    redoTasks();
}

void MainGame::redoTasks() {
    auto redo = [this](){redoTasks();};
    setTasksLay(tbbllay, [=](std::shared_ptr<Task> t, bool upd){
        overlay = new TaskOverlay(t, redo, this);
        tlay->addWidget(overlay, 0, 0);
        if (upd) redoTasks();
    }, this);
    setTasksCatsLay(tcatlay, redo, this);
    if (overlay) overlay->raise();
}
