#pragma once
#include <QGridLayout>
#include <QBoxLayout>
#include <QPointer>
#include "wids/flow.hpp"
#include "extra/drag.hpp"

class MainGame; // Forward reference
class TaskView : public QWidget {
public:
    TaskView(MainGame* mg);

private:
    QGridLayout* tlay;

    QVBoxLayout* tbbllay;
    FlowLayout* tcatlay;
    DragScroll* tcatdrag;
    QPointer<QWidget> overlay;

    void redoTasks();
};
