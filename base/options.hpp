#pragma once
#include <QLayout>
#include <QWidget>

struct Option {
    QString title;
};

void setOpts(QLayout* lay, std::vector<Option> opts, std::function<void(Option)> func, QWidget* parent = nullptr);
