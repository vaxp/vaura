#pragma once
/// @file context_menu.hpp
/// @brief ContextMenu widget — appears on right-click with a list of actions.
///
/// Usage:
///   context_menu({
///       .child = my_widget,
///       .items = {
///           {"Copy",  [](){ /* ... */ }, copy_icon},
///           {"Paste", [](){ /* ... */ }},
///           {.is_separator = true},
///           {"Delete", [](){ /* ... */ }, nullptr, true, true}, // danger
///       },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <vector>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// ContextMenuItem
// ════════════════════════════════════════════════════════════════

/// @brief A single item in a ContextMenu or PopupMenu.
struct ContextMenuItem {
    std::string            label;                  ///< Display text (ignored for separators).
    std::function<void()>  on_tap;                 ///< Action callback.
    WidgetPtr              icon       = nullptr;   ///< Optional leading icon.
    bool                   is_separator = false;   ///< Renders as a horizontal divider.
    bool                   enabled    = true;      ///< Greyed-out if false.
    bool                   is_danger  = false;     ///< Renders label in red (destructive actions).
};

// ════════════════════════════════════════════════════════════════
// ContextMenuConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the ContextMenu widget.
struct ContextMenuConfig {
    WidgetPtr                    child;              ///< The wrapped widget that triggers the menu.
    std::vector<ContextMenuItem> items;              ///< Menu items in order.

    Color  background_color  = 0xFF1E293B;           ///< Menu panel background.
    Color  text_color        = 0xFFF8FAFC;           ///< Normal item text color.
    Color  disabled_color    = 0xFF475569;           ///< Greyed-out item color.
    Color  danger_color      = 0xFFEF4444;           ///< Destructive item text color.
    Color  hover_color       = 0xFF334155;           ///< Item hover background.
    Color  separator_color   = 0xFF334155;           ///< Horizontal divider color.
    Color  border_color      = 0xFF475569;           ///< Panel border color.
    float  border_radius     = 8.0f;                 ///< Panel corner radius.
    float  min_width         = 160.0f;              ///< Minimum panel width.
    float  item_height       = 38.0f;               ///< Height of each item row.
    float  font_size         = 13.5f;               ///< Item label font size.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a ContextMenu widget.
std::shared_ptr<Widget> context_menu(ContextMenuConfig config);

/// Declarative syntax alias for ContextMenu
inline std::shared_ptr<Widget> ContextMenu(ContextMenuConfig config) {
    return context_menu(std::move(config));
}

} // namespace vaura
