#include "game.hpp"
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

    auto labl = new QLabel("Hello from Qt on Android!");
    labl->setStyleSheet("background-color: #3399DD;");
    mlay->addWidget(labl, 2);

    auto botsect = new QHBoxLayout();
        auto labl2 = new QLabel("Option section!");
        labl2->setStyleSheet("background-color: #33DD99;");
        labl2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        botsect->addWidget(labl2);

        auto quickbtns = new QVBoxLayout();
            auto at = new QPushButton("All tasks");
            connect(at, &QPushButton::clicked, this, &MainGame::allTasks);
            quickbtns->addWidget(at);
            auto nt = new QPushButton("New topic");
            connect(nt, &QPushButton::clicked, this, &MainGame::newTopic);
            quickbtns->addWidget(nt);
            auto st = new QPushButton("Settings");
            connect(st, &QPushButton::clicked, this, [=](){ stack->setCurrentWidget(setts); });
            quickbtns->addWidget(st);
        botsect->addLayout(quickbtns);
    mlay->addLayout(botsect, 1);


    // Create the settings window
    setts = new QWidget();
    stack->addWidget(setts);
    auto slay = new QVBoxLayout(setts);

    auto bk = new QPushButton("Back");
    connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
    slay->addWidget(bk);


    // Initialise things!
    setStyleSheet(
        "QMainWindow { background-color: #333333; }"
        "QPushButton {"
            "background-color: #CCAA55;"
            "color: white;"
            "border-radius: 4px;"
            "padding: 6px 12px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 6px;"
            "border-color: #887755;"
        "}"
        "QPushButton:pressed {"
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
