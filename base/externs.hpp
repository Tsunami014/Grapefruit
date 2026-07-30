#pragma once
#include <QString>
#include <unordered_set>

void resetExterns();
QString runExtern(std::string name);
bool evalExtern(std::string name);
const extern std::unordered_set<std::string> externList;
