#include "renameOverl.hpp"
#include "font.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QGridLayout>
#include <QApplication>
#include <QTimer>

constexpr int MARGIN = 16;


RenameOverlay::RenameOverlay(QString initial, std::function<void(QString)> done, QWidget* parent) : QWidget(parent) {
    auto* lay = new QGridLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setRowStretch(0, 1);
    lay->setRowStretch(2, 1);
    lay->setColumnStretch(0, 1);
    lay->setColumnStretch(1, 7);
    lay->setColumnStretch(2, 1);

    main = new QLineEdit(initial, this);
    resizeFont(main, 1.2);
    connect(main, &QLineEdit::returnPressed, this, [=](){
        done(main->text());
        deleteLater();
    });
    lay->addWidget(main, 1, 1);
    QTimer::singleShot(0, this, [this](){ main->setFocus(); });
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
    if (qobject_cast<QLineEdit*>(wid)) { event->ignore(); }
    else if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
}

void RenameOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    auto r = rect();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(r, QColor(125, 125, 125, 125));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRoundedRect(main->geometry().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN), 14, 14);
}
