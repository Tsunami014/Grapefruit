#include "taskload.hpp"

std::vector<std::shared_ptr<Task>> taskslist;

void loadTasks() {
    taskslist.emplace_back(std::make_shared<Task>("Sample task 1"));
    taskslist.emplace_back(std::make_shared<Task>("Sample task 2"));
}
void saveTasks() {}
