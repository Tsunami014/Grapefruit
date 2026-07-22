#pragma once
#include <QDate>
#include <QWidget>

extern bool showingDate;
QDate getDate(const QDate& initial = {});
