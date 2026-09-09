#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
#include "extra/icon.hpp"
#include "wids/secret.hpp"
#include "wids/confirm.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

void MainGame::generateSettings() {
    auto slay = new QVBoxLayout(setts);
    slay->setSpacing(16);

    auto addbtn = [=](QString txt, QBoxLayout* opts, QWidget* parent, bool bad = false){
        auto btn = new QPushButton(txt, parent);
        ColGroups::setGrp(btn, bad? ColGroups::ErrorContainer : ColGroups::PrimaryContainer);
        btn->setProperty("btnsty", "big");
        opts->addWidget(btn);
        return btn;
    };

    auto fmtLay = [](QBoxLayout* lay) {
        lay->setSpacing(12);
        lay->setContentsMargins(12,12,12,12);
    };

    {auto sp = new Spoiler("Style", this);
    auto lay = new QVBoxLayout();
    fmtLay(lay);
        {auto labl = new QLabel("App theme", sp);
        lay->addWidget(labl);}
        auto opts = new QHBoxLayout();
            auto mkThemeBtn = [=](QString thmtxt, const int thmval) {
                auto btn = addbtn(thmtxt, opts, sp);
                btn->connect(btn, &QPushButton::clicked, this, [=](){
                    if (theme != thmval) {
                        theme = thmval;
                        genStyle();
                        saveTasks();
                    }
                });
                connect(this, &MainGame::themeChange, btn, [=](){
                    ColGroups::setGrp(btn, theme == thmval? ColGroups::Primary : ColGroups::PrimaryContainer);
                    btn->style()->unpolish(btn);
                    btn->style()->polish(btn);
                });
                if (theme == thmval) {
                    ColGroups::setGrp(btn, ColGroups::Primary);
                }
            };
            mkThemeBtn("System", -1);
            mkThemeBtn("Dark", 0);
            mkThemeBtn("Light", 1);
        lay->addLayout(opts);

        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        lay->addWidget(line);}

        {auto scrl = new QScrollArea(this);
        scrl->setFrameShape(QFrame::NoFrame);
        scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        scrl->setProperty("bg", true);

        scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrl->horizontalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        scrl->horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);
        auto tcatdrag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());

        auto* catcont = new QWidget(this);
        catcont->setObjectName("transpbg");
        auto opts2 = new QHBoxLayout(catcont);
            const int size = 50;
            for (const QColor& c : cols) {
                auto* btn = new QPushButton(sp);
                btn->setFixedSize(size, size);

                btn->connect(btn, &QPushButton::clicked, this, [=](){
                    base = c;
                    genStyle();
                    saveTasks();
                });

                btn->setStyleSheet(QString(
                    "background-color: %1;"
                    "border: 3px solid %2;"
                    "border-radius: %3px;"
                )
                    .arg(c.name())
                    .arg(c.darker(150).name())
                    .arg(size / 2)
                );
                opts2->addWidget(btn);
            }
        tcatdrag->installOn(opts2);
        scrl->setWidget(catcont);
        scrl->setWidgetResizable(true);
        lay->addWidget(scrl);}
    sp->setContentLayout(*lay);
    slay->addWidget(sp);}

    {auto sp = new Spoiler("Reset", this);
    auto opts = new QHBoxLayout();
    fmtLay(opts);
        {auto btn = addbtn("Delete All Tasks", opts, sp, true);
        btn->connect(btn, &QPushButton::clicked, [=](){
            if (confirm(setts, "Are you sure you want to delete ALL your tasks?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    delAllTasks();
                    tasks->redoTasks();
                }
        });}

        {auto btn = addbtn("Reset Tasks", opts, sp, true);
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
    fmtLay(opts);
        auto labl = new QLabel("Task info...", sp);
        opts->addWidget(labl);

        {auto btn = addbtn("Reload", opts, sp);
        btn->connect(btn, &QPushButton::clicked, labl, [=](){
            labl->setText(getAllTasksDebugInfo());
            sp->updateHeights();
        });}
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    slay->addStretch();

    auto topsect = new QHBoxLayout();
        auto bk = new QPushButton();
        ColGroups::setGrp(bk, ColGroups::SecondaryContainer);
        bk->setProperty("btnsty", "round outline");
        setColIco(bk, ":/assets/UI/back.svg");
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
