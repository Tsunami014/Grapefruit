#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLabel>
#include "base/converse.hpp"

class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
    QWidget* tasks;
    QGridLayout* tlay;

    QVBoxLayout* tbbllay;
    QLayout* tcatlay;

    void redoTasks();

    Conversation* conv;

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
    void generateTasks();
    void initstyle();
};
