#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <memory>
#include "base/tasks.hpp"

class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();
public slots:
    void newTopic();

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
    QWidget* tasks;
    QGridLayout* tlay;

    void generateSettings();
    void generateTasks();
    void initstyle();
    void taskOverlay(std::shared_ptr<Task> task);
};
