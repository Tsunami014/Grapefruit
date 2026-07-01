#include <QMainWindow>
#include <QStackedWidget>

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

    void generateSettings();
    void generateTasks();
    void initstyle();
};
