#include "taskbbl.hpp"
#include <QLabel>
#include <QStyleOption>
#include <QPainter>

TaskBubble::TaskBubble(const QString& top, const QString& bot, QWidget* parent) : QWidget(parent) {
    main = new QVBoxLayout(this);
    main->setSpacing(0);

    {auto labl = new QLabel(top, this);
    labl->setAlignment(Qt::AlignCenter);
    labl->setProperty("bubble", "top");
    main->addWidget(labl);}

    {auto labl = new QLabel(bot, this);
    labl->setAlignment(Qt::AlignCenter);
    labl->setProperty("bubble", "bot");
    main->addWidget(labl);}
}

void TaskBubble::mousePressEvent(QMouseEvent *event) {
    event->accept();
    pressed = true;
    refreshStyle();
    QWidget::mousePressEvent(event);
}
void TaskBubble::mouseReleaseEvent(QMouseEvent *event) {
    if (!pressed) return;
    event->accept();
    pressed = false;
    refreshStyle();
    QWidget::mouseReleaseEvent(event);
    emit clicked();
}
void TaskBubble::mouseMoveEvent(QMouseEvent *event) {
    if (pressed && !rect().contains(event->pos())) {
        event->accept();
        pressed = false;
        refreshStyle();
    }
    QWidget::mouseMoveEvent(event);
}
void TaskBubble::refreshStyle() {
    setProperty("pressed", pressed);
    style()->unpolish(this);
    style()->polish(this);
    for (QLabel* labl : findChildren<QLabel*>()) {
        style()->unpolish(labl);
        style()->polish(labl);
        labl->update();
    }
    update();
}
