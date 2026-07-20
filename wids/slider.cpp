#include "slider.hpp"

void Slidr::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) {
        QSlider::mousePressEvent(event);
        return;
    }
    dragging = true;
    setSliderDown(true);
    applyValueFromPos(event->pos());
    event->accept();
}
void Slidr::mouseMoveEvent(QMouseEvent* event) {
    if (dragging) {
        applyValueFromPos(event->pos());
        event->accept();
        return;
    }
    QSlider::mouseMoveEvent(event);
}
void Slidr::mouseReleaseEvent(QMouseEvent* event) {
    clearFocus();
    if (event->button() == Qt::LeftButton && dragging) {
        applyValueFromPos(event->pos());
        dragging = false;
        setSliderDown(false);
        event->accept();
        return;
    }
    QSlider::mouseReleaseEvent(event);
}

void Slidr::paintEvent(QPaintEvent* event) {
    QSlider::paintEvent(event);
    // Paint the ticks
    if (tickPosition() == QSlider::NoTicks || tickInterval() <= 0) return;

    QStyleOptionSlider opt;
    initStyleOption(&opt);
    const QRect grooveRect = style()->subControlRect(
        QStyle::CC_Slider, &opt, QStyle::SC_SliderGroove, this);
    const QRect handleRect = style()->subControlRect(
        QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);

    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);
    QPen pen(Qt::black);
    pen.setWidth(2);
    p.setPen(pen);

    if (maximum() <= minimum()) return;

    for (int v = minimum(); v <= maximum(); v += tickInterval()) {
        if (v == value()) continue;
        if (orientation() == Qt::Horizontal) {
            const int available = grooveRect.width() - handleRect.width();
            const int x = grooveRect.left() + handleRect.width() / 2
                + QStyle::sliderPositionFromValue(minimum(), maximum(), v, available, false);
            p.drawLine(x, grooveRect.top(), x, grooveRect.bottom());
        } else {
            const int available = grooveRect.height() - handleRect.height();
            const int y = grooveRect.top() + handleRect.height() / 2
                + QStyle::sliderPositionFromValue(minimum(), maximum(), v, available, true);
            p.drawLine(grooveRect.left(), y, grooveRect.right(), y);
        }
    }
}

void Slidr::applyValueFromPos(const QPoint& pos) {
    QStyleOptionSlider opt;
    initStyleOption(&opt);
    const QRect handleRect = style()->subControlRect(
        QStyle::CC_Slider, &opt, QStyle::SC_SliderHandle, this);
    setValue(clickValue(pos, handleRect));
}
int Slidr::clickValue(const QPoint& pos, const QRect& handleRect) const {
    bool horiz = orientation() == Qt::Horizontal;
    int avaliable; int clickpos;
    if (horiz) {
        avaliable = width() - handleRect.width();
        clickpos = pos.x() - handleRect.width() / 2;
    } else {
        avaliable = height() - handleRect.height();
        clickpos = pos.y() - handleRect.height() / 2;
    }
    return QStyle::sliderValueFromPosition(
        minimum(), maximum(), clickpos, avaliable, !horiz);
}
