#include "game.hpp"
#include "wids/icobtn.hpp"
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>

MainGame::MainGame() {
    stack = new QStackedWidget(this);
    setCentralWidget(stack);

    // Create the main window
    main = new QWidget();
    stack->addWidget(main);
    auto mlay = new QVBoxLayout(main);

    {auto labl = new QLabel("Hello from Qt on Android!");
    labl->setStyleSheet("background-color: #3399DD;");
    mlay->addWidget(labl, 2);}

    auto botsect = new QHBoxLayout();
        {auto labl = new QLabel("Option section!");
        labl->setStyleSheet("background-color: #33DD99;");
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        botsect->addWidget(labl);}

        auto quickbtns = new QVBoxLayout();
            auto at = new IcoButton("All Tasks", ":/assets/tasks.svg");
            connect(at, &QPushButton::clicked, this, &MainGame::allTasks);
            quickbtns->addWidget(at);
            auto nt = new IcoButton("New topic", ":/assets/new.svg");
            connect(nt, &QPushButton::clicked, this, &MainGame::newTopic);
            quickbtns->addWidget(nt);
            auto st = new IcoButton("Settings", ":/assets/settings.svg");
            connect(st, &QPushButton::clicked, this, [=](){ stack->setCurrentWidget(setts); });
            quickbtns->addWidget(st);
            int w = std::max({
                at->sizeHint().width(),
                nt->sizeHint().width(),
                st->sizeHint().width()
            });
            QSize sze(w-16, w-16);
            at->setMinimumWidth(w);
            at->setIconSize(sze);
            nt->setMinimumWidth(w);
            nt->setIconSize(sze);
            st->setMinimumWidth(w);
            st->setIconSize(sze);
        botsect->addLayout(quickbtns);
    mlay->addLayout(botsect, 1);


    // Create the settings window
    setts = new QWidget();
    stack->addWidget(setts);
    auto slay = new QVBoxLayout(setts);

    auto topsect = new QHBoxLayout();
        auto bk = new QPushButton("Back");
        connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
        topsect->addWidget(bk);
        {auto labl = new QLabel("Settings");
        labl->setAlignment(Qt::AlignCenter);
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        topsect->addWidget(labl);}
    slay->addLayout(topsect);

    {auto labl = new QLabel("Bottom section");
    labl->setStyleSheet("background-color: #AA55AA;");
    labl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    slay->addWidget(labl);}


    // Initialise things!
    setStyleSheet(
        "QMainWindow { background-color: #333333; }"
        "QToolButton {"
            "background-color: #CCAA55;"
            "color: white;"
            "border-radius: 4px;"
            "padding: 6px 12px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 6px;"
            "border-color: #887755;"
        "}"
        "QToolButton:pressed {"
            "background-color: #997744;"
            "border-style: inset;"
        "}"
    );
    stack->setCurrentWidget(main);
    setWindowFlags(windowFlags() | Qt::ExpandedClientAreaHint);
    show();
}

void MainGame::newTopic() {
}

void MainGame::allTasks() {
}
