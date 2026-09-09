#pragma once
#include "colours.hpp"
#include "extra/icon.hpp"
#include <QToolButton>

class IcoButton : public QToolButton {
    Q_OBJECT
public:
    explicit IcoButton(const QString& text, const QString& asset, QWidget* parent = nullptr)
        : QToolButton(parent) {
            setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            setText(text);
            ColGroups::setGrp(this, ColGroups::SecondaryContainer);
            setColIco(this, asset);
            setAccessibleDescription(text);
        }
};
