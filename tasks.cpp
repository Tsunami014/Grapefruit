#include "tasks.hpp"
#include "game.hpp"
#include "base/taskload.hpp"
#include "extra/help.hpp"
#include "wids/taskOverlay.hpp"
#include "wids/renameOverl.hpp"
#include "wids/confirm.hpp"
#include "font.hpp"
#include <QBoxLayout>

TaskView::TaskView() {
    auto mtlay = new QVBoxLayout(this);
    mtlay->setContentsMargins(9, 9, 9, 9);
    mtlay->setSpacing(0);

    auto addBtn = [&](QString asset){
        auto btn = new QPushButton();
        btn->setProperty("fancy", true);
        btn->setIcon(QIcon(asset));
        btn->setIconSize(QSize(48, 44));
        btn->setMinimumHeight(56);
        return btn;
    };

    auto toplay = new QHBoxLayout();
        {auto bin = addBtn(":/assets/UI/bin.svg");
        connect(bin, &QPushButton::clicked, this, [this](){
            if (deleteCategory(this)) redoTasks();
        });
        toplay->addWidget(bin);}

        topheader = new QLabel(this);
        resizeFont(topheader, 1.5);
        topheader->setContentsMargins(4,4,4,8);
        topheader->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        topheader->setAlignment(Qt::AlignCenter);
        toplay->addWidget(topheader);

        newtaskbtn = addBtn(":/assets/UI/plus.svg");
        connect(newtaskbtn, &QPushButton::clicked, this, [this](){
            overlay = new TaskOverlay(newtask(), [this](){ redoTasks(); }, this, window());
            overlay->show();
            redoTasks();
        });
        toplay->addWidget(newtaskbtn);
    mtlay->addLayout(toplay);

    {auto tskscrl = new QScrollArea(this);
    tskscrl->setFrameShape(QFrame::NoFrame);
    tskscrl->setProperty("bg", true);

    tskscrl->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    tskscrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tskscrl->verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    tskscrl->verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
    tdrag = new DragScroll(tskscrl->viewport(), tskscrl->verticalScrollBar());

    auto* tcont = new QWidget(this);
    tcont->setObjectName("transpbg");
    tbbllay = new QVBoxLayout(tcont);
    tbbllay->setSpacing(8);
    tbbllay->setAlignment(Qt::AlignTop);
    tskscrl->setWidget(tcont);
    tskscrl->setWidgetResizable(true);
    mtlay->addWidget(tskscrl);}
    mtlay->addSpacing(8);

    {QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Raised);
    mtlay->addWidget(line);}
    mtlay->addSpacing(8);

    auto titlay = new QHBoxLayout();
        titlay->addStretch();
        starbtn = new QPushButton("Starred", this);
        starbtn->setProperty("fancy", true);
        starbtn->setProperty("optbtn", true);
        starbtn->setProperty("current", isStarCat());
        resizeFont(starbtn, 1.2);
        QObject::connect(starbtn, &QPushButton::clicked, [this](){
            if (isStarCat()) showNoCat();
            else showStar();
            redoTasks();
        });
        titlay->addWidget(starbtn);
        titlay->addStretch();

        {auto* labl = new QLabel("Categories", this);
        resizeFont(labl, 1.2);
        labl->setContentsMargins(4,8,4,8);
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        labl->setAlignment(Qt::AlignCenter);
        titlay->addWidget(labl);}
        titlay->addStretch();
    mtlay->addLayout(titlay);
    mtlay->setSpacing(8);

    auto bot = new QHBoxLayout();
    bot->setSpacing(8);
        {auto vlay = new QVBoxLayout();
            {auto help = addBtn(":/assets/UI/help.svg");
            help->setProperty("backbtn", true);
            connect(help, &QPushButton::clicked, this, [this](){
                confirm(this, TASK_HELP, Conf_OK, true);
            });
            vlay->addWidget(help);}

            {auto bk = addBtn(":/assets/UI/back.svg");
            bk->setProperty("backbtn", true);
            connect(bk, &QPushButton::clicked, this, [=](){ MG->toMain(); });
            vlay->addWidget(bk);}
        bot->addLayout(vlay);}

        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Raised);
        bot->addWidget(line);}

        {scrl = new QScrollArea(this);
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
        bot->addWidget(scrl);}

        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Raised);
        bot->addWidget(line);}

        {auto vlay = new QVBoxLayout();
            {auto rnam = addBtn(":/assets/UI/rename.svg");
            connect(rnam, &QPushButton::clicked, this, [this](){
                if (isStarCat()) {
                    confirm(this, "Cannot rename the star category!", Conf_OK);
                    return;
                }
                QString cur = getCurrent();
                overlay = new RenameOverlay("Rename category '" + cur + "'", cur, [this](QString s){
                    if (renameCategory(this, s.trimmed())) redoTasks();
                }, this, window());
                overlay->show();
            });
            vlay->addWidget(rnam);}

            {auto plus = addBtn(":/assets/UI/plusFoldr.svg");
            connect(plus, &QPushButton::clicked, this, [this](){
                overlay = new RenameOverlay("New category", "", [this](QString s){
                    newCategory(this, s.trimmed());
                    redoTasks();
                }, this, window());
                overlay->show();
            });
            vlay->addWidget(plus);}
        bot->addLayout(vlay);}
    mtlay->addLayout(bot);

    // Prevent issues while the app is still loading. Shouldn't be visible anyway
    QTimer::singleShot(0, [this](){ redoTasks(); });
}

void TaskView::redoTasks() {
    starbtn->setProperty("current", isStarCat());
    starbtn->style()->unpolish(starbtn);
    starbtn->style()->polish(starbtn);
    starbtn->update();

    auto redo = [this](){ redoTasks(); };
    setTasksLay(tbbllay, [=](std::shared_ptr<Task> t){
        overlay = new TaskOverlay(t, redo, this, window());
        overlay->show();
    }, redo, this);
    setTasksCatsLay(tcatlay, redo, this);
    tcatdrag->installOn(tcatlay);
    tdrag->installOn(tbbllay);
    tcatlay->activate();

    QTimer::singleShot(0, [this](){
        scrl->widget()->setMinimumWidth(tcatlay->lastSize().width() + 8);

        const int w = scrl->viewport()->width();
        const int h = tcatlay->heightForWidth(w);
        scrl->setFixedHeight(h);
    });

    if (overlay) overlay->raise();

    topheader->setText(curCatName());
    newtaskbtn->setVisible(!isStarCat());
}
