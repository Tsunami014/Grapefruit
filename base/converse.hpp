#pragma once
#include "options.hpp"
#include "wids/flow.hpp"
#include <QLabel>

class Conversation {
public:
    Conversation(FlowLayout* olay, QLabel* curtxt);
    void newTopic();

protected:
    FlowLayout* olay;
    QLabel* curtxt;

    void refresh();
    void display(QString txt, std::vector<Option> opts);
    virtual void onclick(Option o);
};
