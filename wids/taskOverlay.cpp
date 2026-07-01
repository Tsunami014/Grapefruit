#include "taskOverlay.hpp"
#include "wids/txtedit.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QGuiApplication>

const QMargins innerMarg{64, 96, 64, 64};


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, QWidget* parent) : QWidget(parent) {
    main = new QWidget(this);
    main->setGeometry(rect());
    auto* mlay = new QVBoxLayout(main);
    mlay->setContentsMargins(innerMarg + QMargins(12, 12, 12, 12));

    auto titl = new QLabel(task->name, this);
    mlay->addWidget(titl);

    auto* edit = new TxtEdit(this);
    mlay->addWidget(edit);

    bottom = new QWidget(this);
    bottom->setGeometry(rect());
    auto* blay = new QVBoxLayout(bottom);
    blay->setContentsMargins(QMargins(0,0,0,0));
    blay->addStretch();

    auto* bbarr = new QLabel("Hello", this);
    bbarr->setStyleSheet("background-color: white;");
    bbarr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    blay->addWidget(bbarr);
    bbar = bbarr;
}
void TaskOverlay::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    main->setGeometry(rect());
    bottom->setGeometry(rect());
}

void TaskOverlay::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Back) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
        } else { deleteLater(); }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
void TaskOverlay::mousePressEvent(QMouseEvent* event) {
    auto point = event->position().toPoint();
    auto r = rect();
    if (point.y() > r.bottom() - bbar->rect().height()) { event->ignore(); return; }

    if (!r.marginsRemoved(innerMarg).contains(point)) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
            if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
        } else {
            deleteLater();
        }
        event->accept();
        return;
    }
    QWidget* wid = QApplication::widgetAt(point);
    if (qobject_cast<QTextEdit*>(wid)) { event->ignore(); }
    else if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
}

void TaskOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    auto r = rect();
    painter.fillRect(r, QColor(125, 125, 125, 125));
    painter.fillRect(r.marginsRemoved(innerMarg), QColor(255, 255, 255));
}
