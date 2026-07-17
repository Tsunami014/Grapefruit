#include "house.hpp"
#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <QFile>
#include <QDomDocument>
#include <QSvgRenderer>

House::House(QWidget* parent) : scn(), QGraphicsView(parent) {
    setScene(&scn);
    QFile file(":/assets/BG/furnature.svg");
    bool ok = file.open(QIODevice::ReadOnly);

    QDomDocument doc;
    doc.setContent(&file);

    auto child = doc.firstChild().firstChild();
    QDomElement found;
    while (!child.isNull()) {
        if (child.isElement()) {
            auto elm = child.toElement();
            if (elm.attribute("id") == "table") {
                found = elm; break;
            }
        }
        child = child.nextSibling();
    }
    if (found.isNull()) { qWarning() << "Failed to find element!"; return; }
    found.setAttribute("xmlns", "http://www.w3.org/2000/svg");

    QString svg;
    QTextStream stream(&svg);
    found.save(stream, 0);

    QSvgRenderer *renderer = new QSvgRenderer();
    renderer->load(svg.toUtf8());
    if (!renderer->isValid()) { qWarning() << "Failed to render svg!"; return; }
    QGraphicsSvgItem *it = new QGraphicsSvgItem();
    it->setSharedRenderer(renderer);
    it->setFlag(QGraphicsItem::ItemIsMovable);

    scn.addItem(it);
    fitInView(it, Qt::KeepAspectRatio);
}
