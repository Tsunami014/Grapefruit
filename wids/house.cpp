#include "house.hpp"
#include "base/scenes.hpp"
#include "extra/bgitem.hpp"
#include <QGraphicsItem>

House::House(QWidget* parent) : scn(), QGraphicsView(parent) {
    setScene(&scn);
    scale(8,8);
    buildScene("dining");
}

void House::buildScene(QString scene) {
    scn.clear();

    auto its = getSceneItems(scene);
    for (const auto& it : its) {
        auto* item = bgItem(it.name);
#ifdef BUILD
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setData(0, it.name);
#endif
        item->setPos(it.pos);
        scn.addItem(item);
    }
}

#ifdef BUILD
#include <QKeyEvent>
void House::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_P) {
        for (auto* item : scn.items()) {
            QString name = item->data(0).toString();
            qDebug() << (name.isEmpty()? "??" : name) << "@" << item->scenePos();
        }
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}
#endif
