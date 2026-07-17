#include "bgitem.hpp"
#include "wids/house.hpp"
#include <QFile>
#include <QDomDocument>
#include <QSvgRenderer>

QHash<QString, QHash<QString, QDomElement>> fileCache;
QHash<QString, QByteArray> svgCache;

void loadFile(QString fle) {
    QString pth = ":/assets/BG/"+fle+".svg";
    QFile file(pth);
    if (!file.open(QIODevice::ReadOnly)) {
        qFatal() << "Unable to read from file " << pth;
        return;
    }

    QHash<QString, QDomElement> out;
    QDomDocument doc;
    doc.setContent(&file);
    auto child = doc.firstChild().firstChild();
    QDomElement found;
    while (!child.isNull()) {
        if (child.isElement()) {
            auto elm = child.toElement();
            elm.setAttribute("xmlns", "http://www.w3.org/2000/svg");
            out.insert(elm.attribute("id"), elm);
        }
        child = child.nextSibling();
    }

    fileCache.insert(fle, out);
}

#ifdef BUILD
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
class SvgItem : public QGraphicsSvgItem {
public:
    SvgItem(QGraphicsItem* parent = nullptr) : QGraphicsSvgItem(parent) {}

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override {
        QImage image(boundingRect().size().toSize(), QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter painter(&image);
        QStyleOptionGraphicsItem option;
        paint(&painter, &option, nullptr);
        painter.end();

        if (image.pixelColor(event->pos().toPoint()).alpha() > 0) {
            QGraphicsSvgItem::mousePressEvent(event);
        } else {
            event->ignore();
        }
    }
};
#else
using SvgItem = QGraphicsSvgItem;
#endif


QGraphicsSvgItem* bgItem(QString asset, QGraphicsItem* parent) {
    QByteArray out;
    if (svgCache.contains(asset)) {
        out = svgCache[asset];
    } else {
        auto parts = asset.split('/');
        QString fle = parts.at(0);
        QString name = parts.at(1);

        if (!fileCache.contains(fle)) { loadFile(fle); }
        auto its = fileCache[fle];
        if (!its.contains(name)) {
            qFatal() << "Failed to find element" << name << "in file" << fle;
            return nullptr;
        }

        QString svg;
        QTextStream stream(&svg);
        its[name].save(stream, 0);
        out = svg.toUtf8();
        svgCache.insert(asset, out);
    }

    QSvgRenderer* renderer = new QSvgRenderer();
    renderer->load(out);
    if (!renderer->isValid()) {
        qFatal() << "Failed to render svg!";
        return nullptr;
    }
    QGraphicsSvgItem* it = new SvgItem(parent);
    it->setSharedRenderer(renderer);
    return it;
}
