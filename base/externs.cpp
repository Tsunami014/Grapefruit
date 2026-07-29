#include "externs.hpp"
#include "game.hpp"
#include "taskload.hpp"
#include <QDebug>
#include <QTime>

void resetExterns() {}

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
    if (name == "RSB") {
        resetBest();
        return "";
    }
    if (name == "best_name") {
        return getBestTask()->name;
    }
    qFatal() << "Unknown external name:" << name;
    return "";
}

const std::unordered_set<std::string> externList = {
    "time", "thistime", "scene", "RSB", "best_name"
};
