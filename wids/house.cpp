#include "house.hpp"
#include <QGraphicsRectItem>

House::House(QWidget* parent) : scn(), QGraphicsView(parent) {
    setScene(&scn);
    QGraphicsRectItem* rect = scn.addRect(0, 0, 100, 100);
    rect->setBrush(Qt::blue);
    rect->setFlag(QGraphicsItem::ItemIsMovable);
}
