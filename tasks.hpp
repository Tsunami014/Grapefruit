#pragma once
#include <QBoxLayout>
#include <QPointer>
#include <QScrollArea>
#include <QPushButton>
#include <QLabel>
#include "wids/flow.hpp"
#include "extra/drag.hpp"

class TaskView : public QWidget {
public:
    TaskView();

    void redoTasks();
private:
    QLabel* topheader;
    QPushButton* newtaskbtn;

    QVBoxLayout* tbbllay;
    FlowLayout* tcatlay;
    DragScroll* tcatdrag;
    DragScroll* tdrag;
    QPushButton* starbtn;
    QScrollArea* scrl;
    QPointer<QWidget> overlay;
};
