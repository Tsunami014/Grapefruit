#pragma once
#include <QDate>
#include <QWidget>

extern bool showingDate;
QDate getDate(const QDate& initial = {});

QString strTime(int minutes, bool brief = false);
QString parseDate(const QDate& date, bool brief = false);
