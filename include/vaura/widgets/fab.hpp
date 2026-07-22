#pragma once
/// @file fab.hpp
/// @brief FloatingActionButton (FAB) widget — a prominent circular action button.
///
/// Supports three sizes:
///   - Regular (56×56)  — default
///   - Mini    (40×40)
///   - Extended (height=56, width=auto) — shows icon + label side by side
///
/// Usage:
///   fab({
///       .icon       = icon(Icons::add),
///       .on_pressed = [](){ /* add item */ },
///   })
///
///   fab({                           // Extended FAB
///       .icon       = icon(Icons::edit),
///       .label      = "New Post",
///       .on_pressed = [](){ /* ... */ },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// FABConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the FloatingActionButton widget.
struct FABConfig {
    WidgetPtr             icon;                              ///< Icon widget displayed in the button.
    std::string           label        = "";                 ///< If non-empty → Extended FAB with text.
    std::function<void()> on_pressed;                        ///< Tap callback.

    /// Size preset.
    enum class Size { Regular, Mini } size = Size::Regular;

    Color  background_color  = 0xFF0EA5E9;    ///< Button background.
    Color  hover_color       = 0xFF38BDF8;    ///< Hover background.
    Color  pressed_color     = 0xFF0284C7;    ///< Pressed background.
    Color  icon_color        = 0xFFFFFFFF;    ///< Icon tint color.
    Color  label_color       = 0xFFFFFFFF;    ///< Label text color (extended mode).
    float  label_font_size   = 14.0f;
    float  elevation         = 6.0f;          ///< Shadow depth.
    bool   enabled           = true;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a FloatingActionButton widget.
std::shared_ptr<Widget> fab(FABConfig config);

/// Declarative syntax alias for FAB
inline std::shared_ptr<Widget> FAB(FABConfig config) {
    return fab(std::move(config));
}

} // namespace vaura
