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
        const auto items = scn.items();
        for (auto it = items.crbegin(); it != items.crend(); ++it) {
            auto* item = *it;
            QString name = item->data(0).toString();
            auto pos = item->scenePos();
            qDebug() << QString("%1 @ (%2, %3)")
               .arg(name.isEmpty()? "??" : name)
               .arg(QString::number(pos.x(), 'f', 2))
               .arg(QString::number(pos.y(), 'f', 2));
        }
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}
#endif
