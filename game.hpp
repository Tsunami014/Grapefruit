#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>
#include <QLabel>
#include "wids/flow.hpp"

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

    FlowLayout* olay;
    QLabel* curtxt;

    void keyPressEvent(QKeyEvent* event) override;

    void generateSettings();
    void generateTasks();
    void initstyle();
};
