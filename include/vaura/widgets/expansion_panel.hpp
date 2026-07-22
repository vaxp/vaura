#pragma once
/// @file expansion_panel.hpp
/// @brief ExpansionPanel — group of accordions with coordinated open/close state.
///
/// Usage:
///   expansion_panel({
///       .items = {
///           {"General",  "Basic settings",  content1, false},
///           {"Advanced", "Power users",     content2, false},
///       },
///       .allow_multiple_expanded = false, // only one open at a time
///       .on_expansion_changed = [this](int i, bool open) { /* ... */ },
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
// ExpansionPanelItem
// ════════════════════════════════════════════════════════════════

struct ExpansionPanelItem {
    std::string  title;
    std::string  subtitle    = "";
    WidgetPtr    content;
    bool         is_expanded = false;
};

// ════════════════════════════════════════════════════════════════
// ExpansionPanelConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the ExpansionPanel widget.
struct ExpansionPanelConfig {
    std::vector<ExpansionPanelItem>    items;
    std::function<void(int, bool)>     on_expansion_changed;   ///< index, is_expanded

    bool   allow_multiple_expanded = false; ///< false = accordion mode (one open at a time).

    Color  background_color  = 0xFF1E293B;
    Color  header_color      = 0xFF0F172A;
    Color  expanded_bg       = 0xFF162032;
    Color  border_color      = 0xFF334155;
    Color  title_color       = 0xFFF8FAFC;
    Color  subtitle_color    = 0xFF94A3B8;
    Color  icon_color        = 0xFF64748B;
    Color  active_icon_color = 0xFF0EA5E9;
    float  border_radius     = 12.0f;
    float  header_height     = 56.0f;
    float  title_font_size   = 15.0f;
    float  subtitle_font_size = 12.5f;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create an ExpansionPanel widget.
std::shared_ptr<Widget> expansion_panel(ExpansionPanelConfig config);

} // namespace vaura
