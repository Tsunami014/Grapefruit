#include <QApplication>
#include "game.hpp"
#include "base/taskload.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QFont font = QApplication::font();
    font.setPointSize(16);
    QApplication::setFont(font);

    loadTasks();
    auto game = new MainGame();

    game->setWindowFlags(game->windowFlags() | Qt::ExpandedClientAreaHint);
    game->show();
    return app.exec();
}
