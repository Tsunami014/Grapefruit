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
    genStyle(false);
}


enum PalleteOpts {
    FIXED,
    PRIMARY,
    SECONDARY,
    TERTIARY,
    NEUTRAL,
    NEUTRALVARIANT,
};

QColor getCol(float bhue, PalleteOpts palstyl, float tone) {
    switch (palstyl) {
        case FIXED:
            return QColor::fromHslF(bhue, 0.32, tone);
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

QColor fixcol(QColor inp, float xtratone) {
    bool light;
    if (MG->theme == -1) {
        Qt::ColorScheme scheme = qApp->styleHints()->colorScheme();
        light = scheme != Qt::ColorScheme::Dark;
    } else { light = MG->theme == 1; }

    return QColor::fromHslF(inp.hueF(), 0.48, std::clamp(light? 0.4f+xtratone:0.8f-xtratone, 0.0f, 1.0f));
}

constexpr int diff = 10;
inline QColor colbang(QColor orig, bool light, int amnt = 1) {
    return QColor(orig.red() - diff*amnt, orig.green() - diff*amnt, orig.blue() - diff*amnt);
}

const QRegularExpression stylRe(R"(\$(!*)([a-zA-Z]+)\$?)");
void MainGame::genStyle(bool sig) {
    bool light;
    if (theme == -1) {
        Qt::ColorScheme scheme = qApp->styleHints()->colorScheme();
        light = scheme != Qt::ColorScheme::Dark; // Includes unknown
    } else { light = theme == 1; }
    float bhue = base.hueF();

    {const float bg = light? 0.5:0.75;
    const float fg = light? 1.0:0.2;
    const float bgcont = light? 0.8:0.3;
    const float fgcont = light? 0.1:0.9;
    styls[Cols::Primary] = getCol(bhue, PRIMARY, bg);
    styls[Cols::OnPrimary] = getCol(bhue, PRIMARY, fg);
    styls[Cols::PrimaryContainer] = getCol(bhue, PRIMARY, bgcont);
    styls[Cols::OnPrimaryContainer] = getCol(bhue, PRIMARY, fgcont);
    styls[Cols::PrimaryInverse] = getCol(bhue, PRIMARY, light? 0.9:0.3);
    styls[Cols::OnPrimaryInverse] = getCol(bhue, PRIMARY, light? 0.1:0.9);

    styls[Cols::Secondary] = getCol(bhue, SECONDARY, bg);
    styls[Cols::OnSecondary] = getCol(bhue, SECONDARY, fg);
    styls[Cols::SecondaryContainer] = getCol(bhue, SECONDARY, bgcont);
    styls[Cols::OnSecondaryContainer] = getCol(bhue, SECONDARY, fgcont);

    styls[Cols::Tertiary] = getCol(bhue, TERTIARY, bg);
    styls[Cols::OnTertiary] = getCol(bhue, TERTIARY, fg);
    styls[Cols::TertiaryContainer] = getCol(bhue, TERTIARY, bgcont);
    styls[Cols::OnTertiaryContainer] = getCol(bhue, TERTIARY, fgcont);

    float errh = 0.99f;
    styls[Cols::Error] = getCol(errh, PRIMARY, bg);
    styls[Cols::OnError] = getCol(errh, PRIMARY, fg);
    styls[Cols::ErrorContainer] = getCol(errh, PRIMARY, bgcont);
    styls[Cols::OnErrorContainer] = getCol(errh, PRIMARY, fgcont);
    }

    {float each = 1.0f/6.0f;
    constexpr float bg = 0.8;
    constexpr float on = 0.2;
    styls[Cols::RedFixed] = getCol(0, FIXED, bg);
    styls[Cols::OnRedFixed] = getCol(0, FIXED, on);
    styls[Cols::OrangeFixed] = getCol(each*0.5f, FIXED, bg);
    styls[Cols::OnOrangeFixed] = getCol(each*0.5f, FIXED, on);
    styls[Cols::YellowFixed] = getCol(each, FIXED, bg);
    styls[Cols::OnYellowFixed] = getCol(each, FIXED, on);
    styls[Cols::GreenFixed] = getCol(each*2, FIXED, bg);
    styls[Cols::OnGreenFixed] = getCol(each*2, FIXED, on);
    styls[Cols::BlueFixed] = getCol(each*3.5f, FIXED, bg);
    styls[Cols::OnBlueFixed] = getCol(each*3.5f, FIXED, on);
    styls[Cols::PurpleFixed] = getCol(each*5, FIXED, bg);
    styls[Cols::OnPurpleFixed] = getCol(each*5, FIXED, on);
    }

    styls[Cols::Outline] = getCol(bhue, NEUTRALVARIANT, light? 0.5:0.55);
    styls[Cols::OutlineVariant] = getCol(bhue, NEUTRALVARIANT, light? 0.7:0.45); // Lighter

    styls[Cols::Surface] = getCol(bhue, NEUTRAL, light? 0.98:0.06);
    styls[Cols::OnSurface] = getCol(bhue, NEUTRAL, light? 0.1:0.9);
    styls[Cols::OnSurfaceVariant] = getCol(bhue, NEUTRALVARIANT, light? 0.3:0.8);

    styls[Cols::SurfaceContainerLow] = getCol(bhue, NEUTRAL, light? 0.94:0.10);
    styls[Cols::SurfaceContainer] = getCol(bhue, NEUTRAL, light? 0.92:0.12);
    styls[Cols::SurfaceContainerHigh] = getCol(bhue, NEUTRAL, light? 0.90:0.17);
    styls[Cols::SurfaceContainerHighest] = getCol(bhue, NEUTRAL, light? 0.88:0.22);
    styls[Cols::SurfaceContainerHighestest] = getCol(bhue, NEUTRAL, light? 0.84:0.26);

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

        auto col = styls[Cols::fromName(m.captured(2))];
        col = colbang(col, light, m.captured(1).length());
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

    if (sig) emit themeChange();
}
