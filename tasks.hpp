#pragma once
#include <QGridLayout>
#include <QBoxLayout>
#include <QPointer>
#include <QScrollArea>
#include "wids/flow.hpp"
#include "extra/drag.hpp"

class TaskView : public QWidget {
public:
    TaskView();

    void redoTasks();
private:
    QGridLayout* tlay;

    QVBoxLayout* tbbllay;
    FlowLayout* tcatlay;
    DragScroll* tcatdrag;
    QScrollArea* scrl;
    QPointer<QWidget> overlay;
};
