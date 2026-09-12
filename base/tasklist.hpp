#pragma once
#include "task.hpp"
#include "extra/itemopts.hpp"
#include <QString>

using tasklist = std::vector<std::shared_ptr<Task>>;
std::map<QString, tasklist> alltasks;


inline void defTasks() {
    tasklist home;
    home.emplace_back(std::make_shared<Task>("Meal prep", QString(
        "+40m Research one new interesting meal to cook\n"
        "+15m Plan meals for the week\n"
        "+10m Write a shopping list\n"
        "Due by @%1"
    ).arg(QDate::currentDate().addDays(15-QDate::currentDate().dayOfWeek()).toString("yyyy-MM-dd")),
    3, std::set<QString>{"reward"},
        "It will ensure everything is prepared and ready to go\n"
        "You would like meal variety"
    ));
    alltasks["Home"] = home;

    tasklist misc;
    misc.emplace_back(std::make_shared<Task>("Hobies", QString(
        "Drawing\n"
        "Board games"
    ),
    2, std::set<QString>{"simple", "enjoyable", "reward"},
        "You can enjoy some relaxing time\n"
        "You want some variety in what you do every day"
    ));
    misc.emplace_back(std::make_shared<Task>("Misc important jobs", QString(
        "+10m Reply to that email!! (You know the one)\n"
        "+20m Clear the desk\n"
        "%1+20m Sort out the mess in the cupboard"
    ).arg(donePref),
    2, std::set<QString>{"simple", "reward"},
        "You want everything to be organised"
    ));
    alltasks["Misc"] = misc;

    tasklist schl;
    schl.emplace_back(std::make_shared<Task>("Maths work", QString(
        "%1+20m Finish remaining maths homework\n"
        "+5m Submit all homework @%2\n"
        "+20m Go through the missed lesson's notes\n"
        "+40m Finish and submit work for missed lesson @%3"
    ).arg(donePref)
        .arg(QDate::currentDate().addDays(5).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(8).toString("yyyy-MM-dd")),
    3, std::set<QString>{"challenging", "lengthy", "obligation"},
        "You want to fully understand the coursework"
    ));
    schl.emplace_back(std::make_shared<Task>("Physics work", QString(
        "+20m Finish this week's homework sheet @%1\n"
        "+40m Do next week's homework sheet @%2"
    )
        .arg(QDate::currentDate().addDays(6).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(13).toString("yyyy-MM-dd")),
    3, std::set<QString>{"challenging", "lengthy", "interesting", "obligation"},
        "You want to be fully caught up with all coursework\n"
        "You want a deep understanding of every topic"
    ));
    alltasks["!School"] = schl;

    tasklist study;
    study.emplace_back(std::make_shared<Task>("Maths revision", QString(
        "Find topics I struggle with\n"
        "Do a few practice questions on it\n"
        "Try a past paper"
    ),
    2, std::set<QString>{"challenging", "obligation", "growth"},
        "You want to try fun, tricky problems"
    ));
    study.emplace_back(std::make_shared<Task>("Physics revision", QString(
        "%1+20m Create flashcards for the current topic\n"
        "+1.3h Practice the flashcards\n"
        "%1+10m Create a summary of notes\n"
        "+1.3h Take a practice test\n"
        "+20m Look over the theory of the parts of the test I did bad in\n"
        "+30m Do targeted flashcard revision\n"
        "Exam!! @%2\n"
        "Summarise a topic\n"
        "Explain a whole topic to my friend\n"
        "Do a past paper\n"
        "Research concepts I didn't understand"
    ).arg(donePref)
        .arg(QDate::currentDate().addDays(14).toString("yyyy-MM-dd")),
    4, std::set<QString>{"challenging", "lengthy", "interesting", "obligation", "growth"},
        "You want a deeper understanding of how the world works\n"
        "You want to be confident at difficult problems"
    ));
    alltasks["!Study"] = study;
}
