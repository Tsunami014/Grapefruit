#pragma once
#include "task.hpp"

void resetVariety();
void justSuggested(std::shared_ptr<Task> task);

extern QHash<QString, int> ratios;
void loadRatios();

/// Score out of 1000, higher score = more important
int score(std::shared_ptr<Task> task);
/// Base score = score minus the random
int basescore(const Task& task);
int baseextra(); // Get the amount that randomness can give to a task
