#include "externs.hpp"
#include "game.hpp"
#include "taskload.hpp"
#include "quals.hpp"
#include <QDebug>
#include <QTime>
#include <QRandomGenerator>

QString getTime() {
    int hour = QTime::currentTime().hour();
    if (hour < 5) {
        return "night";
    } else if (hour < 12) {
        return "morning";
    } else if (hour < 17) {
        return "afternoon";
    } else if (hour < 21) {
        return "evening";
    } else {
        return "night";
    }
}

QString bestQ;
bool hasBQ = false;
inline bool genBQ() {
    auto bt = getBestTask();
    if (bt == nullptr) return false;
    auto [bquals, sco] = bestQualities(bt->quals);
    if (sco <= 0) return false;
    if (sco > 2 ||
        (sco == 2 && QRandomGenerator::global()->bounded(2) == 0)) {
            static const QStringList choices{
                "very ",
                "very ",
                "quite ",
                "extremely ",
                "really ",
            };
            bestQ = choices[QRandomGenerator::global()->bounded(choices.size())];
    }
    if ((bquals.size() > 5) ||
        (bquals.size() > 4 && sco == 2) ||
        (bquals.size() > 3 && sco > 2) ||
        (bquals.size() >= 2 && QRandomGenerator::global()->bounded(3) == 0)) {
            // Pop a random element by swapping with the back to be O(1)
            int i = QRandomGenerator::global()->bounded(int(bquals.size()));
            bestQ += std::move(bquals[i]) + " and ";
            bquals[i] = std::move(bquals.back());
            bquals.pop_back();
    }
    bestQ += bquals[QRandomGenerator::global()->bounded(int(bquals.size()))];
    return true;
}

QString bestR;
bool hasBR = false;

void resetExterns() {
    bestQ = {};
    hasBQ = false;
    bestR = {};
    hasBR = false;
}


QString runExtern(std::string name) {
    if (name == "time")
        return getTime();
    if (name == "thistime") {
        QString t = getTime();
        if (t == "night") return "tonight";
        return "this "+t;
    }
    if (name == "scene")
        return MG->house->curScnName();
    if (name == "best_name")
        return getBestTask()->name;
    if (name == "best_qual")
        return bestQ;
    if (name == "best_reason")
        return bestR;
    return {};
}

bool evalExtern(std::string name) {
    if (name == "time" ||
        name == "thistime" ||
        name == "scene") {
        return true;
    }
    if (name == "best_name") {
        return getBestTask() != nullptr;
    }
    if (name == "best_reason") {
        if (!hasBR) {
            hasBR = true;
            auto bt = getBestTask();
            auto opts = bt->getReasons().split("\n", Qt::SkipEmptyParts);
            if (bt != nullptr || !opts.isEmpty()) return false;
            bestR = opts[QRandomGenerator::global()->bounded(opts.size())];
            return true;
        }
        return !bestR.isNull();
    }
    if (name == "best_qual") {
        if (!hasBQ) {
            hasBQ = true;
            return genBQ();
        }
        return !bestQ.isNull();
    }
    qFatal() << "Unknown external name:" << name;
    return false;
}

const std::unordered_set<std::string> externList = {
    "time", "thistime", "scene", "best_name"
};
