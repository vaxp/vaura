#pragma once
/// @file tooltip.hpp
/// @brief Tooltip widget — shows a message on hover.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>

namespace vaura {

struct TooltipConfig {
    std::string message;                   ///< The text to display
    float vertical_offset = 24.0f;         ///< Offset below the child
    float wait_duration = 0.0f;            ///< (Future) Hover delay before showing
    Color background_color = 0xFF1E293B;   ///< Tooltip background color
    Color text_color = 0xFFFFFFFF;         ///< Tooltip text color
    float font_size = 14.0f;               ///< Tooltip text size
    WidgetPtr child;                       ///< The widget to wrap
};

/// @brief Creates a Tooltip widget that wraps the given child.
std::shared_ptr<Widget> tooltip(TooltipConfig config);

} // namespace vaura
