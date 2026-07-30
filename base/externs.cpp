#include "externs.hpp"
#include "game.hpp"
#include "taskload.hpp"
#include <QDebug>
#include <QTime>

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

void resetExterns() {
    bestQ = {};
    hasBQ = false;
}

QString runExtern(std::string name) {
    if (name == "time") {
        return getTime();
    }
    if (name == "thistime") {
        QString t = getTime();
        if (t == "night") return "tonight";
        return "this "+t;
    }
    if (name == "scene") {
        return MG->house->curScnName();
    }
    if (name == "best_name") {
        return getBestTask()->name;
    }
    if (name == "best_qual") {
        return bestQ;
    }
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
    if (name == "best_qual") {
        if (!hasBQ) {
            hasBQ = true;
            auto bt = getBestTask();
            if (bt == nullptr) return false;
            bestQ = bt->bestQual();
        }
        return !bestQ.isNull();
    }
    qFatal() << "Unknown external name:" << name;
    return false;
}

const std::unordered_set<std::string> externList = {
    "time", "thistime", "scene", "best_name"
};
