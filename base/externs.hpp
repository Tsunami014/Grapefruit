#pragma once
#include <QString>
#include <unordered_set>

QString runExtern(std::string name);
const extern std::unordered_set<std::string> externList;
