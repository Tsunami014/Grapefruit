#include "game.hpp"
#include "wids/icobtn.hpp"
#include "wids/secret.hpp"
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
            auto addBtn = [&](const QString& labl, const QString& asset, auto slot) {
                auto btn = new IcoButton(labl, asset);
                btn->setIconSize(QSize(0,0));
                connect(btn, &QPushButton::clicked, this, slot);
                return btn;
            };
            auto at = addBtn("All Tasks", ":/assets/tasks.svg", &MainGame::allTasks);
            auto nt = addBtn("New topic", ":/assets/new.svg", &MainGame::newTopic);
            auto st = addBtn("Settings", ":/assets/settings.svg", [=](){ stack->setCurrentWidget(setts); });
            int w = std::max({
                at->sizeHint().width(), nt->sizeHint().width(), st->sizeHint().width()
            });
            QSize sze(w*0.6, w*0.6);
            auto fixBtn = [&](IcoButton* btn) {
                quickbtns->addWidget(btn);
                btn->setMinimumWidth(w+6);
                btn->setIconSize(sze);
            };
            fixBtn(at); fixBtn(nt); fixBtn(st);
        botsect->addLayout(quickbtns);
    mlay->addLayout(botsect, 1);


    // Create the settings window
    setts = new QWidget();
    stack->addWidget(setts);
    auto slay = new QVBoxLayout(setts);
    slay->setSpacing(16);

    auto topsect = new QHBoxLayout();
        auto bk = new QPushButton();
        bk->setProperty("isbtn", true);
        bk->setIcon(QIcon(":/assets/back.svg"));
        bk->setIconSize(QSize(48, 48));
        bk->
        connect(bk, &QPushButton::clicked, this, [this](){ stack->setCurrentWidget(main); });
        topsect->addWidget(bk);
        {auto labl = new QLabel("Settings");
        labl->setAlignment(Qt::AlignCenter);
        labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        topsect->addWidget(labl);}
    slay->addLayout(topsect);

    {auto opts = new QVBoxLayout();
        {auto labl = new QLabel("Bottom section");
        opts->addWidget(labl);}
    auto sp = new Spoiler("Title");
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    {auto opts = new QVBoxLayout();
        {auto labl = new QLabel("Bottom section2");
        opts->addWidget(labl);}
    auto sp = new Spoiler("Title2");
    sp->setContentLayout(*opts);
    slay->addWidget(sp);}

    slay->addStretch();


    // Initialise things!
    setStyleSheet(
        "QMainWindow { background-color: #DDD; }"
        "IcoButton, *[isbtn=true] {"
            "background-color: #CCAA55;"
            "color: white;"
            "border-radius: 4px;"
            "padding: 4px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 6px;"
            "border-color: #887755;"
        "}"
        "IcoButton:pressed, *[isbtn=true]:pressed {"
            "background-color: #997744;"
            "border-style: inset;"
        "}"
        "Spoiler QToolButton {"
            "background-color: #CCAA55;"
            "color: white;"
            "border-radius: 4px;"
            "padding: 5px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 6px;"
            "border-color: #887755;"
            "qproperty-lineColour: #555;"
            "qproperty-lineWidth: 3px;"
        "}"
        "Spoiler QToolButton:pressed {"
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
