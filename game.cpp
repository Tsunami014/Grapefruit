#include "game.hpp"
#include "base/options.hpp"
#include "wids/icobtn.hpp"
#include "wids/flow.hpp"
#include <QLabel>
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

    {auto labl = new QLabel("Hello from Qt on Android!", main);
    labl->setStyleSheet("background-color: #4488CC;");
    mlay->addWidget(labl, 2);}

    auto botsect = new QHBoxLayout();
        auto optcontnr = new QWidget(main);
        optcontnr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            auto olay = new FlowLayout(optcontnr, 8, 16, 24);
            setOpts(olay, { {"Hi"}, {"Bye"} }, optcontnr);
        botsect->addWidget(optcontnr);

        auto quickbtns = new QVBoxLayout();
            auto addBtn = [&](const QString& labl, const QString& asset, auto slot) {
                auto btn = new IcoButton(labl, asset);
                btn->setIconSize(QSize(0,0));
                connect(btn, &QPushButton::clicked, this, slot);
                return btn;
            };
            auto nt = addBtn("New topic", ":/assets/new.svg", &MainGame::newTopic);
            auto at = addBtn("All Tasks", ":/assets/tasks.svg", [=](){ stack->setCurrentWidget(tasks); });
            auto st = addBtn("Settings", ":/assets/settings.svg", [=](){ stack->setCurrentWidget(setts); });
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
    tasks = new QWidget();
    stack->addWidget(tasks);
    generateTasks();

    // Last initialisation!
    initstyle();
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

void MainGame::newTopic() {
}
