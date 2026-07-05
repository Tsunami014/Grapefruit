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
    std::vector<Option> opts;

    void refreshOpts();
};
