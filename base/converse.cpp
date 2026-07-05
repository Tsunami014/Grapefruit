#include "converse.hpp"

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {
        opts = {{"Hi"}, {"Bye"}};
        curtxt->setText("Hello!");
        refresh();
    }

void Conversation::newTopic() {
    curtxt->setText("What would you like to talk about?");
    refresh();
}

void Conversation::onclick(Option o) {
    curtxt->setText(o.title);
}

void Conversation::refresh() { setOpts(olay, opts, [=](Option o){ onclick(o); }); }
