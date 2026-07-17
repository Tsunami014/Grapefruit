#include "house.hpp"
#include "extra/bgitem.hpp"
#include <QGraphicsItem>

House::House(QWidget* parent) : scn(), QGraphicsView(parent) {
    setScene(&scn);

    auto it = bgItem("furnature/table");
    it->setFlag(QGraphicsItem::ItemIsMovable);
    scn.addItem(it);
    fitInView(it, Qt::KeepAspectRatio);
}
