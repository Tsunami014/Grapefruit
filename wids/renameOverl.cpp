#include "renameOverl.hpp"
#include "font.hpp"
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QApplication>
#include <QTimer>

constexpr int MARGIN = 8;

RenameOverlay::RenameOverlay(QString title, QString initial,
        std::function<void(QString)> done,
        QWidget* ref, QWidget* parent
        ) : QWidget(parent), ref(ref) {
    if (parent) {
        setGeometry(parent->rect());
        parent->installEventFilter(this);
    }

    auto* outerV = new QVBoxLayout(this);
    outerV->setContentsMargins(0,0,0,0);
    auto* outerH = new QHBoxLayout();
    outerH->setContentsMargins(0,0,0,0);
    outerV->addStretch();
    outerV->addLayout(outerH);
    outerV->addStretch();

    main = new QWidget(this);
    outerH->addStretch(1);
    outerH->addWidget(main, 7);
    outerH->addStretch(1);
    auto vlay = new QVBoxLayout(main);

    auto labl = new QLabel(title, main);
    labl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    resizeFont(labl, 1.2);
    labl->setWordWrap(true);
    vlay->addWidget(labl);

    auto hlay = new QHBoxLayout();
    vlay->addLayout(hlay);

    auto le = new QLineEdit(initial, this);
    le->setMaxLength(20);
    resizeFont(le, 1.4);
    connect(le, &QLineEdit::returnPressed, this, [=](){
        QString txt = le->text();
        deleteLater();
        QTimer::singleShot(0, [=](){ done(txt); });
    });
    QTimer::singleShot(0, this, [=](){
        le->setFocus();
        QGuiApplication::inputMethod()->show();
    });
    hlay->addWidget(le);

    auto btn = new QPushButton();
    btn->setProperty("thm", "secondary");
    btn->setProperty("btnsty", "round");
    btn->setIcon(QIcon(":/assets/UI/check.svg"));
    int mx = le->rect().height() + 8;
    btn->setIconSize(QSize(mx, mx-4));
    connect(btn, &QPushButton::clicked, le, &QLineEdit::returnPressed);
    hlay->addWidget(btn);

    setSze();
    if (parent) parent->installEventFilter(this);
    else qApp->installEventFilter(this);
}
bool RenameOverlay::eventFilter(QObject* watched, QEvent* event) {
    if (watched == parent() && event->type() == QEvent::Resize) {
        setSze();
    }
    return QWidget::eventFilter(watched, event);
}
void RenameOverlay::setSze() {
    auto geom = QGuiApplication::primaryScreen()->geometry();
    QRect av(ref->mapToGlobal(QPoint(0, 0)), ref->size());
    setGeometry(geom);
    setContentsMargins(
        av.left()-geom.left(),
        av.top()-geom.top(),
        geom.right()-av.right(),
        geom.bottom()-av.bottom()
    );
}
void RenameOverlay::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Back) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
        } else { deleteLater(); }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
void RenameOverlay::mousePressEvent(QMouseEvent* event) {
    auto point = event->position().toPoint();
    if (!main->geometry().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN).contains(point)) {
        if (QGuiApplication::inputMethod()->isVisible()) {
            QGuiApplication::inputMethod()->hide();
            if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
        } else {
            deleteLater();
        }
        event->accept();
        return;
    }
    QWidget* wid = QApplication::widgetAt(point);
    if (qobject_cast<QLineEdit*>(wid)) { event->ignore(); }
    else if (QWidget* focus = QApplication::focusWidget()) { focus->clearFocus(); }
}

void RenameOverlay::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(125, 125, 125, 125));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255));
    painter.drawRoundedRect(main->geometry().adjusted(-MARGIN, -MARGIN, MARGIN, MARGIN), 14, 14);
}
