#pragma once
#include "options.hpp"
#include "wids/flow.hpp"
#include <QLabel>

class Conversation {
public:
    Conversation(FlowLayout* olay, QLabel* curtxt);
    void newTopic();

protected:
    std::string purpose = "^";

    FlowLayout* olay;
    QLabel* curtxt;

    void refresh();
    void display(QString txt, optList opts = {});
    virtual void onclick(Option o);
};
