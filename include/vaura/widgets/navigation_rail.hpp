#pragma once
/// @file navigation_rail.hpp
/// @brief NavigationRail widget — vertical navigation menu for desktop/tablet.
///
/// Usage:
///   navigation_rail({
///       .items = {
///           {home_icon, "Home"},
///           {search_icon, "Search"},
///           {settings_icon, "Settings"},
///       },
///       .selected_index = 0,
///       .on_changed = [](int i) { /* switch page */ },
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
// NavigationRailItem
// ════════════════════════════════════════════════════════════════

/// @brief A single destination in the NavigationRail.
struct NavigationRailItem {
    WidgetPtr   icon;           ///< Icon widget (required).
    std::string label;          ///< Text label shown in extended mode.
};

// ════════════════════════════════════════════════════════════════
// NavigationRailConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the NavigationRail widget.
struct NavigationRailConfig {
    std::vector<NavigationRailItem> items;                    ///< Destinations.
    int                             selected_index = 0;       ///< Active destination index.
    std::function<void(int)>        on_changed;               ///< Fired when selection changes.
    WidgetPtr                       header = nullptr;         ///< Optional widget above items (e.g., logo).
    WidgetPtr                       footer = nullptr;         ///< Optional widget below items.

    bool   extended         = false;     ///< Show label beside icon (wide mode).
    float  collapsed_width  = 72.0f;     ///< Width in collapsed mode.
    float  extended_width   = 200.0f;    ///< Width in extended mode.

    Color  background_color = 0xFF0F172A;  ///< Rail background.
    Color  indicator_color  = 0xFF1E3A5F;  ///< Active indicator background.
    Color  active_icon_color   = 0xFF38BDF8;
    Color  inactive_icon_color = 0xFF64748B;
    Color  active_label_color  = 0xFF38BDF8;
    Color  inactive_label_color = 0xFF64748B;
    float  item_height      = 56.0f;       ///< Height of each destination tile.
    float  indicator_radius = 14.0f;       ///< Border radius of the active indicator.
    float  icon_size        = 22.0f;
    float  label_font_size  = 13.0f;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a NavigationRail widget.
std::shared_ptr<Widget> navigation_rail(NavigationRailConfig config);

/// Declarative syntax alias for NavigationRail
inline std::shared_ptr<Widget> NavigationRail(NavigationRailConfig config) {
    return navigation_rail(std::move(config));
}

} // namespace vaura
