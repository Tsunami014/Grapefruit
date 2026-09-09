#include <QApplication>
#include "game.hpp"
#include "base/taskload.hpp"
#include "base/importance.hpp"

MainGame* MG = nullptr;

int main(int argc, char* argv[]) {
    QCoreApplication::setApplicationName("Grapefruit");
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/assets/icon.svg"));

    QFont font = QApplication::font();
    font.setPointSize(16);
    QApplication::setFont(font);

    MG = new MainGame();

    loadTasks();
    loadRatios();
    MG->init();

    MG->setWindowFlags(MG->windowFlags() | Qt::ExpandedClientAreaHint);
    MG->show();
    return app.exec();
}
