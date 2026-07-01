#include "taskOverlay.hpp"
#include "wids/txtedit.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QGuiApplication>

const QMargins innerMarg{64, 96, 64, 48};


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);

    auto* mlay = new QVBoxLayout();
    mlay->setContentsMargins(innerMarg + QMargins(12, 12, 12, 12));
        auto titl = new QLabel(task->name, this);
        mlay->addWidget(titl);
        auto* edit = new TxtEdit(this);
        mlay->addWidget(edit);
    lay->addLayout(mlay);

    auto* bbarr = new QLabel("Hello", this);
    bbarr->setStyleSheet("background-color: white;");
    bbarr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    lay->addWidget(bbarr);
    bbar = bbarr;
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
    auto point = mapToGlobal(event->position().toPoint());
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
    painter.fillRect(r.marginsRemoved(innerMarg + QMargins(0,0,0,bbar->rect().height())), QColor(255, 255, 255));
}
