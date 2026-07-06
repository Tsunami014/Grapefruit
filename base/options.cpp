#include "options.hpp"
#include "font.hpp"
#include <QPushButton>

void setOpts(QLayout* lay, std::vector<Option> opts, std::function<void(Option)> func, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    for (auto& o : opts) {
        auto* btn = new QPushButton(o.title, parent);
        btn->setProperty("fancy", true);
        btn->setProperty("optbtn", true);
        resizeFont(btn, 1.25);
        lay->connect(btn, &QPushButton::clicked, lay, [=](){ func(o); });
        lay->addWidget(btn);
    }
}
