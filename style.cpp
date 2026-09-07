#include "game.hpp"
#include <QFile>
#include <QStyleHints>
#include <QRegularExpression>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/qnativeinterface.h>
#endif

const QColor base(236, 161, 211);

void MainGame::setupStyle() {
    QObject::connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
              qApp, [this](Qt::ColorScheme scheme) {
        if (theme == -1) genStyle();
    });
    genStyle();
}


enum PalleteOpts {
    PRIMARY,
    SECONDARY,
    TERTIARY,
    NEUTRAL,
    NEUTRALVARIANT,
};

QColor getCol(float bhue, PalleteOpts palstyl, float tone) {
    switch (palstyl) {
        case PRIMARY:
            return QColor::fromHslF(bhue, 0.48, tone);
        case SECONDARY:
            return QColor::fromHslF(bhue, 0.16, tone);
        case TERTIARY:
            return QColor::fromHslF(
                std::fmod(bhue+(60.0f/360.0f), 1.0f), 0.24, tone);
        case NEUTRAL:
            return QColor::fromHslF(bhue, 0.04, tone);
        case NEUTRALVARIANT:
            return QColor::fromHslF(bhue, 0.08, tone);
    }
    return {};
}

constexpr int diff = 15;
inline QColor colbang(QColor orig, bool light) {
    return QColor(orig.red() - diff, orig.green() - diff, orig.blue() - diff);
}

const QRegularExpression stylRe(R"(\$([a-zA-Z]+)\$?)");
void MainGame::genStyle() {
    bool light;
    if (theme == -1) {
        Qt::ColorScheme scheme = qApp->styleHints()->colorScheme();
        light = scheme != Qt::ColorScheme::Dark; // Includes unknown
    } else { light = theme == 1; }
    float bhue = base.hueF();

    styls[Cols::Primary] = getCol(bhue, PRIMARY, light? 0.4:0.8);
    styls[Cols::OnPrimary] = getCol(bhue, PRIMARY, light? 1.0:0.2);
    styls[Cols::PrimaryContainer] = getCol(bhue, PRIMARY, light? 0.9:0.3);
    styls[Cols::OnPrimaryContainer] = getCol(bhue, PRIMARY, light? 0.1:0.9);
    styls[Cols::PrimaryInverse] = getCol(bhue, PRIMARY, light? 0.9:0.3);
    styls[Cols::OnPrimaryInverse] = getCol(bhue, PRIMARY, light? 0.1:0.9);

    styls[Cols::Secondary] = getCol(bhue, SECONDARY, light? 0.4:0.8);
    styls[Cols::OnSecondary] = getCol(bhue, SECONDARY, light? 1.0:0.2);
    styls[Cols::SecondaryContainer] = getCol(bhue, SECONDARY, light? 0.9:0.3);
    styls[Cols::OnSecondaryContainer] = getCol(bhue, SECONDARY, light? 0.1:0.9);

    styls[Cols::Tertiary] = getCol(bhue, TERTIARY, light? 0.4:0.8);
    styls[Cols::OnTertiary] = getCol(bhue, TERTIARY, light? 1.0:0.2);
    styls[Cols::TertiaryContainer] = getCol(bhue, TERTIARY, light? 0.9:0.3);
    styls[Cols::OnTertiaryContainer] = getCol(bhue, TERTIARY, light? 0.1:0.9);

    styls[Cols::Outline] = getCol(bhue, NEUTRALVARIANT, light? 0.5:0.6);
    styls[Cols::OutlineVariant] = getCol(bhue, NEUTRALVARIANT, light? 0.8:0.3); // Lighter

    styls[Cols::Surface] = getCol(bhue, NEUTRAL, light? 0.98:0.06);
    styls[Cols::OnSurface] = getCol(bhue, NEUTRAL, light? 0.1:0.9);
    styls[Cols::OnSurfaceVariant] = getCol(bhue, NEUTRALVARIANT, light? 0.3:0.8);

    styls[Cols::SurfaceContainerLow] = getCol(bhue, NEUTRAL, light? 0.96:0.10);
    styls[Cols::SurfaceContainer] = getCol(bhue, NEUTRAL, light? 0.94:0.12);
    styls[Cols::SurfaceContainerHigh] = getCol(bhue, NEUTRAL, light? 0.92:0.17);
    styls[Cols::SurfaceContainerHighest] = getCol(bhue, NEUTRAL, light? 0.90:0.22);

    static QString mstyl = [](){
        QFile file(":/style.qss");
        bool ok = file.open(QIODevice::ReadOnly);
        // Should always be ok because we're loading from a preset internal file
        return QString::fromUtf8(file.readAll());
    } ();
    QString styl = mstyl;

    auto it = stylRe.globalMatch(styl);
    int offs = 0;
    while (it.hasNext()) {
        auto m = it.next();

        QString nam = m.captured(1);
        bool dark = nam.startsWith('!');
        if (dark) nam = nam.sliced(1);

        auto col = styls[Cols::fromName(nam)];
        if (dark) col = colbang(col, light);
        QString repl = col.name();

        int start = m.capturedStart(0) + offs;
        int end = m.capturedEnd(0) + offs;
        styl.replace(start, end - start, repl);
        offs += repl.length() - (end - start);
    }

    QString grps;
    for (const auto& [g, inf] : ColGroups::Groups) {
        QColor fg = styls[inf.fg];
        QColor bg = styls[inf.bg];

        grps += "*[grp="+inf.nam+"]{" +
            "color:" + fg.name() + ";" +
            "background-color:" + bg.name() + ";" +
        "}*[grp="+inf.nam+"]:pressed{" +
            "background-color:" + colbang(bg, light).name() + ";" +
        "}";
    }
    styl.replace("^^grps", grps);

    setStyleSheet(styl);

    // If on Android, set the navigation icons to be dark/light with the theme
#ifdef Q_OS_ANDROID
    // Must use a timer because of weird Qt quirks
    if (!stylNavTimer) {
        stylNavTimer = new QTimer(this);
        stylNavTimer->setSingleShot(true);
    } else {
        stylNavTimer->disconnect();
    }

    auto updFn = [this, light]() {
        QNativeInterface::QAndroidApplication::runOnAndroidMainThread([light]() {
            QJniObject activity = QNativeInterface::QAndroidApplication::context();
            QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");

            if (QNativeInterface::QAndroidApplication::sdkVersion() >= 30) {
                QJniObject controller = window.callObjectMethod(
                    "getInsetsController", "()Landroid/view/WindowInsetsController;");
                if (controller.isValid()) {
                    const int APPEARANCE_LIGHT_STATUS_BARS = 8;
                    const int APPEARANCE_LIGHT_NAVIGATION_BARS = 16;
                    int mask = APPEARANCE_LIGHT_STATUS_BARS | APPEARANCE_LIGHT_NAVIGATION_BARS;
                    int value = light ? mask : 0;   // set flag => dark icons on light bg
                    controller.callMethod<void>("setSystemBarsAppearance", "(II)V", value, mask);
                }
            } else {
                QJniObject decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
                jint visibility = decorView.callMethod<jint>("getSystemUiVisibility", "()I");

                const int flags = 0x2000 | 0x10; // LIGHT_STATUS_BAR | LIGHT_NAVIGATION_BAR
                visibility = light ? (visibility | flags) : (visibility & ~flags);

                decorView.callMethod<void>("setSystemUiVisibility", "(I)V", visibility);
            }
        });
    };

    connect(stylNavTimer, &QTimer::timeout, this, updFn);
    stylNavTimer->start(300);
    updFn();
#endif
}
