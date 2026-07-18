#include "renameOverl.hpp"
#include "font.hpp"
#include <QGridLayout>
#include <QBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QTimer>

constexpr int MARGIN = 8;


RenameOverlay::RenameOverlay(QString initial, std::function<void(QString)> done, QWidget* parent) : QWidget(parent) {
    auto* lay = new QGridLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setRowStretch(0, 1);
    lay->setRowStretch(2, 1);
    lay->setColumnStretch(0, 1);
    lay->setColumnStretch(1, 7);
    lay->setColumnStretch(2, 1);

    main = new QWidget(this);
    auto mlay = new QHBoxLayout(main);
    lay->addWidget(main, 1, 1);

    auto le = new QLineEdit(initial, this);
    resizeFont(le, 1.4);
    connect(le, &QLineEdit::returnPressed, this, [=](){
        done(le->text());
        deleteLater();
    });
    QTimer::singleShot(0, this, [le](){ le->setFocus(); });
    mlay->addWidget(le);

    auto btn = new QPushButton();
    btn->setProperty("fancy", true);
    btn->setIcon(QIcon(":/assets/UI/check.svg"));
    int mx = le->rect().height() + 8;
    btn->setIconSize(QSize(mx, mx-4));
    btn->setProperty("backbtn", true);
    connect(btn, &QPushButton::clicked, le, &QLineEdit::returnPressed);
    mlay->addWidget(btn);
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
