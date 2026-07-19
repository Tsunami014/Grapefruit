#include "drag.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <QAbstractButton>
#include <QScrollBar>
#include <QLayout>
#include <QLayoutItem>

const double FRICTION = 0.9;
const int THRESHOLD = 8;  // px needed to move by to count as dragging


DragScroll::DragScroll(QWidget* viewp, QScrollBar* scrollb)
        : QObject(viewp), viewp(viewp), scrollb(scrollb) {
    timer.start();
    connect(&tick, &QTimer::timeout, this, &DragScroll::ontick);
    viewp->installEventFilter(this);
}

void DragScroll::installOn(QWidget* w) {
    w->installEventFilter(this);
    for (auto* c : w->findChildren<QWidget*>())
        c->installEventFilter(this);
}
void DragScroll::installOn(QLayout* l) {
    if (!l) return;
    for (int i = 0; i < l->count(); ++i) {
        QLayoutItem* it = l->itemAt(i);

        if (QWidget* nw = it->widget()) installOn(nw);
        if (QLayout* nl = it->layout()) installOn(nl);
    }
}

bool DragScroll::eventFilter(QObject* obj, QEvent* ev) {
    auto* me = static_cast<QMouseEvent*>(ev);
    switch (ev->type()) {
        case QEvent::MouseButtonPress:
            if (me->button() != Qt::LeftButton) return false;
            tick.stop();
            velocity = 0;
            startPos = lastPos = me->globalPosition().toPoint();
            pressedChild = qobject_cast<QWidget*>(obj);
            pressed = true;
            dragging = false;
            lastT = timer.elapsed();
            return false; // let the click through for now

        case QEvent::MouseMove: {
            if (!pressed) return false;
            QPoint gpos = me->globalPosition().toPoint();

            if (!dragging) {
                if ((gpos - startPos).manhattanLength() <= THRESHOLD) return false;
                dragging = true;
                if (auto* btn = qobject_cast<QAbstractButton*>(pressedChild)) {
                    btn->setDown(false);
                }
                if (pressedChild) {
                    QEvent leave(QEvent::Leave);
                    QApplication::sendEvent(pressedChild, &leave);
                }
                viewp->grabMouse();
            }

            int d;
            if (scrollb->orientation() == Qt::Horizontal) {
                d = gpos.x() - lastPos.x();
            } else {
                d = gpos.y() - lastPos.y();
            }
            scrollb->setValue(scrollb->value() - d);
            lastPos = gpos;

            qint64 now = timer.elapsed();
            qint64 t = qMax<qint64>(1, now-lastT);
            lastT = now;

            // Add new instantaneous velocity with previous
            velocity = 0.8*velocity + 0.2*(double(d) / t);
            return true;
        }

        case QEvent::MouseButtonRelease: {
            if (!pressed) return false;
            pressed = false;
            bool drag = dragging;
            if (drag) viewp->releaseMouse();
            if (std::abs(velocity) > 0.03) tick.start(16);
            return drag; // Swallow release only if it was a drag
        }

        default:
            return false;
    }
}

void DragScroll::DragScroll::ontick() {
    scrollb->setValue(scrollb->value() - velocity*16.0);
    velocity *= FRICTION;

    if (std::abs(velocity) < 0.01 || scrollb->value() <= scrollb->minimum() || scrollb->value() >= scrollb->maximum()) {
        velocity = 0.0;
        tick.stop();
    }
}
