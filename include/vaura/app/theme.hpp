#pragma once
/// @file theme.hpp
/// @brief Theme system — provides consistent styling across the app.
///
/// ThemeData holds all design tokens (colors, text styles, radii).
/// Theme is an InheritedWidget that propagates ThemeData down the tree.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/widgets/text.hpp"
#include <string>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// ColorScheme — material-style color palette
// ════════════════════════════════════════════════════════════════

struct ColorScheme {
    Color primary        = 0xFF6200EE;
    Color onPrimary      = 0xFFFFFFFF;
    Color secondary      = 0xFF03DAC6;
    Color onSecondary    = 0xFF000000;
    Color background     = 0xFFF5F5F5;
    Color onBackground   = 0xFF000000;
    Color surface        = 0xFFFFFFFF;
    Color onSurface      = 0xFF000000;
    Color error          = 0xFFB00020;
    Color onError        = 0xFFFFFFFF;
    Color outline        = 0xFFE0E0E0;
    Color shadow         = 0x40000000;

    static ColorScheme light() {
        return ColorScheme{};
    }

    static ColorScheme dark() {
        return ColorScheme{
            .primary      = 0xFFBB86FC,
            .onPrimary    = 0xFF000000,
            .secondary    = 0xFF03DAC6,
            .onSecondary  = 0xFF000000,
            .background   = 0xFF121212,
            .onBackground = 0xFFFFFFFF,
            .surface      = 0xFF1E1E1E,
            .onSurface    = 0xFFFFFFFF,
            .error        = 0xFFCF6679,
            .onError      = 0xFF000000,
            .outline      = 0xFF333333,
            .shadow       = 0x60000000,
        };
    }
};

// ════════════════════════════════════════════════════════════════
// TextTheme — text style presets
// ════════════════════════════════════════════════════════════════

struct TextTheme {
    TextConfig displayLarge;
    TextConfig displayMedium;
    TextConfig displaySmall;
    TextConfig headlineLarge;
    TextConfig headlineMedium;
    TextConfig headlineSmall;
    TextConfig titleLarge;
    TextConfig titleMedium;
    TextConfig titleSmall;
    TextConfig bodyLarge;
    TextConfig bodyMedium;
    TextConfig bodySmall;
    TextConfig labelLarge;
    TextConfig labelMedium;
    TextConfig labelSmall;

    static TextTheme defaults(Color onBackground) {
        return TextTheme{
            .displayLarge  = { .font_size = 57, .color = onBackground, .weight = TextStyle::Regular },
            .displayMedium = { .font_size = 45, .color = onBackground, .weight = TextStyle::Regular },
            .displaySmall  = { .font_size = 36, .color = onBackground, .weight = TextStyle::Regular },
            .headlineLarge = { .font_size = 32, .color = onBackground, .weight = TextStyle::Bold },
            .headlineMedium= { .font_size = 28, .color = onBackground, .weight = TextStyle::SemiBold },
            .headlineSmall = { .font_size = 24, .color = onBackground, .weight = TextStyle::SemiBold },
            .titleLarge    = { .font_size = 22, .color = onBackground, .weight = TextStyle::Medium },
            .titleMedium   = { .font_size = 16, .color = onBackground, .weight = TextStyle::Medium },
            .titleSmall    = { .font_size = 14, .color = onBackground, .weight = TextStyle::Medium },
            .bodyLarge     = { .font_size = 16, .color = onBackground, .weight = TextStyle::Regular },
            .bodyMedium    = { .font_size = 14, .color = onBackground, .weight = TextStyle::Regular },
            .bodySmall     = { .font_size = 12, .color = onBackground, .weight = TextStyle::Regular },
            .labelLarge    = { .font_size = 14, .color = onBackground, .weight = TextStyle::Medium },
            .labelMedium   = { .font_size = 12, .color = onBackground, .weight = TextStyle::Medium },
            .labelSmall    = { .font_size = 11, .color = onBackground, .weight = TextStyle::Medium },
        };
    }
};

// ════════════════════════════════════════════════════════════════
// ThemeData — complete theme definition
// ════════════════════════════════════════════════════════════════

struct ThemeData {
    ColorScheme colorScheme;
    TextTheme   textTheme;

    float defaultBorderRadius = 8.0f;
    EdgeInsets defaultPadding  = EdgeInsets::all(16.0f);
    float elevation            = 4.0f;

    static ThemeData light() {
        auto cs = ColorScheme::light();
        return ThemeData{
            .colorScheme = cs,
            .textTheme   = TextTheme::defaults(cs.onBackground),
        };
    }

    static ThemeData dark() {
        auto cs = ColorScheme::dark();
        return ThemeData{
            .colorScheme = cs,
            .textTheme   = TextTheme::defaults(cs.onBackground),
        };
    }
};

} // namespace vaura
