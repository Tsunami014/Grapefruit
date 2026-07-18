#pragma once
#include <QBoxLayout>
#include "wids/taskbbl.hpp"

QString getCurrent();

void setTasksCatsLay(QLayout* lay, std::function<void()> redo, QWidget* parent = nullptr);
void setTasksLay(QLayout* lay, std::function<void(TaskBubble*, std::shared_ptr<Task>)> connect, QWidget* parent = nullptr);

void newCategory(QString name);
bool renameCategory(QString newname);
bool deleteCategory(QWidget* parent);

void loadTasks();
void saveTasks();
