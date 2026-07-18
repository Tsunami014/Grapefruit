#pragma once
#include <QWidget>
#include "base/task.hpp"

class TaskOverlay : public QWidget {
public:
    explicit TaskOverlay(std::shared_ptr<Task> task, std::function<void()> update, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private:
    QWidget* bbar;
    inline QMargins totMargin();
};
