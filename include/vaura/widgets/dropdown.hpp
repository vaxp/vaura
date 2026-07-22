#pragma once
/// @file dropdown.hpp
/// @brief Dropdown widget — Combobox/Select menu for picking one item from a list.
///
/// Usage:
///   dropdown({
///       .items = {{"Option A", "a"}, {"Option B", "b"}},
///       .selected_value = "a",
///       .on_changed = [](const std::string& v) { /* ... */ },
///       .width = 200.0f,
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
// DropdownItem
// ════════════════════════════════════════════════════════════════

/// @brief A single selectable item inside a Dropdown list.
struct DropdownItem {
    std::string label;            ///< Display text.
    std::string value;            ///< Unique value returned via on_changed.
    WidgetPtr   icon = nullptr;   ///< Optional leading icon widget.
};

// ════════════════════════════════════════════════════════════════
// DropdownConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Dropdown widget.
struct DropdownConfig {
    std::vector<DropdownItem>               items;                        ///< List of selectable items.
    std::string                             selected_value;               ///< Currently selected value.
    std::function<void(const std::string&)> on_changed;                   ///< Fired when selection changes.

    std::string  placeholder       = "Select...";    ///< Shown when nothing is selected.
    Color        background_color  = 0xFF1E293B;     ///< Field & list background.
    Color        text_color        = 0xFFF8FAFC;     ///< Text color.
    Color        placeholder_color = 0xFF64748B;     ///< Placeholder text color.
    Color        border_color      = 0xFF334155;     ///< Border of the closed field.
    Color        focus_color       = 0xFF0EA5E9;     ///< Border when open.
    Color        selected_color    = 0xFF0D2137;     ///< Highlight of selected item.
    Color        hover_color       = 0xFF334155;     ///< Hover highlight.
    float        border_radius     = 8.0f;           ///< Corner radius.
    float        width             = 200.0f;         ///< Total width of the control.
    float        height            = 44.0f;          ///< Height of the closed field.
    float        max_list_height   = 220.0f;         ///< Max height of open list (scrollable if needed).
    float        font_size         = 14.0f;          ///< Text size.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Dropdown widget.
std::shared_ptr<Widget> dropdown(DropdownConfig config);

/// Declarative syntax alias for Dropdown
inline std::shared_ptr<Widget> Dropdown(DropdownConfig config) {
    return dropdown(std::move(config));
}

} // namespace vaura
