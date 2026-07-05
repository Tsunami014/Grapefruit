#pragma once
#include <QLayout>
#include <QWidget>

struct Option {
    QString title;
};
using optList = std::vector<Option>;

void setOpts(QLayout* lay, optList opts, std::function<void(Option)> func, QWidget* parent = nullptr);
