// Thanks in part to https://doc.qt.io/qt-6/qtwidgets-layouts-flowlayout-example.html !
#pragma once
#include <QLayout>
#include <QWidget>
#include <QStyle>
#include <QResizeEvent>
class FlowLayout : public QLayout {
public:
    explicit FlowLayout(QWidget* parent, int margin = -1, int hSpacing = -1, int vSpacing = -1);
    explicit FlowLayout(int margin = -1, int hSpacing = -1, int vSpacing = -1);
    ~FlowLayout();

    void vertical(int mxRows) { maxRows = mxRows; }
    void horizontal() { maxRows = -1; }

    void addItem(QLayoutItem* item) override;
    int horizontalSpacing() const;
    int verticalSpacing() const;
    Qt::Orientations expandingDirections() const override;
    bool hasHeightForWidth() const override { return true; };
    int heightForWidth(int) const override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;
    QSize sizeHint() const override;
    QLayoutItem* takeAt(int index) override;

    QSize lastSize() const;

private:
    int doLayout(const QRect& rect, bool testOnly = false) const;
    int smartSpacing(QStyle::PixelMetric pm) const;

    QList<QLayoutItem*> itemList;
    int hSpace;
    int vSpace;

    mutable QSize _lastSze;

    /// If maxRows <= 0 then will place elements horizontally, else will place elements vertically
    int maxRows = -1;
};

class FlowContainer : public QWidget {
public:
    explicit FlowContainer(QWidget* parent = nullptr) : QWidget(parent) {}
protected:
    void resizeEvent(QResizeEvent* event) override;
};
