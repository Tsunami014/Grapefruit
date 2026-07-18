#include <QString>

std::pair<std::string, int> choose(const std::vector<std::string>& opts);
std::pair<QString, int> choose(const std::vector<QString>& opts);
std::pair<QString, int> choose(const QStringList& opts);
