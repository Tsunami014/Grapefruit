#include <QString>
#include <QLayout>
#include <unordered_set>
#include <set>

using qualityTyp = std::unordered_map<int, std::unordered_set<std::string>>;
const std::unordered_map<QString, qualityTyp>& qualities();
const std::set<QString>& qualkeys();

void addQualityBtns(QLayout* lay, QWidget* parent);
