#include "game.hpp"
#include "base/taskload.hpp"
#include "extra/hsluv.h"
#include <QFile>
#include <QStyleHints>
#include <QRandomGenerator>
#include <QRegularExpression>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/qnativeinterface.h>
#endif

void MainGame::setupStyle() {
    cols = {
        {179, 59, 21}, // Red
        {245, 226, 158}, // Yellow
        {178, 234, 211}, // Green
        {137, 232, 232}, // Aqua
        {118, 156, 223}, // Blue
        {150, 123, 182}, // Purple
        {237, 189, 213}, // Pink
    };
    if (!base.isValid()) {
        base = cols[QRandomGenerator::global()->bounded(int(cols.size()))];
        QTimer::singleShot(0, [](){ saveTasks(); }); // So it doesn't change each time
    }
    QObject::connect(qApp->styleHints(), &QStyleHints::colorSchemeChanged,
              qApp, [this](Qt::ColorScheme scheme) {
        if (theme == -1) genStyle();
    });
    genStyle(true);
}


enum PalleteOpts {
    FIXED,
    PRIMARY,
    SECONDARY,
    TERTIARY,
    NEUTRAL,
    NEUTRALVARIANT,

PALLETE_COUNT};

struct RoleStyle {
    double saturation = 0; // Saturation (0-100)
    double hueShiftDeg = 0; // Added to the base hue (in degrees)
    double toneContrast = 1.0f; // Multiplied to tone, scaled from 0.5
};
using ThemeRow = std::array<RoleStyle, PALLETE_COUNT>;
inline const ThemeRow& themeRow(Themes::Theme thm) {
    static const ThemeRow tonal = {{
        { 32 }, // FIXED
        { 48 }, // PRIMARY
        { 16 }, // SECONDARY
        { 24, 60 }, // TERTIARY
        { 4 }, // NEUTRAL
        { 8 }, // NEUTRALVARIANT
    }};
    static const ThemeRow muted = {{
        { 18 }, // FIXED
        { 28 }, // PRIMARY
        { 10 }, // SECONDARY
        { 14, 45 }, // TERTIARY
        { 3 }, // NEUTRAL
        { 5 }, // NEUTRALVARIANT
    }};
    static const ThemeRow vivid = {{
        { 35, 0, 0.9 }, // FIXED
        { 78, 0, 0.9 }, // PRIMARY
        { 60, 0, 0.9 }, // SECONDARY
        { 75, 25, 0.9 }, // TERTIARY
        { 9, 0, 0.9 }, // NEUTRAL
        { 18, 0, 0.9 }, // NEUTRALVARIANT
    }};
    static const ThemeRow grayscale = {{
        { 1, 0, 1.2 }, // FIXED
        { 1, 0, 1.2 }, // PRIMARY
        { 0, 0, 1.2 }, // SECONDARY
        { 0, 0, 1.2 }, // TERTIARY
        { 0, 0, 1.2 }, // NEUTRAL
        { 0, 0, 1.2 }, // NEUTRALVARIANT
    }};
    static const ThemeRow expressive = {{
        { 35 }, // FIXED
        { 60 }, // PRIMARY
        { 55, -50 }, // SECONDARY
        { 55, -160 }, // TERTIARY
        { 6 }, // NEUTRAL
        { 10 }, // NEUTRALVARIANT
    }};
    static const ThemeRow fruitsalad = {{
        { 30 }, // FIXED
        { 55 }, // PRIMARY
        { 25, 150 }, // SECONDARY
        { 30, 260 }, // TERTIARY
        { 5 }, // NEUTRAL
        { 10 }, // NEUTRALVARIANT
    }};

    switch (thm) {
        case Themes::Tonal: return tonal;
        case Themes::Muted: return muted;
        case Themes::Vivid: return vivid;
        case Themes::Greyscale: return grayscale;
        case Themes::Expressive: return expressive;
        case Themes::FruitSalad: return fruitsalad;
        default: return tonal;
    }
}

QColor getCol(double bhue, PalleteOpts palstyl, double tone, Themes::Theme theme) {
    const auto& styl = themeRow(theme)[static_cast<std::size_t>(palstyl)];

    double hue = std::fmod(bhue + styl.hueShiftDeg, 360.);
    if (hue < 0.) hue += 360.;

    double t = std::clamp(50. + (tone - 50.) * styl.toneContrast, 0., 100.);
    double r, g, b;
    hsluv2rgb(hue, styl.saturation, t, &r, &g, &b);
    return QColor::fromRgbF(r, g, b);
}

constexpr int diff = 10;
inline QColor colbang(QColor orig, bool light, int amnt = 1) {
    return QColor(orig.red() - diff*amnt, orig.green() - diff*amnt, orig.blue() - diff*amnt);
}

double getHue(QColor col) {
    double h, s, l;
    rgb2hsluv(col.redF(), col.greenF(), col.blueF(), &h, &s, &l);
    return h;
}

