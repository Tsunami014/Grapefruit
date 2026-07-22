#pragma once
#include <QBoxLayout>
#include "base/task.hpp"

QString getCurrent();

void setTasksCatsLay(QLayout* lay, std::function<void()> redo, QWidget* parent = nullptr);
void setTasksLay(QLayout* lay, std::function<void(std::shared_ptr<Task>, bool upd)> press, QWidget* parent = nullptr);

void newCategory(QWidget* parent, QString name);
bool renameCategory(QWidget* parent, QString newname);
bool deleteCategory(QWidget* parent);

void removeTask(std::shared_ptr<Task> task, bool trycurfirst = true);
std::shared_ptr<Task> newtask();
std::shared_ptr<Task> newtask(QString cat);

void loadTasks();
void sortTasks(bool all = false);
void saveTasks();
