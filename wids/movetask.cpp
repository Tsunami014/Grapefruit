#include "movetask.hpp"
#include "confirm.hpp"
#include "flow.hpp"
#include "font.hpp"
#include "extra/drag.hpp"
#include "base/taskload.hpp"
#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>


void moveTask(QWidget* parent, std::shared_ptr<Task> task) {
    auto topLevel = parent ? parent->window() : nullptr;
    if (!topLevel) return;
    QEventLoop loop;
    QString cur = taskCategory(task);

    auto ovrl = new ConfirmOverlay(topLevel, true);

    auto lay = new QVBoxLayout(ovrl->inner);
    auto txt = new QLabel("Which category do you want to move this to?\n(Was in "+cur+")", ovrl);
    resizeFont(txt, 1.3);
    txt->setWordWrap(true);
    lay->addWidget(txt);

    auto* scrl = new QScrollArea(ovrl);
    scrl->setFrameShape(QFrame::NoFrame);
    scrl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrl->setProperty("bg", true);

    scrl->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scrl->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrl->verticalScrollBar()->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    scrl->verticalScrollBar()->setFocusPolicy(Qt::NoFocus);

    auto* cont = new QWidget(ovrl);
    cont->setObjectName("cardcont");
    auto* contlay = new FlowLayout(cont);
    for (const auto& cat : allTaskCats()) {
        if (cat == cur) continue;
        auto* btn = new QPushButton(cat, cont);
        btn->setProperty("fancy", true);
        btn->setProperty("optbtn", true);
        resizeFont(btn, 1.2);
        QObject::connect(btn, &QPushButton::clicked, [&, btn](){
            changeCat(task, btn->text(), cur);
            loop.quit();
        });
        contlay->addWidget(btn);
    }

    auto drag = new DragScroll(scrl->viewport(), scrl->verticalScrollBar());
    drag->installOn(cont);
    drag->installOn(contlay);

    scrl->setWidget(cont);
    scrl->setWidgetResizable(true);
    lay->addWidget(scrl);
    lay->addSpacing(8);

    auto* btns = new QDialogButtonBox(QDialogButtonBox::Cancel, ovrl);
    lay->addWidget(btns);
    for (QPushButton* b : btns->findChildren<QPushButton*>()) {
        resizeFont(b, 1.5);
        b->setIcon(QIcon());
    }

    // The only button here is cancel
    QObject::connect(btns, &QDialogButtonBox::clicked, ovrl, [&](QAbstractButton* btn) { loop.quit(); });
    QObject::connect(ovrl, &ConfirmOverlay::pressed, ovrl, [&]() { loop.quit(); });

    ovrl->show();
    ovrl->raise();
    loop.exec();

    ovrl->deleteLater();
    return;
}
