#include <QMainWindow>
#include <QStackedWidget>
#include "tasks.hpp"
#include "base/converse.hpp"

class MainGame : public QMainWindow {
public:
    MainGame();

    void toMain() { stack->setCurrentWidget(main); }
    void toSetts() { stack->setCurrentWidget(setts); }
    void toTasks() { stack->setCurrentWidget(tasks); }

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
    TaskView* tasks;

    Conversation* conv;

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
};
