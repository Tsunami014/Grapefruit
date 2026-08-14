#include "tasks.hpp"
#include "game.hpp"
#include "base/taskload.hpp"
#include "extra/help.hpp"
#include "wids/taskOverlay.hpp"
#include "wids/renameOverl.hpp"
#include "wids/confirm.hpp"
#include <QBoxLayout>
#include <QPushButton>

TaskView::TaskView() {
    tlay = new QGridLayout(this);
    tlay->setContentsMargins(0, 0, 0, 0);
    auto mtlay = new QVBoxLayout();
    mtlay->setContentsMargins(9, 9, 9, 9);
    mtlay->setSpacing(0);
    tlay->addLayout(mtlay, 0, 0);

    tbbllay = new QVBoxLayout();
    mtlay->addLayout(tbbllay);
    mtlay->addStretch();
    mtlay->addSpacing(8);
    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}
    mtlay->addSpacing(8);

    auto topsect = new QHBoxLayout();
    auto addBtn = [&](QString asset){
        auto btn = new QPushButton();
        btn->setProperty("fancy", true);
        btn->setIcon(QIcon(asset));
        btn->setIconSize(QSize(48, 44));
        btn->setMinimumHeight(56);
        return btn;
    };

    {auto* labl = new QLabel("Categories", this);
    labl->setContentsMargins(4,8,4,8);
    labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    labl->setAlignment(Qt::AlignCenter);
    mtlay->addWidget(labl);}

    scrl = new QScrollArea(this);
    scrl->setFrameShape(QFrame::NoFrame);
    scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    scrl->setProperty("bg", true);

    scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrl->horizontalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    scrl->horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);
    tcatdrag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());

    auto* catcont = new QWidget(this);
    catcont->setObjectName("transpbg");
    tcatlay = new FlowLayout(catcont);
    tcatlay->vertical(2);
    scrl->setWidget(catcont);
    scrl->setWidgetResizable(true);
    mtlay->addWidget(scrl);
    mtlay->addSpacing(8);

    auto bot = new QHBoxLayout();
    bot->setSpacing(8);
        {auto bk = addBtn(":/assets/UI/back.svg");
        bk->setProperty("backbtn", true);
        connect(bk, &QPushButton::clicked, this, [=](){ MG->toMain(); });
        bot->addWidget(bk);}

        {auto plus = addBtn(":/assets/UI/plus.svg");
        plus->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        connect(plus, &QPushButton::clicked, this, [this](){
            overlay = new RenameOverlay("New category", "", [this](QString s){
                newCategory(this, s.trimmed());
                redoTasks();
            });
            tlay->addWidget(overlay, 0, 0);
        });
        bot->addWidget(plus);}

        {auto rnam = addBtn(":/assets/UI/rename.svg");
        rnam->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        connect(rnam, &QPushButton::clicked, this, [this](){
            QString cur = getCurrent();
            overlay = new RenameOverlay("Rename category '" + cur + "'", cur, [this](QString s){
                if (renameCategory(this, s.trimmed())) redoTasks();
            });
            tlay->addWidget(overlay, 0, 0);
        });
        bot->addWidget(rnam);}

        {auto bin = addBtn(":/assets/UI/bin.svg");
        bin->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        connect(bin, &QPushButton::clicked, this, [this](){
            if (deleteCategory(this)) redoTasks();
        });
        bot->addWidget(bin);}

        {auto help = addBtn(":/assets/UI/help.svg");
        help->setProperty("backbtn", true);
        connect(help, &QPushButton::clicked, this, [this](){
            confirm(this, TASK_HELP, Conf_OK, true);
        });
        bot->addWidget(help);}
    mtlay->addLayout(bot);

    redoTasks();
}

void TaskView::redoTasks() {
    auto redo = [this](){ redoTasks(); };
    setTasksLay(tbbllay, [=](std::shared_ptr<Task> t, bool upd){
        overlay = new TaskOverlay(t, redo, this);
        tlay->addWidget(overlay, 0, 0);
        if (upd) redoTasks();
    }, this);
    setTasksCatsLay(tcatlay, redo, this);
    tcatdrag->installOn(tcatlay);
    tcatlay->activate();
    scrl->widget()->setMinimumWidth(tcatlay->lastSize().width() + 8);

    QTimer::singleShot(0, [this](){
        const int w = scrl->viewport()->width();
        const int h = tcatlay->heightForWidth(w);
        scrl->setFixedHeight(h);
    });
    if (overlay) overlay->raise();
}
