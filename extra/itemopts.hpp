#pragma once
#include <QLabel>
#include <QBoxLayout>
#include <QTextEdit>
#include <QRegularExpression>

extern const QRegularExpression timeRe;
extern const QString donePref;

void GenerateOpts(QWidget* parent, QBoxLayout* lay, QTextEdit* edit);
