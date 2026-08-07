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
        "+2h Make another square with the new stitch\n"
        "+20m Plan a small crochet project"
    ).arg(donePref),
    1, std::set<QString>{"immersive", "distracting", "repetitive"},
        "You find crochet relaxing and enjoyable\n"
        "You like seeing your ideas take shape"
    ));
    home.emplace_back(std::make_shared<Task>("Personal projects", QString(
        "+1.7h Bake something to share with neighbours @%1\n"
        "+30m Work on a job application that excites you\n"
        "+5m Submit the application"
    ).arg(QDate::currentDate().addDays(7).toString("yyyy-MM-dd")),
    3, std::set<QString>{"impactful", "empowering"}));
    alltasks["Home"] = home;

    tasklist schl;
    schl.emplace_back(std::make_shared<Task>("Maths", QString(
        "%1+20m Finish remaining maths homework\n"
        "+5m Submit all homework @%2\n"
        "+20m Go through the missed lesson's notes\n"
        "+40m Finish and submit work for missed lesson @%3"
    ).arg(donePref)
        .arg(QDate::currentDate().addDays(5).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(8).toString("yyyy-MM-dd")),
    2, std::set<QString>{"repetitive", "satisfying", "time-consuming"},
        "You enjoy the satisfaction of solving problems\n"
        "You like the feeling of really understanding something"
    ));
    schl.emplace_back(std::make_shared<Task>("Physics", QString(
        "%1+20m Create flashcards for the current topic\n"
        "+1.3h Practice the flashcards\n"
        "%1+10m Create a summary of notes\n"
        "+40m Practice the flashcards some more\n"
        "Exam!! @%2"
    ).arg(donePref)
        .arg(QDate::currentDate().addDays(12).toString("yyyy-MM-dd")),
    4, std::set<QString>{"complex", "frustrating", "interesting"},
        "You're curious the world works\n"
        "You like it when theory matches up with what you see in the real world\n"
        "You enjoy the challenge of working through tricky problems"
    ));
    alltasks["!School"] = schl;
}
