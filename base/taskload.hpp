#pragma once
#include <QLayout>
#include "wids/taskbbl.hpp"

void setTasksCatsLay(QLayout* lay, std::function<void()> redo, QWidget* parent = nullptr);
void setTasksLay(QLayout* lay, std::function<void(TaskBubble*, std::shared_ptr<Task>)> connect, QWidget* parent = nullptr);
void loadTasks();
void saveTasks();
