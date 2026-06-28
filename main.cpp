#include <QApplication>
#include <QMainWindow>
#include <QPushButton>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;

    QPushButton* button = new QPushButton("Hello from Qt on Android!");

    window.setCentralWidget(button);
    window.setWindowFlags(window.windowFlags() | Qt::ExpandedClientAreaHint);
    window.show();
    return app.exec();
}
