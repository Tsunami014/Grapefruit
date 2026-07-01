#include "taskOverlay.hpp"
#include "wids/txtedit.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QGuiApplication>

const QMargins innerMarg{64, 72, 64, 56};


TaskOverlay::TaskOverlay(std::shared_ptr<Task> task, QWidget* parent) : QWidget(parent) {
    auto* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);

    auto* mlay = new QVBoxLayout();
    mlay->setContentsMargins(innerMarg + QMargins(16, 16, 16, 16));
        auto titl = new QLabel(task->name, this);
        mlay->addWidget(titl);
        auto* edit = new TxtEdit(this);
        mlay->addWidget(edit);
    lay->addLayout(mlay);

    bbar = new QWidget(this);
    bbar->setStyleSheet("background-color: white;");
    bbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    lay->addWidget(bbar);

    auto* blay = new QHBoxLayout(bbar);
    blay->setContentsMargins(16,8,16,8);
    auto* labl = new QLabel("Hello!", bbar);
    blay->addWidget(labl);
    blay->addStretch();
}

inline QMargins TaskOverlay::totMargin() {
    return innerMarg + QMargins(0,0,0,bbar->rect().height());
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

    if (!r.marginsRemoved(totMargin()).contains(point)) {
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
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(r, QColor(125, 125, 125, 125));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRoundedRect(r.marginsRemoved(totMargin()), 16, 16);
}