const QRegularExpression stylRe(R"(\$(!*)([a-zA-Z]+)\$?)");
void MainGame::genStyle(bool init) {
    bool light;
    if (theme == -1) {
        Qt::ColorScheme scheme = qApp->styleHints()->colorScheme();
        light = scheme != Qt::ColorScheme::Dark; // Includes unknown
    } else { light = theme == 1; }
    double bhue = getHue(base);

    {const double bg = light? 50:75;
    const double fg = light? 100:20;
    const double bgcont = light? 80:30;
    const double fgcont = light? 10:90;
    styls[Cols::Primary] = getCol(bhue, PRIMARY, bg, colthm);
    styls[Cols::OnPrimary] = getCol(bhue, PRIMARY, fg, colthm);
    styls[Cols::PrimaryContainer] = getCol(bhue, PRIMARY, bgcont, colthm);
    styls[Cols::OnPrimaryContainer] = getCol(bhue, PRIMARY, fgcont, colthm);
    styls[Cols::PrimaryInverse] = getCol(bhue, PRIMARY, light? 90:30, colthm);
    styls[Cols::OnPrimaryInverse] = getCol(bhue, PRIMARY, light? 10:90, colthm);

    styls[Cols::Secondary] = getCol(bhue, SECONDARY, bg, colthm);
    styls[Cols::OnSecondary] = getCol(bhue, SECONDARY, fg, colthm);
    styls[Cols::SecondaryContainer] = getCol(bhue, SECONDARY, bgcont, colthm);
    styls[Cols::OnSecondaryContainer] = getCol(bhue, SECONDARY, fgcont, colthm);

    styls[Cols::Tertiary] = getCol(bhue, TERTIARY, bg, colthm);
    styls[Cols::OnTertiary] = getCol(bhue, TERTIARY, fg, colthm);
    styls[Cols::TertiaryContainer] = getCol(bhue, TERTIARY, bgcont, colthm);
    styls[Cols::OnTertiaryContainer] = getCol(bhue, TERTIARY, fgcont, colthm);

    double errh = 355;
    styls[Cols::Error] = getCol(errh, PRIMARY, bg, colthm);
    styls[Cols::OnError] = getCol(errh, PRIMARY, fg, colthm);
    styls[Cols::ErrorContainer] = getCol(errh, PRIMARY, bgcont, colthm);
    styls[Cols::OnErrorContainer] = getCol(errh, PRIMARY, fgcont, colthm);
    }

    {double each = 30;
    const double bg = 80;
    const double on = 20;
    styls[Cols::RedFixed] = getCol(0, FIXED, bg, colthm);
    styls[Cols::OnRedFixed] = getCol(0, FIXED, on, colthm);
    styls[Cols::OrangeFixed] = getCol(each*0.5, FIXED, bg, colthm);
    styls[Cols::OnOrangeFixed] = getCol(each*0.5, FIXED, on, colthm);
    styls[Cols::YellowFixed] = getCol(each, FIXED, bg, colthm);
    styls[Cols::OnYellowFixed] = getCol(each, FIXED, on, colthm);
    styls[Cols::GreenFixed] = getCol(each*2, FIXED, bg, colthm);
    styls[Cols::OnGreenFixed] = getCol(each*2, FIXED, on, colthm);
    styls[Cols::BlueFixed] = getCol(each*3.5, FIXED, bg, colthm);
    styls[Cols::OnBlueFixed] = getCol(each*3.5, FIXED, on, colthm);
    styls[Cols::PurpleFixed] = getCol(each*5, FIXED, bg, colthm);
    styls[Cols::OnPurpleFixed] = getCol(each*5, FIXED, on, colthm);
    }

    styls[Cols::Outline] = getCol(bhue, NEUTRALVARIANT, light? 50:55, colthm);
    styls[Cols::OutlineVariant] = getCol(bhue, NEUTRALVARIANT, light? 70:45, colthm); // Lighter

    styls[Cols::Surface] = getCol(bhue, NEUTRAL, light? 98:6, colthm);
    styls[Cols::OnSurface] = getCol(bhue, NEUTRAL, light? 10:90, colthm);
    styls[Cols::OnSurfaceVariant] = getCol(bhue, NEUTRALVARIANT, light? 30:80, colthm);

    styls[Cols::SurfaceContainerLow] = getCol(bhue, NEUTRAL, light? 94:10, colthm);
    styls[Cols::SurfaceContainer] = getCol(bhue, NEUTRAL, light? 92:12, colthm);
    styls[Cols::SurfaceContainerHigh] = getCol(bhue, NEUTRAL, light? 90:17, colthm);
    styls[Cols::SurfaceContainerHighest] = getCol(bhue, NEUTRAL, light? 88:22, colthm);
    styls[Cols::SurfaceContainerHighestest] = getCol(bhue, NEUTRAL, light? 84:26, colthm);

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

    // It will be annoyed if you try to run this immediately on startup, so we queue it
    if (init) {
        QTimer::singleShot(0, updFn);
    } else { updFn(); }
#endif

    if (!init) emit themeChange();
}
