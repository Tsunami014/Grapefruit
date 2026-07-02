#pragma once
#include <QTimer>
#include <QElapsedTimer>
#include <QScrollArea>

class DragScroll : public QObject {
public:
    explicit DragScroll(QWidget* viewport, QScrollBar* scrollb);
    void installOn(QWidget* w);

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;

private:
    void ontick();

    QWidget* viewp;
    QScrollBar* scrollb;
    QWidget* pressedChild = nullptr;
    QPoint startPos, lastPos;
    qint64 lastT = 0;
    bool pressed = false, dragging = false;
    double velocity = 0.0;

    QElapsedTimer timer;
    QTimer tick;
};
