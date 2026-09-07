#pragma once
#include <QString>
#include <QWidget>
#include <QDebug>
#include <cstdint>
#include <array>

namespace Cols {
    enum Colour : uint8_t {
        Primary,
        OnPrimary,
        PrimaryContainer,
        OnPrimaryContainer,
        PrimaryInverse,
        OnPrimaryInverse,

        Secondary,
        OnSecondary,
        SecondaryContainer,
        OnSecondaryContainer,

        Tertiary,
        OnTertiary,
        TertiaryContainer,
        OnTertiaryContainer,

        Outline,
        OutlineVariant,

        Surface,
        OnSurface,
        OnSurfaceVariant,

        SurfaceContainerLow,
        SurfaceContainer,
        SurfaceContainerHigh,
        SurfaceContainerHighest,

    Count};

    inline constexpr std::array<QStringView, Count> names = {
        u"Prim",
        u"OnPrim",
        u"PrimContnr",
        u"OnPrimContnr",
        u"PrimInv",
        u"OnPrimInv",

        u"Sec",
        u"OnSec",
        u"SecContnr",
        u"OnSecContnr",

        u"Tert",
        u"OnTert",
        u"TertContnr",
        u"OnTertContnr",

        u"Outl",
        u"OutlVar",

        u"Surf",
        u"OnSurf",
        u"OnSurfVar",

        u"SurfContnrLow",
        u"SurfContnr",
        u"SurfContnrHigh",
        u"SurfContnrHighest",
    };

    constexpr Colour fromName(QStringView name) {
        for (uint8_t i = 0; i < Count; ++i) {
            if (names[i] == name) {
                return static_cast<Colour>(i);
            }
        }
        qFatal() << "Could not find colour named" << name;
        return {};
    }
}

namespace ColGroups {
    struct ColGrp {
        QString nam;
        Cols::Colour fg;
        Cols::Colour bg;

        QString _getid() {
            static QString id = "";

            // Increment from right to left
            for (int i = id.length() - 1; i >= 0; --i) {
                if (id[i] == 'z') {
                    id[i] = 'A';
                    return id;
                } else if (id[i] == 'Z') {
                    // Continue, and carry over to next column
                    id[i] = 'a';
                } else {
                    id[i] = QChar(id[i].unicode() + 1);
                    return id;
                }
            }
            // Grow when all carry over
            id.prepend('a');
            return id;
        }

        ColGrp(Cols::Colour fg, Cols::Colour bg) : nam(_getid()), fg(fg), bg(bg) {}
    };

    enum Group : uint8_t {
        Primary,
        PrimaryContainer,
        PrimaryInverse,
        Secondary,
        SecondaryContainer,
        Tertiary,
        TertiaryContainer,
    };
    inline const std::unordered_map<Group, ColGrp> Groups = {
        { Primary, { Cols::OnPrimary, Cols::Primary } },
        { PrimaryContainer, { Cols::OnPrimaryContainer, Cols::PrimaryContainer } },
        { PrimaryInverse, { Cols::OnPrimaryInverse, Cols::PrimaryInverse } },
        { Secondary, { Cols::OnSecondary, Cols::Secondary } },
        { SecondaryContainer, { Cols::OnSecondaryContainer, Cols::SecondaryContainer } },
        { Tertiary, { Cols::OnTertiary, Cols::Tertiary } },
        { TertiaryContainer, { Cols::OnTertiaryContainer, Cols::TertiaryContainer } },
    };

    inline QString nam(Group g) { return Groups.at(g).nam; }
    inline void setGrp(QWidget* wid, Group g) { wid->setProperty("grp", Groups.at(g).nam); }
}
