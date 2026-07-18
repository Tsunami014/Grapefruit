#include "confirm.hpp"
#include "font.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QEventLoop>
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>

void ConfirmOverlay::mousePressEvent(QMouseEvent* event) {
    if (QApplication::widgetAt(event->pos()) == this) {
        emit pressed();
        event->accept();
    } else { event->ignore(); }
    QWidget::mousePressEvent(event);
}
void ConfirmOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(125, 125, 125, 125));
}

bool confirm(QWidget* parent, const QString& title, const QString& text) {
    auto topLevel = parent ? parent->window() : nullptr;
    if (!topLevel) return false;

    auto ovrl = new ConfirmOverlay(topLevel);
    ovrl->setGeometry(topLevel->rect());

    auto card = new QWidget(ovrl);
    card->setObjectName("card");
    card->setStyleSheet(
        "#card {"
            "background: white;"
            "border-radius: 8px;"
        "}"
        "QPushButton {"
            "background-color: #8B81A2;"
            "color: white;"
            "padding: 16px 8px;"
            "border-style: outset;"
            "border-width: 2px;"
            "border-radius: 12px;"
            "border-color: #7B738A;"
        "}"
        "QPushButton:pressed {"
            "background-color: #756D93;"
            "border-style: inset;"
        "}"
    );

    auto lay = new QVBoxLayout(card);
    auto txt = new QLabel("<b>" + title + "</b><br>" + text);
    resizeFont(txt, 1.3);
    txt->setWordWrap(true);
    lay->addWidget(txt);
    lay->addSpacing(8);

    auto* btns = new QDialogButtonBox(QDialogButtonBox::Yes | QDialogButtonBox::No);
    lay->addWidget(btns);
    for (QPushButton* b : btns->findChildren<QPushButton*>()) {
        resizeFont(b, 1.5);
        b->setIcon(QIcon());
    }

    card->adjustSize();
    card->move((ovrl->width() - card->width()) / 2,
               (ovrl->height() - card->height()) / 2);

    QEventLoop loop;
    bool result = false;

    QObject::connect(btns, &QDialogButtonBox::clicked, ovrl, [&](QAbstractButton* btn) {
        result = btns->buttonRole(btn) == QDialogButtonBox::YesRole;
        loop.quit();
    });
    QObject::connect(ovrl, &ConfirmOverlay::pressed, ovrl, [&]() { loop.quit(); });

    ovrl->show();
    ovrl->raise();
    loop.exec();

    ovrl->deleteLater();
    return result;
}
