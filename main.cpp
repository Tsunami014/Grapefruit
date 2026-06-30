#include <QApplication>
#include "game.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    auto game = new MainGame();
    return app.exec();
}
