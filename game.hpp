#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QColor>
#ifdef Q_OS_ANDROID
#include <QTimer>
#endif
#include "tasks.hpp"
#include "colours.hpp"
#include "base/converse.hpp"
#include "wids/house.hpp"

class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();
    void init();

    void toMain() { stack->setCurrentWidget(main); }
    void toSetts() { stack->setCurrentWidget(setts); }
    void toTasks();

    Conversation* conv;
    House* house;

    void genStyle(bool sig = true);
    std::unordered_map<Cols::Colour, QColor> styls;

    /// -1: system, 0: dark, 1: light
    int theme = -1;
    QColor base;

signals:
    void themeChange();

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
    TaskView* tasks;

    std::vector<QColor> cols;

    void setupStyle();

#ifdef Q_OS_ANDROID
    QTimer *stylNavTimer = nullptr;
#endif

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
};

extern MainGame* MG;
