#include "game.hpp"
#include "base/taskload.hpp"
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
    float saturation = 0.0f; // Saturation (0-1)
    int hueShiftDeg = 0; // Added to the base hue (in degrees)
    float toneContrast = 1.0f; // Multiplied to tone, scaled from 0.5
};
using ThemeRow = std::array<RoleStyle, PALLETE_COUNT>;
inline const ThemeRow& themeRow(Themes::Theme thm) {
    static const ThemeRow tonal = {{
        { 0.32f }, // FIXED
        { 0.48f }, // PRIMARY
        { 0.16f }, // SECONDARY
        { 0.24f, 60 }, // TERTIARY
        { 0.04f }, // NEUTRAL
        { 0.08f }, // NEUTRALVARIANT
    }};
    static const ThemeRow muted = {{
        { 0.18f }, // FIXED
        { 0.28f }, // PRIMARY
        { 0.10f }, // SECONDARY
        { 0.14f, 45 }, // TERTIARY
        { 0.03f }, // NEUTRAL
        { 0.05f }, // NEUTRALVARIANT
    }};
    static const ThemeRow vivid = {{
        { 0.35f, 0, 0.9f }, // FIXED
        { 0.78f, 0, 0.9f }, // PRIMARY
        { 0.60f, 0, 0.9f }, // SECONDARY
        { 0.75f, 25, 0.9f }, // TERTIARY
        { 0.09f, 0, 0.9f }, // NEUTRAL
        { 0.18f, 0, 0.9f }, // NEUTRALVARIANT
    }};
    static const ThemeRow grayscale = {{
        { 0.01f, 0, 1.2f }, // FIXED
        { 0.01f, 0, 1.2f }, // PRIMARY
        { 0.0f, 0, 1.2f }, // SECONDARY
        { 0.0f, 0, 1.2f }, // TERTIARY
        { 0.0f, 0, 1.2f }, // NEUTRAL
        { 0.0f, 0, 1.2f }, // NEUTRALVARIANT
    }};
    static const ThemeRow expressive = {{
        { 0.35f }, // FIXED
        { 0.60f }, // PRIMARY
        { 0.55f, -50 }, // SECONDARY
        { 0.55f, -160 }, // TERTIARY
        { 0.06f }, // NEUTRAL
        { 0.10f }, // NEUTRALVARIANT
    }};
    static const ThemeRow fruitsalad = {{
        { 0.30f }, // FIXED
        { 0.55f }, // PRIMARY
        { 0.25f, 150 }, // SECONDARY
        { 0.30f, 260 }, // TERTIARY
        { 0.05f }, // NEUTRAL
        { 0.10f }, // NEUTRALVARIANT
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

QColor getCol(float bhue, PalleteOpts palstyl, float tone, Themes::Theme theme) {
    const auto& styl = themeRow(theme)[static_cast<std::size_t>(palstyl)];

    float hue = std::fmod(bhue + styl.hueShiftDeg / 360.0f, 1.0f);
    if (hue < 0.0f) hue += 1.0f; // fmod can be negative

    float t = std::clamp(0.5f + (tone - 0.5f) * styl.toneContrast, 0.0f, 1.0f);

    return QColor::fromHslF(hue, styl.saturation, t);
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
void MainGame::genStyle(bool init) {
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
    styls[Cols::Primary] = getCol(bhue, PRIMARY, bg, colthm);
    styls[Cols::OnPrimary] = getCol(bhue, PRIMARY, fg, colthm);
    styls[Cols::PrimaryContainer] = getCol(bhue, PRIMARY, bgcont, colthm);
    styls[Cols::OnPrimaryContainer] = getCol(bhue, PRIMARY, fgcont, colthm);
    styls[Cols::PrimaryInverse] = getCol(bhue, PRIMARY, light? 0.9:0.3, colthm);
    styls[Cols::OnPrimaryInverse] = getCol(bhue, PRIMARY, light? 0.1:0.9, colthm);

    styls[Cols::Secondary] = getCol(bhue, SECONDARY, bg, colthm);
    styls[Cols::OnSecondary] = getCol(bhue, SECONDARY, fg, colthm);
    styls[Cols::SecondaryContainer] = getCol(bhue, SECONDARY, bgcont, colthm);
    styls[Cols::OnSecondaryContainer] = getCol(bhue, SECONDARY, fgcont, colthm);

    styls[Cols::Tertiary] = getCol(bhue, TERTIARY, bg, colthm);
    styls[Cols::OnTertiary] = getCol(bhue, TERTIARY, fg, colthm);
    styls[Cols::TertiaryContainer] = getCol(bhue, TERTIARY, bgcont, colthm);
    styls[Cols::OnTertiaryContainer] = getCol(bhue, TERTIARY, fgcont, colthm);

    float errh = 0.99f;
    styls[Cols::Error] = getCol(errh, PRIMARY, bg, colthm);
    styls[Cols::OnError] = getCol(errh, PRIMARY, fg, colthm);
    styls[Cols::ErrorContainer] = getCol(errh, PRIMARY, bgcont, colthm);
    styls[Cols::OnErrorContainer] = getCol(errh, PRIMARY, fgcont, colthm);
    }

    {float each = 1.0f/6.0f;
    constexpr float bg = 0.8;
    constexpr float on = 0.2;
    styls[Cols::RedFixed] = getCol(0, FIXED, bg, colthm);
    styls[Cols::OnRedFixed] = getCol(0, FIXED, on, colthm);
    styls[Cols::OrangeFixed] = getCol(each*0.5f, FIXED, bg, colthm);
    styls[Cols::OnOrangeFixed] = getCol(each*0.5f, FIXED, on, colthm);
    styls[Cols::YellowFixed] = getCol(each, FIXED, bg, colthm);
    styls[Cols::OnYellowFixed] = getCol(each, FIXED, on, colthm);
    styls[Cols::GreenFixed] = getCol(each*2, FIXED, bg, colthm);
    styls[Cols::OnGreenFixed] = getCol(each*2, FIXED, on, colthm);
    styls[Cols::BlueFixed] = getCol(each*3.5f, FIXED, bg, colthm);
    styls[Cols::OnBlueFixed] = getCol(each*3.5f, FIXED, on, colthm);
    styls[Cols::PurpleFixed] = getCol(each*5, FIXED, bg, colthm);
    styls[Cols::OnPurpleFixed] = getCol(each*5, FIXED, on, colthm);
    }

    styls[Cols::Outline] = getCol(bhue, NEUTRALVARIANT, light? 0.5:0.55, colthm);
    styls[Cols::OutlineVariant] = getCol(bhue, NEUTRALVARIANT, light? 0.7:0.45, colthm); // Lighter

    styls[Cols::Surface] = getCol(bhue, NEUTRAL, light? 0.98:0.06, colthm);
    styls[Cols::OnSurface] = getCol(bhue, NEUTRAL, light? 0.1:0.9, colthm);
    styls[Cols::OnSurfaceVariant] = getCol(bhue, NEUTRALVARIANT, light? 0.3:0.8, colthm);

    styls[Cols::SurfaceContainerLow] = getCol(bhue, NEUTRAL, light? 0.94:0.10, colthm);
    styls[Cols::SurfaceContainer] = getCol(bhue, NEUTRAL, light? 0.92:0.12, colthm);
    styls[Cols::SurfaceContainerHigh] = getCol(bhue, NEUTRAL, light? 0.90:0.17, colthm);
    styls[Cols::SurfaceContainerHighest] = getCol(bhue, NEUTRAL, light? 0.88:0.22, colthm);
    styls[Cols::SurfaceContainerHighestest] = getCol(bhue, NEUTRAL, light? 0.84:0.26, colthm);

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
