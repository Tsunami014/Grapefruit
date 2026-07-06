#pragma once
#include "options.hpp"
#include "wids/flow.hpp"
#include <QLabel>

class Conversation {
public:
    Conversation(FlowLayout* olay, QLabel* curtxt);
    void newTopic();

protected:
    std::string purpose;
    std::unordered_set<std::string> context = {};

    FlowLayout* olay;
    QLabel* curtxt;

    /// [(sentence, idx of options in opts list), ...]
    using _sentList = std::vector<std::pair<std::string, uint>>;
    bool matches(const std::string& req, const _sentList& sents);
    std::pair<QString, uint> getSentence(const _sentList& sents);
    QString polishSentence(QString sent);

    void refresh();
    void display(QString txt, optList opts = {});
    virtual void onclick(Option o);
};
