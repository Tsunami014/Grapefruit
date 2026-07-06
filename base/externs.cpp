#include "externs.hpp"
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

QString runExtern(std::string name) {
    if (name == "time_of_day") {
        return getTime();
    }
    qFatal() << "Unknown external name:" << name;
    return "";
}
