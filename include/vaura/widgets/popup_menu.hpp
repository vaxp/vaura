#pragma once
/// @file popup_menu.hpp
/// @brief PopupMenu widget — a floating menu anchored to a button or element.
///
/// Unlike ContextMenu (right-click triggered), PopupMenu opens on a normal tap
/// of the anchor widget and positions itself relative to the anchor.
///
/// Usage:
///   popup_menu({
///       .anchor = icon_button,
///       .items  = {
///           {"Settings", [](){ /* ... */ }},
///           {"Help",     [](){ /* ... */ }},
///           {.is_separator = true},
///           {"Logout", [](){ /* ... */ }, nullptr, true, true},
///       },
///       .placement = PopupMenuConfig::Placement::BelowLeft,
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/widgets/context_menu.hpp"  // re-uses ContextMenuItem
#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <vector>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// PopupMenuConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the PopupMenu widget.
struct PopupMenuConfig {
    WidgetPtr                    anchor;             ///< Widget that opens the menu on tap.
    std::vector<ContextMenuItem> items;              ///< Menu items (shared with ContextMenuItem).

    /// Where the menu panel appears relative to the anchor.
    enum class Placement {
        BelowLeft,   ///< Panel top-left aligns with anchor bottom-left  (default).
        BelowRight,  ///< Panel top-right aligns with anchor bottom-right.
        AboveLeft,   ///< Panel bottom-left aligns with anchor top-left.
        AboveRight,  ///< Panel bottom-right aligns with anchor top-right.
    } placement = Placement::BelowLeft;

    Color  background_color  = 0xFF1E293B;
    Color  text_color        = 0xFFF8FAFC;
    Color  disabled_color    = 0xFF475569;
    Color  danger_color      = 0xFFEF4444;
    Color  hover_color       = 0xFF334155;
    Color  separator_color   = 0xFF334155;
    Color  border_color      = 0xFF475569;
    float  border_radius     = 8.0f;
    float  min_width         = 160.0f;
    float  item_height       = 38.0f;
    float  font_size         = 13.5f;
    float  anchor_height     = 40.0f;  ///< Height of the anchor for offset calculation.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a PopupMenu widget.
std::shared_ptr<Widget> popup_menu(PopupMenuConfig config);

/// Declarative syntax alias for PopupMenu
inline std::shared_ptr<Widget> PopupMenu(PopupMenuConfig config) {
    return popup_menu(std::move(config));
}

} // namespace vaura
