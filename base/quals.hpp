#include <QString>
#include <unordered_set>
#include <set>

using qualityTyp = std::unordered_map<int, std::unordered_set<std::string>>;
std::unordered_map<QString, qualityTyp> qualities();
std::set<QString> qualkeys();
