#include "house.hpp"
#include "base/scenes.hpp"
#include "extra/bgitem.hpp"
#include <QGraphicsItem>

House::House(QWidget* parent) : scn(), QGraphicsView(parent) {
    setScene(&scn);
    scale(8,8);
    //buildScene("dining#eat#pizza");
    newScene();
}

void House::newScene() {
    buildScene(randomScene());
}

void House::buildScene(QString scene) {
    scn.clear();

    auto its = getSceneItems(scene);
    for (const auto& it : its) {
        auto* item = bgItem(it.name);
#ifdef DEBUG
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setData(0, it.name);
        item->setData(1, it.flip);
#endif
        if (it.flip) {
            item->setTransformOriginPoint(item->boundingRect().center());
            QTransform trans;
            trans.scale(-1, 1);
            item->setTransform(trans);
        }
        item->setPos(it.pos);
        scn.addItem(item);
    }
}

#ifdef DEBUG
#include <QKeyEvent>
void House::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_P) {
        const auto items = scn.items();
        for (auto it = items.crbegin(); it != items.crend(); ++it) {
            auto* item = *it;
            QString name = item->data(0).toString();
            bool flip = item->data(1).toBool();
            auto pos = item->scenePos();
            qDebug().noquote() << QString("[%1, %2, %3]")
               .arg((flip? "~":"") + (name.isEmpty()? "??" : name.split('/')[1]))
               .arg(QString::number(pos.x(), 'f', 2))
               .arg(QString::number(pos.y(), 'f', 2));
        }
        event->accept();
        return;
    }
    QGraphicsView::keyPressEvent(event);
}
#endif
