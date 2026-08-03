#include <QString>
#include <unordered_set>
#include <set>

using qualityTyp = std::unordered_map<int, std::unordered_set<std::string>>;
const std::unordered_map<QString, qualityTyp>& qualities();
const std::set<QString>& qualkeys();
const std::unordered_set<QString>& goodquals();

double scoreQualities(std::set<QString> quals);
std::pair<std::vector<QString>, int> bestQualities(std::set<QString> quals);
