#pragma once
#include <QLayout>
#include <QWidget>

struct Option {
    QString title;
    std::unordered_set<std::string> changes;
    std::string newpurp = "";
};
using optList = std::vector<Option>;

void setOptsLay(QLayout* lay, optList opts, std::function<void(Option)> func, QWidget* parent = nullptr);
