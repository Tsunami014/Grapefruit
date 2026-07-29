#pragma once
#include <QString>
#include <unordered_set>

QString runExtern(std::string name);
void resetExterns(); // Reset for a new topic
const extern std::unordered_set<std::string> externList;
