#include <QMainWindow>
#include <QStackedWidget>

class MainGame : public QMainWindow {
    Q_OBJECT
public:
    MainGame();
public slots:
    void newTopic();
    void allTasks();

private:
    QStackedWidget* stack;
    QWidget* main;
    QWidget* setts;
};
