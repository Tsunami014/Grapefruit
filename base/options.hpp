#pragma once
#include <QLayout>
#include <QWidget>

struct Option {
    QString title;
};

void setOpts(QLayout* lay, std::vector<Option> opts, QWidget* parent = nullptr);
