#pragma once
#include <QToolButton>

class IcoButton : public QToolButton {
    Q_OBJECT
public:
    explicit IcoButton(const QString& text, const QString& asset, QWidget* parent = nullptr)
        : QToolButton(parent) {
            setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            setText(text);
            setIcon(QIcon(asset));
        }
};
