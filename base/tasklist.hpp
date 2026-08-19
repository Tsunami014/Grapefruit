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
    1, std::set<QString>{"immersive", "repetitive", "empowering", "satisfying"},
        "You find crochet relaxing and enjoyable\n"
        "You like seeing your ideas take shape"
    ));
    alltasks["Home"] = home;

    tasklist misc;
    misc.emplace_back(std::make_shared<Task>("Prepare for the street party", QString(
        "+10m Move the appointment on that day @%1\n"
        "+1.7h Bake the biscuits I'll bring @%2\n"
        "PARTY @%3"
    )
        .arg(QDate::currentDate().addDays(9).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(14).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(13).toString("yyyy-MM-dd")),
    3, std::set<QString>{"boring", "small", "irritating"},
        "You enjoy contributing to make other people happy"
    ));
    misc.emplace_back(std::make_shared<Task>("Misc important jobs", QString(
        "+10m Reply to that email!! (You know the one)\n"
        "+20m Clear the desk\n"
        "%1+20m Sort out the mess in the cupboard"
    ).arg(donePref),
    3, std::set<QString>{"boring", "small", "irritating"},
        "It'll stop nagging at the back of your mind\n"
        "You feel satisfied when everything is in order"
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
    3, std::set<QString>{"repetitive", "satisfying", "time-consuming"},
        "You enjoy knowing that you understand the coursework"
    ));
    schl.emplace_back(std::make_shared<Task>("Physics work", QString(
        "+20m Finish this week's homework sheet @%1\n"
        "+40m Do next week's homework sheet @%2"
    )
        .arg(QDate::currentDate().addDays(6).toString("yyyy-MM-dd"))
        .arg(QDate::currentDate().addDays(13).toString("yyyy-MM-dd")),
    3, std::set<QString>{"complex", "frustrating", "interesting"},
        "You like being up-to-date with course work\n"
        "You enjoy the coursework and want to know it all"
    ));
    alltasks["!School"] = schl;

    tasklist study;
    study.emplace_back(std::make_shared<Task>("Maths revision", QString(
        "Find topics I struggle with\n"
        "Do a few practice questions on it\n"
        "Try a past paper"
    ),
    2, std::set<QString>{"repetitive", "overwhelming", "time-consuming"},
        "You enjoy working through tricky problems\n"
        "You like the feeling of finally understanding something"
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
    ),
    4, std::set<QString>{"complex", "time-consuming", "interesting"},
        "You're curious how the world works\n"
        "You like it when theory matches up with what you see in the real world\n"
        "You enjoy the challenge of working through tricky problems"
    ));
    alltasks["!Study"] = study;
}
