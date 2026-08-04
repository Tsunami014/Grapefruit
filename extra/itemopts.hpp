#pragma once
#include <QLabel>
#include <QBoxLayout>
#include <QTextEdit>
#include <QRegularExpression>

extern const QRegularExpression timeRe;
extern const QRegularExpression dateRe;
extern const QString donePref;

int parseStrTime(QString time);

void GenerateOpts(QWidget* parent, QBoxLayout* lay, QTextEdit* edit, bool full);
