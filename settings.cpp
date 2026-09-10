#include "game.hpp"
#include "font.hpp"
#include "base/taskload.hpp"
#include "extra/icon.hpp"
#include "wids/secret.hpp"
#include "wids/confirm.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

constexpr int colbtnsize = 50;

void MainGame::generateSettings() {
    auto slay = new QVBoxLayout(setts);
    slay->setSpacing(16);

    QVBoxLayout* mslay;
    DragScroll* sdrag;
    {auto mainscrl = new QScrollArea(this);
    mainscrl->setFrameShape(QFrame::NoFrame);
    mainscrl->setProperty("bg", true);

    mainscrl->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mainscrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    mainscrl->verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    mainscrl->verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
    sdrag = new DragScroll(mainscrl->viewport(), mainscrl->verticalScrollBar());

    auto* tcont = new QWidget(this);
    tcont->setObjectName("transpbg");
    mslay = new QVBoxLayout(tcont);
    mslay->setSpacing(16);
    mslay->setAlignment(Qt::AlignTop);
    mainscrl->setWidget(tcont);
    mainscrl->setWidgetResizable(true);
    slay->addWidget(mainscrl);}


    auto addbtn = [=](const QString& txt, QWidget* parent, bool bad = false){
        auto btn = new QPushButton(txt, parent);
        ColGroups::setGrp(btn, bad? ColGroups::ErrorContainer : ColGroups::PrimaryContainer);
        btn->setProperty("btnsty", "big");
        return btn;
    };
    auto fmtLay = [](QLayout* lay) {
        lay->setSpacing(12);
        lay->setContentsMargins(12,12,12,12);
    };

    {auto sp = new Spoiler("App Style", this);
    auto lay = new QVBoxLayout();
    fmtLay(lay);
        auto opts = new QHBoxLayout();
            auto mkThemeBtn = [=](QString thmtxt, const int thmval) {
                auto btn = addbtn(thmtxt, sp);
                btn->connect(btn, &QPushButton::clicked, this, [=](){
                    if (theme != thmval) {
                        theme = thmval;
                        genStyle();
                        saveTasks();
                    }
                });
                auto colour = [=]() { ColGroups::setGrp(btn, theme == thmval? ColGroups::Primary : ColGroups::PrimaryContainer); };
                connect(this, &MainGame::themeChange, btn, [=](){
                    colour();
                    btn->style()->unpolish(btn);
                    btn->style()->polish(btn);
                });
                colour();
                opts->addWidget(btn);
            };
            mkThemeBtn("System", -1);
            mkThemeBtn("Dark", 0);
            mkThemeBtn("Light", 1);
        lay->addLayout(opts);

        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        lay->addWidget(line);}

        auto opts2 = new QHBoxLayout();
            {auto* btn = new QPushButton(sp);
            btn->setFixedSize(colbtnsize, colbtnsize);
            const int icosze = colbtnsize*0.7;
            btn->setIconSize(QSize(icosze, icosze));
            ColGroups::setGrp(btn, ColGroups::PrimaryContainer);

            auto restyl = [=](){
                btn->setStyleSheet(QString(
                    "background-color: %1;"
                    "border: 3px solid %2;"
                    "border-radius: %3px;"
                )
                    .arg(base.name())
                    .arg(base.darker(150).name())
                    .arg(colbtnsize / 2)
                );
                double lum = (0.299 * base.red() + 0.587 * base.green() + 0.114 * base.blue());
                bool light = lum > 128.0;
                setPlainColIco(btn, ":/assets/UI/paint.svg",
                    QColor::fromHslF(std::fmod(base.hueF()+0.12f, 1.0f), 0.48, light? 0.2:0.9));
            };
            restyl();
            connect(this, &MainGame::themeChange, btn, [=](){
                    restyl();
                btn->style()->unpolish(btn);
                btn->style()->polish(btn);
            });
            opts2->addWidget(btn);}
        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::VLine);
        opts2->addWidget(line);}

        {auto scrl = new QScrollArea(this);
        scrl->setFrameShape(QFrame::NoFrame);
        scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        scrl->setObjectName("highcard");

        scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrl->horizontalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        scrl->horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);
        auto drag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());

        auto* cont = new QWidget(this);
        cont->setObjectName("transpbg");
        auto opts2inr = new QHBoxLayout(cont);
            for (const QColor& c : cols) {
                auto* btn = new QPushButton(sp);
                btn->setFixedSize(colbtnsize, colbtnsize);

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
                    .arg(colbtnsize / 2)
                );
                opts2inr->addWidget(btn);
            }
        drag->installOn(opts2inr);
        scrl->setWidget(cont);
        scrl->setWidgetResizable(true);
        opts2->addWidget(scrl);}
        lay->addLayout(opts2);

        {QFrame* line = new QFrame();
        line->setFrameShape(QFrame::HLine);
        lay->addWidget(line);}

        {auto scrl = new QScrollArea(this);
        scrl->setFrameShape(QFrame::NoFrame);
        scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        scrl->setObjectName("highcard");

        scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrl->horizontalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        scrl->horizontalScrollBar()->setFocusPolicy(Qt::NoFocus);
        auto drag = new DragScroll(scrl->viewport(), scrl->horizontalScrollBar());

        auto* cont = new QWidget(this);
        cont->setObjectName("transpbg");
        auto thmopts = new FlowLayout(cont);
        thmopts->vertical(2);
        for (const auto& th : Themes::names) {
            auto btn = addbtn(th.first.toString(), sp);
            btn->connect(btn, &QPushButton::clicked, this, [=](){
                if (colthm != th.second) {
                    colthm = th.second;
                    genStyle();
                    saveTasks();
                }
            });
            auto colour = [=]() { ColGroups::setGrp(btn, colthm == th.second? ColGroups::Primary : ColGroups::PrimaryContainer); };
            connect(this, &MainGame::themeChange, btn, [=](){
                colour();
                btn->style()->unpolish(btn);
                btn->style()->polish(btn);
            });
            colour();
            thmopts->addWidget(btn);
        }
        drag->installOn(thmopts);
        scrl->setWidget(cont);
        scrl->setWidgetResizable(true);
        lay->addWidget(scrl);}

        {auto sp2 = new Spoiler("Colour Test", this);
        sp2->contentArea.setObjectName("highcard");
        auto opts = new FlowLayout(8, 8, 8);
        fmtLay(opts);
            for (const auto& [g, inf] : ColGroups::Groups) {
                auto* btn = new QPushButton(sp2);
                btn->setFixedSize(colbtnsize, colbtnsize);

                auto inf2 = inf; // So it doesn't complain
                auto styl = [=](){
                    btn->setStyleSheet(QString(
                        "background-color: %1;"
                        "border: %3px solid %2;"
                        "border-radius: %4px;"
                    )
                        .arg(styls[inf2.fg].name())
                        .arg(styls[inf2.bg].name())
                        .arg(colbtnsize * 0.35)
                        .arg(colbtnsize * 0.25)
                    );
                };
                connect(this, &MainGame::themeChange, btn, [=](){
                    styl();
                    btn->style()->unpolish(btn);
                    btn->style()->polish(btn);
                });
                styl();
                opts->addWidget(btn);
            }
        sp2->setContentLayout(*opts);
        connect(sp2, &Spoiler::heightChanged, sp, [sp](){ sp->updateHeights(); });
        lay->addWidget(sp2);}
    sp->setContentLayout(*lay);
    mslay->addWidget(sp);}

    {auto sp = new Spoiler("Reset", this);
    auto opts = new QHBoxLayout();
    fmtLay(opts);
        {auto btn = addbtn("Delete All Tasks", sp, true);
        btn->connect(btn, &QPushButton::clicked, [=](){
            if (confirm(setts, "Are you sure you want to delete ALL your tasks?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    delAllTasks();
                    tasks->redoTasks();
                }
        });
        opts->addWidget(btn);}

        {auto btn = addbtn("Reset Tasks", sp, true);
        btn->connect(btn, &QPushButton::clicked, [=](){
            if (confirm(setts, "Are you sure you want to reset ALL your tasks with the defaults?", Conf_YESNO)
                == QDialogButtonBox::YesRole) {
                    resetTasks();
                    tasks->redoTasks();
                }
        });
        opts->addWidget(btn);}
    sp->setContentLayout(*opts);
    mslay->addWidget(sp);}

    {auto sp = new Spoiler("Debug", this);
    auto opts = new QVBoxLayout();
    fmtLay(opts);
        auto labl = new QLabel("Task info will go here...", sp);
        opts->addWidget(labl);

        {auto btn = addbtn("Reload task info", sp);
        btn->connect(btn, &QPushButton::clicked, labl, [=](){
            labl->setText(getAllTasksDebugInfo());
            sp->updateHeights();
        });
        opts->addWidget(btn);}
    sp->setContentLayout(*opts);
    mslay->addWidget(sp);}

    sdrag->installOn(mslay);

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
