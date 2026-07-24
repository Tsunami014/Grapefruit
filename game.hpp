#include <QMainWindow>
#include <QStackedWidget>
#include "tasks.hpp"
#include "base/converse.hpp"

class MainGame : public QMainWindow {
public:
    MainGame();

    void toMain() { stack->setCurrentWidget(main); }
    void toSetts() { stack->setCurrentWidget(setts); }
    void toTasks();

    Conversation* conv;

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
    TaskView* tasks;

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
};

extern MainGame* MG;
