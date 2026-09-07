#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
#include "wids/secret.hpp"
#include "wids/confirm.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

void MainGame::generateSettings() {
    auto slay = new QVBoxLayout(setts);
    slay->setSpacing(16);

    auto addbtn = [=](QString txt, QBoxLayout* opts, QWidget* parent){
        auto btn = new QPushButton(txt, parent);
        btn->setProperty("thm", "secondary");
        auto wrap = new QWidget(parent);
        auto wraplay = new QHBoxLayout(wrap);
        wraplay->setContentsMargins(8,8,8,8);
        wraplay->addWidget(btn);
        opts->addWidget(wrap);
        return btn;
    };

    {auto sp = new Spoiler("Style", this);
    auto lay = new QVBoxLayout();
        {auto labl = new QLabel("App theme", sp);
        lay->addWidget(labl);}
        auto opts = new QHBoxLayout();
            {auto btn = addbtn("System", opts, sp);
            btn->connect(btn, &QPushButton::clicked, [=](){
                if (theme != -1) {
                    theme = -1;
                    genStyle();
                }
            });}
            {auto btn = addbtn("Dark", opts, sp);
            btn->connect(btn, &QPushButton::clicked, [=](){
                if (theme != 0) {
                    theme = 0;
                    genStyle();
                }
            });}
            {auto btn = addbtn("Light", opts, sp);
            btn->connect(btn, &QPushButton::clicked, [=](){
                if (theme != 1) {
                    theme = 1;
                    genStyle();
                }
            });}
        lay->addLayout(opts);
    sp->setContentLayout(*lay);
    slay->addWidget(sp);}

    {auto sp = new Spoiler("Reset", this);
    auto opts = new QHBoxLayout();
        {auto btn = addbtn("Delete All Tasks", opts, sp);
        btn->connect(btn, &QPushButton::clicked, [=](){
            if (confirm(setts, "Are you sure you want to delete ALL your tasks?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    delAllTasks();
                    tasks->redoTasks();
                }
        });}

        {auto btn = addbtn("Reset Tasks", opts, sp);
        btn->connect(btn, &QPushButton::clicked, [=](){
            if (confirm(setts, "Are you sure you want to reset ALL your tasks with the defaults?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    resetTasks();
                    tasks->redoTasks();
                }
        });}
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    {auto sp = new Spoiler("Debug", this);
    auto opts = new QVBoxLayout();
        auto labl = new QLabel("Task info...", sp);
        opts->addWidget(labl);

        {auto btn = addbtn("Reload", opts, sp);
        btn->connect(btn, &QPushButton::pressed, labl, [=](){
            labl->setText(getAllTasksDebugInfo());
            sp->updateHeights();
        });}
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    slay->addStretch();

    auto topsect = new QHBoxLayout();
        auto bk = new QPushButton();
        bk->setProperty("thm", "neutral");
        bk->setProperty("btnsty", "round");
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
}
