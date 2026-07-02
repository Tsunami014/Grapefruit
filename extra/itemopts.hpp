#pragma once
#include <QWidget>
#include <QBoxLayout>
#include <QTextEdit>
#include <QRegularExpression>

const QRegularExpression timeRe(R"((?<=^|[ \n<>])#([a-zA-Z])(?=[ \n<>]|$))");

void GenerateOpts(QWidget* parent, QBoxLayout* lay, QTextEdit* edit, bool focus);
