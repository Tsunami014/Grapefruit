#include <QApplication>
#include "game.hpp"
#include "base/taskload.hpp"

MainGame* MG = nullptr;

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QFont font = QApplication::font();
    font.setPointSize(16);
    QApplication::setFont(font);

    loadTasks();
    MG = new MainGame();

    MG->setWindowFlags(MG->windowFlags() | Qt::ExpandedClientAreaHint);
    MG->show();
    return app.exec();
}
