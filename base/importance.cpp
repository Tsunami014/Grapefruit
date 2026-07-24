#include "importance.hpp"
#include <QRandomGenerator>
#include <QHash>

double Randomness(std::shared_ptr<Task> task) {
    return QRandomGenerator::global()->generateDouble();
}
double Importance(std::shared_ptr<Task> task) {
    return double(task->import)/5;
}

constexpr double k = 14.0; // Number of days until it becomes half as important
constexpr double s = 10.0; // The 'speed' of the change - each increase makes it take an extra day to reach the same value (relative to k)
double datesigm(QDate date, double max, double min) {
    if (date.isNull()) return min;
    return (max-min) / (1.0 + std::exp((QDate::currentDate().daysTo(date) - k) / s)) + min;
}
double Urgency(std::shared_ptr<Task> task) {
    auto p = task->Progress();
    if (p.isEmpty()) return 0.0;
    return (datesigm(p.nextDue, 7, 1) + datesigm(p.lastDue, 3, 0.5))/10;
}
constexpr double a = 7.0; // The 'speed' of the change (or slope of the middle)
constexpr double b = 6.0; // Number of hours until it becomes twice as load-y (when c is 1)
constexpr double c = 0.22; // Initial dampening (takes longer to get started)
double timesigm(float time, double max, double min) {
    return (max-min) * std::pow(1+std::pow(double(time)/b, -a), -c) + min;
}
double Load(std::shared_ptr<Task> task) {
    auto p = task->Progress();
    if (p.isEmpty()) return 0.0;
    return (timesigm(p.nextTime, 7, 1) * datesigm(p.nextDue, 1, 0.5) +
        timesigm(p.totTime, 3, 0.5) * datesigm(p.nextDue, 1, 0.5))/10;
}
double Resonance(std::shared_ptr<Task> task) {
    return 0.1;
}
double Momentum(std::shared_ptr<Task> task) {
    return 0.1;
}
double Variety(std::shared_ptr<Task> task) {
    return 0.1;
}
double CatVariety(std::shared_ptr<Task> task) {
    return 0.1;
}


using _ratioNamesTyp = QHash<QString, std::function<double(std::shared_ptr<Task>)>>;
_ratioNamesTyp _ratioNames() {
    _ratioNamesTyp rn;
    rn.insert("Randomness", Randomness);
    rn.insert("Importance", Importance);
    rn.insert("Urgency", Urgency);
    rn.insert("Load", Load);
    rn.insert("Resonance", Resonance);
    rn.insert("Momentum", Momentum);
    rn.insert("Variety", Variety);
    rn.insert("CatVariety", CatVariety);
    return rn;
}
const _ratioNamesTyp ratioNames = _ratioNames();
QHash<QString, int> ratios;

void loadRatios() {
    ratios.insert("Randomness", 1);
    ratios.insert("Importance", 1);
    ratios.insert("Urgency", 4);
    ratios.insert("Load", 4);
    ratios.insert("Resonance", 4);
    ratios.insert("Momentum", 2);
    ratios.insert("Variety", 2);
    ratios.insert("CatVariety", 2);
}

/// Out of 1000, higher = more important
int score(std::shared_ptr<Task> t) {
    double tot = 0.0;
    int totRatio = 0;
    for (auto it = ratioNames.constBegin(); it != ratioNames.constEnd(); it++) {
        const QString& name = it.key();
        const auto& func = it.value();

        int weight = ratios.value(name, 0);
        tot += func(t) * weight;
        totRatio += weight;
    }
    if (totRatio == 0) return 0;
    return int((tot / totRatio) * 1000);
}
#ifdef DEBUG
int basescore(const Task& task) {
    auto t = std::make_shared<Task>(task);
    double tot = 0.0;
    int totRatio = 0;
    for (auto it = ratioNames.constBegin(); it != ratioNames.constEnd(); it++) {
        const QString& name = it.key();
        if (name == "Randomness") continue;
        const auto& func = it.value();

        int weight = ratios.value(name, 0);
        tot += func(t) * weight;
        totRatio += weight;
    }
    if (totRatio == 0) return 0;
    return int((tot / totRatio) * 1000);
}
#endif
