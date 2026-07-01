#include "taskOverlay.hpp"
#include "wids/txtedit.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QGuiApplication>


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, QWidget* parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(innerMarg() + QMargins(12, 12, 12, 12));

    auto titl = new QLabel(task->name, this);
    lay->addWidget(titl);

    auto* edit = new TxtEdit(this);
    lay->addWidget(edit);
}

const QMargins TaskOverlay::innerMarg() {
    return {64, 96, 64, 48};
}

void TaskOverlay::mousePressEvent(QMouseEvent* event) {
    if (!rect().marginsRemoved(innerMarg()).contains(event->position().toPoint())) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
        } else {
            deleteLater();
        }
        event->accept();
    } else { event->ignore(); }
}

void TaskOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    auto r = rect();
    painter.fillRect(r, QColor(125, 125, 125, 125));
    painter.fillRect(r.marginsRemoved(innerMarg()), QColor(255, 255, 255));
}
