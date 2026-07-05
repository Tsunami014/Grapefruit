#include "options.hpp"
#include <QPushButton>

void setOpts(QLayout* lay, std::vector<Option> opts, QWidget* parent) {
    QLayoutItem* item;
    while ((item = lay->takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }

    for (const auto& o : opts) {
        auto* btn = new QPushButton(o.title, parent);
        btn->setProperty("fancy", true);
        btn->setProperty("optbtn", true);
        QFont f = btn->font();
        f.setPointSize(f.pointSize() * 1.4);
        btn->setFont(f);
        // lay->connect(btn, &QPushButton::clicked, lay, [=](){  });
        lay->addWidget(btn);
    }
}
