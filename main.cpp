#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QPushButton button("Hello from Qt on Android!");
    button.resize(320, 240);
    button.show();

    return app.exec();
}
