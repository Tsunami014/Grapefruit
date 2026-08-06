#pragma once
#include "task.hpp"
#include "extra/itemopts.hpp"
#include <QString>

using tasklist = std::vector<std::shared_ptr<Task>>;
std::map<QString, tasklist> alltasks;


inline void defTasks() {
    tasklist home;
    home.emplace_back(std::make_shared<Task>("Crochet", QString(
        "%1+2h Finish crochet square\n"
        "%1+10m Learn new crochet stitch\n"
        "+30m Practice new crochet stitch a little\n"
        "+2.5h Make another square with the new stitch\n"
        "+30m Plan a large crochet project"
    ).arg(donePref),
    1, std::set<QString>{"immersive", "distracting", "repetitive"},
        "You enjoy crochet"
    ));
    home.emplace_back(std::make_shared<Task>("Important reminders", QString(
        "+30m Write job application\n"
        "+5m Submit job application\n"
        "+1.7h Cook a cake for the street party @%1"
    ).arg(QDate::currentDate().addDays(2).toString("yyyy-MM-dd")),
    5, std::set<QString>{"impactful", "empowering"}
    ));
    alltasks["Home"] = home;

    tasklist schl;
    schl.emplace_back(std::make_shared<Task>("Maths", QString(
        "%1+20m Finish remaining maths homework\n"
        "+5m Submit all homework @%2\n"
        "+30m Go through the missed lesson's notes\n"
        "+45m Finish and submit work for missed lesson @%3"
    ).arg(donePref)
        .arg(QDate::currentDate().addDays(5).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(8).toString("yyyy-MM-dd")),
    2, std::set<QString>{"repetitive", "satisfying", "time-consuming"},
        "You want to get good at maths\n"
        "You don't want to fall behind in maths"
    ));
    schl.emplace_back(std::make_shared<Task>("Physics", QString(
        "%1+20m Create flashcards for the current topic\n"
        "+1.3h Practice the flashcards\n"
        "%1+10m Create a summary of notes\n"
        "+1h Practice the flashcards some more\n"
        "Exam!! @%2"
    ).arg(donePref)
        .arg(QDate::currentDate().addDays(12).toString("yyyy-MM-dd")),
    4, std::set<QString>{"complex", "frustrating", "interesting"},
        "You want to be good at physics\n"
        "You may want a job in physics"
    ));
    alltasks["!School"] = schl;
}
