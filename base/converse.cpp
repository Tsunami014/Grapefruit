#include "converse.hpp"

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {
        refresh();
    }

void Conversation::newTopic() {
    refresh();
}

void Conversation::refresh() {
    display("Hello!", {{"One"}, {"Two"}});
}

void Conversation::onclick(Option o) {
    display(o.title, {{"Hi"}, {"Bye"}});
}

void Conversation::display(QString title, std::vector<Option> opts) {
    curtxt->setText(title);
    setOpts(olay, opts, [=](Option o){ onclick(o); });
}
