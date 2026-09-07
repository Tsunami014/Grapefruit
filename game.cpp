#include "game.hpp"
#include "font.hpp"
#include "wids/icobtn.hpp"
#include "wids/flow.hpp"
#include <QBoxLayout>
#include <QPushButton>
#include <QKeyEvent>

MainGame::MainGame() {
    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    // Create the main screen
    main = new QWidget();
    stack->addWidget(main);
    auto mlay = new QVBoxLayout(main);

    house = new House(main);
    house->setStyleSheet("background-color: transparent;");
    mlay->addWidget(house, 2);

    auto* curtxt = new QLabel(main);
    curtxt->setWordWrap(true);
    curtxt->setAlignment(Qt::AlignCenter);
    curtxt->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    curtxt->setObjectName("curtxt");
    ColGroups::setGrp(curtxt, ColGroups::Primary);
    resizeFont(curtxt, 1.2);
    QFontMetrics fm(curtxt->font());
    curtxt->setMinimumHeight(fm.lineSpacing() * 1.2 * 2); // 2 lines plus extra space
#ifdef APP_DEBUG
    {
        auto* btn = new QPushButton("REDO", this);
        ColGroups::setGrp(btn, ColGroups::TertiaryContainer);
        btn->setProperty("btnsty", "big");
        resizeFont(btn, 0.7);
        connect(btn, &QPushButton::clicked, this, [=](){ conv->onclick({}); });

        auto* lay = new QHBoxLayout();
        lay->addWidget(curtxt);
        lay->addWidget(btn);
        mlay->addLayout(lay);
    }
#else
    mlay->addWidget(curtxt);
#endif

    auto botsect = new QHBoxLayout();
        auto optcontnr = new QWidget(main);
        optcontnr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        auto* olay = new FlowLayout(optcontnr, 8, 8, 16);
        botsect->addWidget(optcontnr);

        conv = new Conversation(olay, curtxt);

        auto quickbtns = new QVBoxLayout();
            auto addBtn = [&](const QString& labl, const QString& asset, auto slot) {
                auto btn = new IcoButton(labl, asset);
                ColGroups::setGrp(btn, ColGroups::Tertiary);
                btn->setIconSize(QSize(0,0));
                resizeFont(btn, 0.9);
                connect(btn, &QPushButton::clicked, this, slot);
                return btn;
            };
            auto nt = addBtn("New topic", ":/assets/UI/new.svg", [=](){ conv->newTopic(); });
            auto at = addBtn("All Tasks", ":/assets/UI/tasks.svg", [=](){ toTasks(); });
            auto st = addBtn("Settings", ":/assets/UI/settings.svg", [=](){ toSetts(); });
            int w = std::max({
                nt->sizeHint().width(), at->sizeHint().width(), st->sizeHint().width()
            });
            QSize sze(w*0.6, w*0.6);
            auto fixBtn = [&](IcoButton* btn) {
                quickbtns->addWidget(btn);
                btn->setMinimumWidth(w+6);
                btn->setIconSize(sze);
            };
            fixBtn(nt); fixBtn(at); fixBtn(st);
        botsect->addLayout(quickbtns);
    mlay->addLayout(botsect, 1);


    // Create the settings screen
    setts = new QWidget();
    stack->addWidget(setts);
    generateSettings();

    // Create the tasks screen
    tasks = new TaskView();
    stack->addWidget(tasks);

    // Last initialisation!
    setupStyle();
    stack->setCurrentWidget(main);
}

void MainGame::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Back) {
        stack->setCurrentWidget(main);
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
void MainGame::toTasks() {
#ifdef APP_DEBUG
    tasks->redoTasks();
#endif
    stack->setCurrentWidget(tasks);
}
