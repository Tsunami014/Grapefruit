#pragma once
#include <QLayout>

void setTasksCatsLay(QLayout* lay, QWidget* parent = nullptr);
void setTasksLay(QLayout* lay, QGridLayout* tlay, QWidget* parent = nullptr);
void loadTasks();
void saveTasks();
