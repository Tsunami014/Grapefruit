#pragma once
#include <QWidget>
#include <QLabel>
#include <QPointer>
#include <QLineEdit>
#include "base/task.hpp"
#include "wids/txtedit.hpp"
#include "wids/qualsEdit.hpp"

enum class BotSection { Overview, Edit, Reasons, Quals };

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
    explicit TaskOverlay(std::shared_ptr<Task> task, std::function<void()> ondeath, QWidget* ref, QWidget* parent = nullptr);
    ~TaskOverlay() { ondeath(); }

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void generateBot();
    void updateBot();

    bool eventFilter(QObject* watched, QEvent* event) override;
    void setSze();
private:
    QWidget* ref;

    QPointer<QWidget> bbar;
    QWidget* editWid;
    HlTxtEdit* edit;
    QWidget* midwid;
    QWidget* reasonsWid;
    TxtEdit* reasons;
    QWidget* qualsWid;
    QualityEdit* quals;
    QPointer<QLabel> tbtxt;
    /// Parts of the layout that are always hidden when selecting a large text box
    std::vector<QWidget*> parts;
    /// Parts of the layout that are hidden when the title is selected
    std::vector<QWidget*> topparts;
    QLineEdit* titl;
    inline QMargins totMargin();
    std::function<void()> ondeath;

    QScrollBar* botScrl = nullptr;
    BotSection section = BotSection::Overview;
    int scrolls[4] = {0, 0, 0, 0};

    std::shared_ptr<Task> task;
};
