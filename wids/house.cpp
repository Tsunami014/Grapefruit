#include "house.hpp"
#include "base/scenes.hpp"
#include "extra/bgitem.hpp"
#include <QGraphicsItem>

#define BUILD

House::House(QWidget* parent) : scn(), QGraphicsView(parent) {
    setScene(&scn);
    scale(8,8);
    buildScene("dining");
}

void House::buildScene(QString scene) {
    scn.clear();

    auto its = getSceneItems(scene);
    for (const auto& it : its) {
        auto* item = bgItem(it);
#ifdef BUILD
        item->setFlag(QGraphicsItem::ItemIsMovable);
#endif
        scn.addItem(item);
    }
}
