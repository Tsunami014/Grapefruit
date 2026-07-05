#include "converse.hpp"

Conversation::Conversation(FlowLayout* olay, QLabel* curtxt)
    : olay(olay), curtxt(curtxt) {}

void Conversation::newTopic() {
    refreshOpts();
}

void Conversation::refreshOpts() { setOpts(olay, opts); }
