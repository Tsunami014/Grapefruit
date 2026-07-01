#include <QMainWindow>
#include <QStackedWidget>
#include <QGridLayout>

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
};
