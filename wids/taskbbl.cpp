#include "taskbbl.hpp"
#include <QLabel>
#include <QStyleOption>
#include <QPainter>
#include <QTimer>

TaskBubble::TaskBubble(std::shared_ptr<Task> t, QWidget* parent) : QWidget(parent) {
    main = new QVBoxLayout(this);
    main->setSpacing(0);

    {auto labl = new QLabel(t->name, this);
    labl->setAlignment(Qt::AlignCenter);
    labl->setProperty("bubble", "top");
    main->addWidget(labl);}

    {auto labl = new QLabel(t->bottom(), this);
    labl->setAlignment(Qt::AlignCenter);
    labl->setProperty("bubble", "bot");
    main->addWidget(labl);}
}

void TaskBubble::mousePressEvent(QMouseEvent* event) {
    event->accept();
    pressed = true;
    QTimer::singleShot(0, this, &TaskBubble::refreshStyle);
    QWidget::mousePressEvent(event);
}
void TaskBubble::mouseReleaseEvent(QMouseEvent* event) {
    if (!pressed) return;
    event->accept();
    pressed = false;
    QTimer::singleShot(0, this, &TaskBubble::refreshStyle);
    QWidget::mouseReleaseEvent(event);
    emit clicked();
}
void TaskBubble::mouseMoveEvent(QMouseEvent* event) {
    if (pressed && !rect().contains(event->pos())) {
        event->accept();
        pressed = false;
        QTimer::singleShot(0, this, &TaskBubble::refreshStyle);
    }
    QWidget::mouseMoveEvent(event);
}
void TaskBubble::refreshStyle() {
    setProperty("pressed", pressed);
    style()->polish(this);
    for (QLabel* labl : findChildren<QLabel*>()) {
        style()->polish(labl);
        labl->update();
    }
    update();
}
