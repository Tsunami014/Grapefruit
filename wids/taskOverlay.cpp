#include "taskOverlay.hpp"
#include <QPainter>
#include <QMouseEvent>

TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, QWidget* parent) {}

void TaskOverlay::mousePressEvent(QMouseEvent *event) {
    if (childAt(event->position().toPoint()) == nullptr) {
        deleteLater();
        event->accept();
    } else { event->ignore(); }
}
void TaskOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor(125, 125, 125, 125));
}
