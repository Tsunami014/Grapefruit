#pragma once
#include "tasks.hpp"
#include <memory>

extern std::vector<std::shared_ptr<Task>> taskslist;
void loadTasks();
void saveTasks();
