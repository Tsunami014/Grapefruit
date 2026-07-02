#include "taskOverlay.hpp"
#include "base/itemopts.hpp"
#include "wids/txtedit.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QGuiApplication>
#include <QPushButton>

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
    bbar->setObjectName("bottom");
    bbar->setStyleSheet("QWidget#bottom { background-color: white; }");
    lay->addWidget(bbar);

    auto* blay = new QHBoxLayout(bbar);
    blay->setContentsMargins(12,6,12,6);
    blay->setSpacing(12);

    int hei = 40+8+12; // Base hei + padding + margin
    bbar->setMaximumHeight(hei); bbar->setMinimumHeight(hei);
    GenerateOpts(bbar, blay, edit, false);
    bbar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    connect(edit, &TxtEdit::focusChange, bbar, [=](bool focus){ GenerateOpts(bbar, blay, edit, focus); });
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
