#pragma once
#include <QWidget>
#include "base/task.hpp"
#include "wids/txtedit.hpp"
#include "wids/qualsEdit.hpp"

class HlTxtEdit : public TxtEdit {
    Q_OBJECT
public:
    using TxtEdit::TxtEdit;
    void highlight();
protected:
    void paintEvent(QPaintEvent* event) override;
};

class TaskOverlay : public QWidget {
public:
    explicit TaskOverlay(std::shared_ptr<Task> task, std::function<void()> ondeath, QWidget* parent = nullptr);
    ~TaskOverlay() { ondeath(); }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void generateBot();
private:
    QWidget* bbar;
    QWidget* editWid;
    HlTxtEdit* edit;
    QWidget* midwid;
    QWidget* reasonsWid;
    TxtEdit* reasons;
    QWidget* qualsWid;
    QualityEdit* quals;
    /// Parts of the layout that are always hidden when selecting a large text box
    std::vector<QWidget*> parts;
    inline QMargins totMargin();
    std::function<void()> ondeath;

    std::shared_ptr<Task> task;
};
