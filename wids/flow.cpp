// Thanks in part to https://doc.qt.io/qt-6/qtwidgets-layouts-flowlayout-example.html !
#include "flow.hpp"

FlowLayout::FlowLayout(QWidget* parent, int margin, int hSpacing, int vSpacing)
        : QLayout(parent), hSpace(hSpacing), vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}
FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
        : QLayout(), hSpace(hSpacing), vSpace(vSpacing) {
    setContentsMargins(margin, margin, margin, margin);
}
FlowLayout::~FlowLayout() {
    QLayoutItem* item;
    while ((item = takeAt(0)) != nullptr) {
        if (auto* wid = item->widget()) wid->deleteLater();
        delete item;
    }
}

int FlowLayout::horizontalSpacing() const {
    if (hSpace >= 0) { return hSpace; }
    else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}
int FlowLayout::verticalSpacing() const {
    if (vSpace >= 0) { return vSpace; }
    else {
        return smartSpacing(QStyle::PM_LayoutVerticalSpacing);
    }
}

void FlowLayout::addItem(QLayoutItem* item) {
    itemList.append(item);
}
int FlowLayout::count() const {
    return itemList.size();
}
QLayoutItem* FlowLayout::itemAt(int index) const {
    return itemList.value(index);
}
QLayoutItem* FlowLayout::takeAt(int index) {
    if (index >= 0 && index < itemList.size()) {
        return itemList.takeAt(index);
    }
    return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const { return {}; }
int FlowLayout::heightForWidth(int width) const {
    return doLayout(QRect(0, 0, width, 0), true);
}

void FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::sizeHint() const { return minimumSize(); }
QSize FlowLayout::minimumSize() const {
    QSize size;
    for (const QLayoutItem* item : std::as_const(itemList)) {
        size = size.expandedTo(item->minimumSize());
    }
    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

struct layoutPart {
    QWidget* wid;
    int basex;
    int right;
};
int FlowLayout::doLayout(const QRect& rect, bool testOnly) const {
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    std::vector<layoutPart> row;
    auto finishRow = [&]() {
        if (testOnly || row.empty()) return;
        int rowwid = row.back().right - row.front().basex;
        int ladj = qMax((effectiveRect.width() - rowwid)/int(row.size()+1), 0);
        qDebug() << row.size() << ladj << rowwid << row.back().right << row.front().basex << effectiveRect.width();
        uint i = 0;
        for (auto& it : row) {
            it.wid->setGeometry(QRect(QPoint(it.basex+ladj*(++i), y), it.wid->sizeHint()));
        }
    };

    for (QLayoutItem* item : std::as_const(itemList)) {
        QWidget* wid = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
            spaceX = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal);
        int spaceY = verticalSpacing();
        if (spaceY == -1)
            spaceY = wid->style()->layoutSpacing(
                QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical);

        int right = x + item->sizeHint().width();
        int nextX = right + spaceX;
        if (right > effectiveRect.right() && lineHeight > 0) {
            finishRow();
            row.clear();
            x = effectiveRect.x();
            y = y + lineHeight;
            right = x + item->sizeHint().width();
            nextX = right + spaceX;
            lineHeight = 0;
        }
        row.push_back({wid, x, right});
        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height() + spaceY);
    }
    finishRow();
    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pm) const {
    QObject* parent = this->parent();
    if (!parent) {
        return -1;
    } else if (parent->isWidgetType()) {
        QWidget* pw = static_cast<QWidget*>(parent);
        return pw->style()->pixelMetric(pm, nullptr, pw);
    } else {
        return static_cast<QLayout*>(parent)->spacing();
    }
}
