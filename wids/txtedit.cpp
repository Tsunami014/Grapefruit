#include "txtedit.hpp"
#include "font.hpp"
#include "extra/drag.hpp"
#include <QScrollBar>
#include <QScroller>
#include <QKeyEvent>
#include <QMimeData>
#include <QPainter>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QWindow>

TxtEdit::TxtEdit(QWidget* parent, bool bullets)
    : QTextEdit(parent) { init(bullets); }
TxtEdit::TxtEdit(const QString& text, QWidget* parent, bool bullets)
    : QTextEdit(text, parent) { init(bullets); }

void TxtEdit::init(bool bullets) {
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    viewport()->setAutoFillBackground(false);
    setFrameStyle(QFrame::NoFrame);
    resizeFont(this, 1.2);

    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    verticalScrollBar()->setFocusPolicy(Qt::NoFocus);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    auto* drag = new DragScroll(viewport(), verticalScrollBar());
    drag->installOn(this);

    if (bullets) {
        barea = new BulletArea(this);
        setViewportMargins(bulletMarginWidth, 0, 0, 0);
        // Repaint the gutter whenever the things that affect it change
        connect(this, &QTextEdit::textChanged, this, [this] { barea->update(); });
        connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this] { barea->update(); });
    }
}

void TxtEdit::focusInEvent(QFocusEvent* e) {
    QTextEdit::focusInEvent(e);
    emit focusChange(true);
}
void TxtEdit::focusOutEvent(QFocusEvent* e) {
    QTextEdit::focusOutEvent(e);
    emit focusChange(false);
}

void TxtEdit::keyPressEvent(QKeyEvent* e) {
    if (e->key() == Qt::Key_Up) {
        if (textCursor().blockNumber() == 0) {
            moveCursor(QTextCursor::Start);
            return;
        }
    } else if (e->key() == Qt::Key_Down) {
        if (textCursor().blockNumber() == document()->blockCount() - 1) {
            moveCursor(QTextCursor::End);
            return;
        }
    }
    QTextEdit::keyPressEvent(e);
}

void TxtEdit::insertFromMimeData(const QMimeData* source) {
    // Only allow text
    if (source->hasText()) {
        insertPlainText(source->text());
    }
}

void TxtEdit::resizeEvent(QResizeEvent *event) {
    QTextEdit::resizeEvent(event);
    if (barea == nullptr) return;
    QRect cr = contentsRect();
    barea->setGeometry(cr.left(), cr.top(), bulletMarginWidth, cr.height());
}
void BulletArea::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::black);
    painter.setPen(Qt::NoPen);

    QTextDocument* doc = edit->document();
    const int scrollY = edit->verticalScrollBar()->value();

    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        if (!block.isVisible() || block.text().isEmpty()) {
            continue;
        }

        QRectF brect = doc->documentLayout()->blockBoundingRect(block);
        qreal linehei = block.layout()->lineAt(0).height();
        painter.drawEllipse(QPointF(
                bulletMarginWidth / 2.0,
                brect.top() - scrollY + linehei / 2.0
            ), bulletRadi, bulletRadi);
    }
}

// Fix on mobile the handle being in the wrong spot
QVariant TxtEdit::inputMethodQuery(Qt::InputMethodQuery query) const {
    QVariant result = QTextEdit::inputMethodQuery(query);
    if (barea == nullptr) return result;

    switch (query) {
        case Qt::ImCursorRectangle:
        case Qt::ImAnchorRectangle:
        case Qt::ImInputItemClipRectangle: {
            if (result.canConvert<QRectF>()) {
                QRectF r = result.toRectF();
                r.translate(bulletMarginWidth, 0);
                return r;
            }
            break;
        }
        default: break;
    }
    return result;
}
