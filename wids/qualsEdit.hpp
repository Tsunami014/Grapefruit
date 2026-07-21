#pragma once
#include "txtedit.hpp"
#include <QFont>

class QualityEdit : public TxtEdit {
    Q_OBJECT
public:
    explicit QualityEdit(std::set<QString> words, QWidget* parent = nullptr);
    void addWord(QString word);
    void rmWord(QString word);
    void toggleWord(QString word);
    std::set<QString> getWords() { return words; }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void focusInEvent(QFocusEvent* event) override;
    void focusOutEvent(QFocusEvent* event) override;
    void updtxt();
    std::set<QString> words;
    QFont regfnt;
    QFont selfnt;
};
