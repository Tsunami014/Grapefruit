#include "icon.hpp"
#include "game.hpp"
#include "colours.hpp"
#include <QIconEngine>
#include <QSvgRenderer>
#include <QPainter>
#include <QFile>

/// To render the custom SVG as a QIcon
class SvgIconEngine : public QIconEngine {
public:
    explicit SvgIconEngine(QByteArray svg)
        : svg(std::move(svg)) {}

    void paint(QPainter* painter,
               const QRect& rect,
               QIcon::Mode, QIcon::State
               ) override {
        QSvgRenderer renderer(svg);
        renderer.setAspectRatioMode(Qt::KeepAspectRatio);
        renderer.render(painter, rect);
    }

    QPixmap pixmap(const QSize& size,
            QIcon::Mode, QIcon::State) override {
        QPixmap result(size);
        result.fill(Qt::transparent);

        QPainter painter(&result);
        QSvgRenderer renderer(svg);
        renderer.setAspectRatioMode(Qt::KeepAspectRatio);
        renderer.render(&painter);

        return result;
    }

    QIconEngine* clone() const override {
        return new SvgIconEngine(svg);
    }

private:
    QByteArray svg;
};


void setColIco(QAbstractButton* wid, const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qFatal() << "Could not open" << path;
    }
    const QString basesvg = QString::fromUtf8(file.readAll());

    auto update = [wid, path, basesvg]() {
        if (auto grp = ColGroups::getGrp(wid)) {
            auto col = ColGroups::Groups.at(*grp).fg;

            QString svg = basesvg;
            svg = svg.replace("#1C274C", MG->styls[col].name(), Qt::CaseInsensitive);
            wid->setIcon(QIcon(new SvgIconEngine(svg.toUtf8())));
        } else {
            qWarning() << "No colour group for" << wid;
        }
    };

    update();
    QObject::connect(MG, &MainGame::themeChange, wid, update);
}
