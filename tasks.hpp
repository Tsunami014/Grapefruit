#pragma once
#include <QGridLayout>
#include <QBoxLayout>
#include <QPointer>
#include "wids/flow.hpp"
#include "extra/drag.hpp"

class TaskView : public QWidget {
public:
    TaskView();
private:
    QGridLayout* tlay;

    QVBoxLayout* tbbllay;
    FlowLayout* tcatlay;
    DragScroll* tcatdrag;
    QPointer<QWidget> overlay;

    void redoTasks();
};
