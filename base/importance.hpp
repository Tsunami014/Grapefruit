#pragma once
#include "task.hpp"
#include "dbug.hpp"

extern QHash<QString, int> ratios;
void loadRatios();

/// Score out of 1000, higher score = more important
int score(std::shared_ptr<Task> task);
#ifdef DEBUG
/// Base score = score() but minus the random, meaning it is out of slightly under 1000
int basescore(const Task& task);
#endif
