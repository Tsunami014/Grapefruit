#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QPointer>
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
    QPointer<QWidget> overlay;

    void redoTasks();

    Conversation* conv;

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
    void generateTasks();
    void initstyle();
};
