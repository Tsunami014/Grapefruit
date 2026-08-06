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

    auto topsect = new QHBoxLayout();
        auto bk = new QPushButton();
        bk->setProperty("fancy", true);
        bk->setProperty("backbtn", true);
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

    auto addbtn = [=](QString txt, QBoxLayout* opts, QWidget* parent){
        auto btn = new QPushButton(txt, parent);
        btn->setProperty("fancy", true);
        btn->setProperty("settbtn", true);
        auto wrap = new QWidget(parent);
        auto wraplay = new QHBoxLayout(wrap);
        wraplay->setContentsMargins(8,8,8,8);
        wraplay->addWidget(btn);
        opts->addWidget(wrap);
        return btn;
    };

    {auto sp = new Spoiler("Reset", this);
    auto opts = new QHBoxLayout();
        {auto btn = addbtn("Delete All Tasks", opts, sp);
        btn->connect(btn, &QPushButton::pressed, [=](){
            btn->clearFocus();
            if (confirm(setts, "Are you sure you want to delete all your tasks?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    delAllTasks();
                }
        });}

        {auto btn = addbtn("Reset Tasks", opts, sp);
        btn->connect(btn, &QPushButton::pressed, [=](){
            btn->clearFocus();
            if (confirm(setts, "Are you sure you want to reset all your tasks with the defaults?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    resetTasks();
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
}
