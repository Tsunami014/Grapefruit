#pragma once

#include <QSlider>
#include <QMouseEvent>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionSlider>

class Slidr : public QSlider {
    Q_OBJECT
public:
    using QSlider::QSlider;

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool dragging = false;

    void applyValueFromPos(const QPoint& pos);
    int clickValue(const QPoint& pos, const QRect& handleRect) const;
};
