#pragma once
/// @file drawer.hpp
/// @brief Drawer widget — a side navigation panel that slides in from left or right.
///
/// Usage:
///   drawer({
///       .body     = main_content,
///       .child    = drawer_content,
///       .is_open  = show_drawer_,
///       .on_close = [this]() { setState([this]{ show_drawer_ = false; }); },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// DrawerConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Drawer widget.
struct DrawerConfig {
    WidgetPtr             body;                              ///< The main screen content behind the drawer.
    WidgetPtr             child;                             ///< Content rendered inside the drawer panel.
    bool                  is_open        = false;            ///< Whether the drawer is open.
    std::function<void()> on_close;                          ///< Called when the user dismisses the drawer.

    /// From which side the drawer slides in.
    enum class Side { Left, Right } side = Side::Left;

    float  width            = 280.0f;                        ///< Width of the drawer panel.
    Color  background_color = 0xFF1E293B;                    ///< Drawer panel background.
    Color  backdrop_color   = 0x99000000;                    ///< Semi-transparent backdrop color.
    bool   dismiss_on_backdrop = true;                       ///< Tap outside to close.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Drawer widget.
std::shared_ptr<Widget> drawer(DrawerConfig config);

/// Declarative syntax alias for Drawer
inline std::shared_ptr<Widget> Drawer(DrawerConfig config) {
    return drawer(std::move(config));
}

} // namespace vaura
