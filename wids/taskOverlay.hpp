#pragma once
#include <QWidget>
#include "base/task.hpp"
#include "wids/txtedit.hpp"

class TaskOverlay : public QWidget {
public:
    explicit TaskOverlay(std::shared_ptr<Task> task, std::function<void()> update, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void generateBot();
private:
    QWidget* bbar;
    TxtEdit* edit;
    std::vector<QWidget*> parts;
    inline QMargins totMargin();
};
