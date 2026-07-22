#pragma once
/// @file badge.hpp
/// @brief A badge widget to display notifications or counts over another widget.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <memory>

namespace vaura {

struct BadgeConfig {
    std::string text;                     ///< Text to display in the badge (e.g., "3", "99+")
    Color background_color = 0xFFEF4444;  ///< Red background
    Color text_color = 0xFFFFFFFF;        ///< White text
    float font_size = 10.0f;              ///< Size of the text
    float padding = 4.0f;                 ///< Internal padding for the badge
    Point offset = {0.0f, 0.0f};          ///< Offset to adjust the badge position relative to top-right
    
    WidgetPtr child;                      ///< The widget being badged
};

/// Create a Badge widget.
std::shared_ptr<Widget> badge(BadgeConfig config);

/// Declarative syntax alias for Badge
inline std::shared_ptr<Widget> Badge(BadgeConfig config) {
    return badge(std::move(config));
}

} // namespace vaura
