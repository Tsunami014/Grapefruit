#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#ifdef Q_OS_ANDROID
#include <QTimer>
#endif
#include "tasks.hpp"
#include "base/converse.hpp"
#include "wids/house.hpp"

class MainGame : public QMainWindow {
public:
    MainGame();

    void toMain() { stack->setCurrentWidget(main); }
    void toSetts() { stack->setCurrentWidget(setts); }
    void toTasks();

    Conversation* conv;
    House* house;

    void genStyle();

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
    TaskView* tasks;

    void setupStyle();
    /// -1: system, 0: dark, 1: light
    int theme = -1;

#ifdef Q_OS_ANDROID
    QTimer *stylNavTimer = nullptr;
#endif

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
};

extern MainGame* MG;
