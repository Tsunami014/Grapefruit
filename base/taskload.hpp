#pragma once
#include <QBoxLayout>
#include "base/task.hpp"

QString getCurrent();
QString curCatName();
void showNoCat();
void showStar();
bool isStarCat();

void setTasksCatsLay(QLayout* lay, std::function<void()> redo, QWidget* parent = nullptr);
void setTasksLay(QLayout* lay, std::function<void(std::shared_ptr<Task>)> press, std::function<void()> reload, QWidget* parent = nullptr);

void newCategory(QWidget* parent, QString name);
bool renameCategory(QWidget* parent, QString newname);
bool deleteCategory(QWidget* parent);

QStringList allTaskCats();
QString taskCategory(std::shared_ptr<Task> task);
void changeCat(std::shared_ptr<Task> task, QString newcat, QString fromcat = {});
void removeTask(std::shared_ptr<Task> task, bool trycurfirst = true);
std::shared_ptr<Task> newtask();
std::shared_ptr<Task> newtask(QString cat);

void loadTasks();
/// Sorting tasks does not affect getBestTask
void sortTasks(bool all = false);
void saveTasks();
void delAllTasks();
void resetTasks();
QString getAllTasksDebugInfo();

std::shared_ptr<Task> getBestTask(bool ignoreinit = false);
void removeBest();
void resetBest();
