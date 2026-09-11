#include <QString>
#include <unordered_set>
#include <set>

struct qualityTyp {
    bool fact;
    using _qualityListTyp = std::unordered_map<int, std::unordered_set<std::string>>;
    _qualityListTyp map;
};
const QMap<QString, qualityTyp>& qualities();
const std::set<QString>& qualkeys();
bool isQualFact(QString qnam);

double scoreQualities(std::set<QString> quals);
