#pragma once
#include "options.hpp"
#include "wids/flow.hpp"
#include <QLabel>

class Conversation {
public:
    Conversation(FlowLayout* olay, QLabel* curtxt);
    void newTopic();
    std::unordered_set<std::string> getContext() { return context; }

protected:
    std::string purpose;
    std::unordered_set<std::string> context = {};

    FlowLayout* olay;
    QLabel* curtxt;

    QString polishSentence(QString sent);

    void refresh();
    void display(QString txt, optList opts = {});
    virtual void onclick(Option o);
};
