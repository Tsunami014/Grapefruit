#include "importance.hpp"
#include <QRandomGenerator>
#include <QHash>

double Randomness(std::shared_ptr<Task> task) {
    return QRandomGenerator::global()->generateDouble();
}
double Importance(std::shared_ptr<Task> task) {
    return double(task->import)/5;
}
double Urgency(std::shared_ptr<Task> task) {
    return 0.1;
}
double Load(std::shared_ptr<Task> task) {
    return 0.1;
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
int basescore(std::shared_ptr<Task> t) {
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
