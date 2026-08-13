#include "confirm.hpp"
#include "font.hpp"
#include "extra/drag.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QEventLoop>
#include <QBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QScrollArea>
#include <QPushButton>

ConfirmOverlay::ConfirmOverlay(QWidget* parent, bool scroll) : QWidget(parent) {
    auto* outerV = new QVBoxLayout(this);
    outerV->setContentsMargins(0,0,0,0);
    auto* outerH = new QHBoxLayout();
    outerH->setContentsMargins(0,0,0,0);
    outerV->addStretch(scroll? 1:0);
    outerV->addLayout(outerH, scroll? 7:0);
    outerV->addStretch(scroll? 1:0);

    inner = new QWidget(this);
    outerH->addStretch(1);
    outerH->addWidget(inner, 7);
    outerH->addStretch(1);

    inner->setObjectName("card");
    setGeometry(parent->rect());
    parent->installEventFilter(this);
}
bool ConfirmOverlay::eventFilter(QObject* watched, QEvent* event) {
    if (watched == parent() && event->type() == QEvent::Resize) {
        QResizeEvent* resizeEvent = static_cast<QResizeEvent*>(event);
        setGeometry({QPoint(0, 0), resizeEvent->size()});
    }
    return QWidget::eventFilter(watched, event);
}

void ConfirmOverlay::mousePressEvent(QMouseEvent* event) {
    if (QApplication::widgetAt(mapToGlobal(event->pos())) == this) {
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

QDialogButtonBox::ButtonRole confirm(QWidget* parent, const QString& text, ConfirmOpts opts, bool scroll) {
    auto topLevel = parent ? parent->window() : nullptr;
    if (!topLevel) return QDialogButtonBox::RejectRole;

    auto ovrl = new ConfirmOverlay(topLevel, scroll);

    auto lay = new QVBoxLayout(ovrl->inner);
    auto txt = new QLabel(text, ovrl);
    txt->setObjectName("cardtxt");
    resizeFont(txt, scroll? 1.1 : 1.3);
    txt->setWordWrap(true);
    if (scroll) {
        auto* scrl = new QScrollArea(ovrl);
        scrl->setFrameShape(QFrame::NoFrame);

        scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrl->verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        scrl->verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
        auto drag = new DragScroll(scrl->viewport(), scrl->verticalScrollBar());
        drag->installOn(txt);

        scrl->setWidget(txt);
        scrl->setWidgetResizable(true);
        lay->addWidget(scrl);

        lay->addStretch();
    } else {
        lay->addWidget(txt);
    }
    lay->addSpacing(8);

    auto* btns = new QDialogButtonBox(ovrl);
    if (opts == Conf_YESNO) {
        btns->setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);
    } else if (opts == Conf_YESNOCANCEL) {
        btns->setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No | QDialogButtonBox::Cancel);
    } else if (opts == Conf_OK) {
        btns->setStandardButtons(QDialogButtonBox::Ok);
    }
    lay->addWidget(btns);
    for (QPushButton* b : btns->findChildren<QPushButton*>()) {
        resizeFont(b, 1.5);
        b->setIcon(QIcon());
    }

    QEventLoop loop;
    QDialogButtonBox::ButtonRole result = QDialogButtonBox::RejectRole;

    QObject::connect(btns, &QDialogButtonBox::clicked, ovrl, [&](QAbstractButton* btn) {
        result = btns->buttonRole(btn);
        loop.quit();
    });
    QObject::connect(ovrl, &ConfirmOverlay::pressed, ovrl, [&]() { loop.quit(); });

    ovrl->show();
    ovrl->raise();
    loop.exec();

    ovrl->deleteLater();
    return result;
}
