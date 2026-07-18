#include "renameOverl.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QGridLayout>
#include <QLabel>
#include <QTextEdit>
#include <QApplication>
#include <QGuiApplication>

constexpr int MARGIN = 12;


RenameOverlay::RenameOverlay(QString initial, std::function<void(QString)> onchange, QWidget* parent) : QWidget(parent) {
    auto* lay = new QGridLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setRowStretch(0, 1);
    lay->setRowStretch(2, 1);
    lay->setColumnStretch(0, 1);
    lay->setColumnStretch(1, 7);
    lay->setColumnStretch(2, 1);

    main = new QLabel("Hello!", this);
    lay->addWidget(main, 1, 1);
}

void RenameOverlay::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Back) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
        } else { deleteLater(); }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
void RenameOverlay::mousePressEvent(QMouseEvent* event) {
    auto point = event->position().toPoint();
    if (!main->geometry().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN).contains(point)) {
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

void RenameOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    auto r = rect();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(r, QColor(125, 125, 125, 125));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRoundedRect(main->geometry().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN), 16, 16);
}
