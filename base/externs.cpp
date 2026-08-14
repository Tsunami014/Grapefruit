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

QString bestR;
bool hasBR = false;

void resetExterns() {
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
    if (name == "best_reason")
        return bestR;
    return {};
}

bool evalExtern(std::string name, QString whole) {
    if (name == "time" ||
        name == "thistime" ||
        name == "scene") {
        return true;
    }
    if (name == "best_name") {
        return getBestTask(whole.startsWith("@ ")) != nullptr;
    }
    if (name == "best_reason") {
        if (!hasBR) {
            hasBR = true;
            auto bt = getBestTask();
            if (bt == nullptr) return false;
            auto opts = bt->getReasons().split("\n", Qt::SkipEmptyParts);
            if (opts.isEmpty()) return false;
            bestR = opts[QRandomGenerator::global()->bounded(opts.size())].trimmed();
            return true;
        }
        return !bestR.isNull();
    }
    qFatal() << "Unknown external name:" << name;
    return false;
}

const std::unordered_set<std::string> externList = {
    "time", "thistime", "scene", "best_name", "best_reason"
};

void externFunc(std::string name) {
    if (name == "reset_best") {
        removeBest();
        return;
    }
    qFatal() << "Unknown external function name:" << name;
    return;
}
