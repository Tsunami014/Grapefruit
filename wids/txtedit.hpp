#pragma once
#include <QTextEdit>

constexpr int bulletMarginWidth = 16;
constexpr qreal bulletRadi = 3;

class BulletArea; // fwd decl
class TxtEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit TxtEdit(QWidget* parent = nullptr, bool bullets = true);
    explicit TxtEdit(const QString& text, QWidget* parent = nullptr, bool bullets = true);

signals:
    void focusChange(bool focus);
protected:
    void focusInEvent(QFocusEvent* e) override;
    void focusOutEvent(QFocusEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void insertFromMimeData(const QMimeData* source) override;
    void resizeEvent(QResizeEvent* event) override;
    QVariant inputMethodQuery(Qt::InputMethodQuery query) const override;

private:
    void init(bool bullets);
    BulletArea* barea = nullptr;
};

class BulletArea : public QWidget {
public:
    explicit BulletArea(TxtEdit* edit) : QWidget(edit), edit(edit) {
        setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
    QSize sizeHint() const override { return QSize(bulletMarginWidth, 0); }

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    TxtEdit* edit;
};
