#pragma once
/// @file tabs.hpp
/// @brief Tabs widget — organize content into separate views.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <vector>

namespace vaura {

struct TabsConfig {
    std::vector<std::pair<std::string, WidgetPtr>> tabs; ///< List of Tab Title and Content pairs.
    int initial_index = 0;                               ///< Which tab is selected by default.
    
    Color tab_bar_color = 0xFF0F172A;                    ///< Background of the tab bar.
    Color active_tab_color = 0xFF1E293B;                 ///< Background of the active tab.
    Color inactive_tab_color = 0x00000000;               ///< Background of inactive tabs.
    Color active_text_color = 0xFF38BDF8;                ///< Text color for active tab.
    Color inactive_text_color = 0xFF94A3B8;              ///< Text color for inactive tabs.
};

/// Create a Tabs widget.
std::shared_ptr<Widget> tabs(TabsConfig config);

/// Declarative syntax alias for Tabs
inline std::shared_ptr<Widget> Tabs(TabsConfig config) {
    return tabs(std::move(config));
}

} // namespace vaura
