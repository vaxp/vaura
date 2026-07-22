#pragma once
/// @file search_bar.hpp
/// @brief SearchBar widget — integrated search field with clear button & suggestions.
///
/// Usage:
///   search_bar({
///       .value       = query_,
///       .on_changed  = [this](const std::string& q) { setState([this, q]{ query_ = q; }); },
///       .placeholder = "Search...",
///       .suggestions = {"Apple", "Banana", "Cherry"},
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
// SearchBarConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the SearchBar widget.
struct SearchBarConfig {
    std::string                             value;             ///< Current text value.
    std::function<void(const std::string&)> on_changed;        ///< Fired on every keystroke.
    std::function<void(const std::string&)> on_submitted;      ///< Fired on Enter/search press.
    std::function<void(const std::string&)> on_suggestion_tap; ///< Fired when a suggestion is tapped.
    std::vector<std::string>                suggestions;       ///< Autocomplete suggestions list.

    std::string  placeholder      = "Search...";
    Color        background_color = 0xFF1E293B;
    Color        text_color       = 0xFFF8FAFC;
    Color        placeholder_color = 0xFF64748B;
    Color        border_color     = 0xFF334155;
    Color        focus_color      = 0xFF0EA5E9;
    Color        icon_color       = 0xFF64748B;
    Color        clear_color      = 0xFF64748B;
    Color        suggestion_hover = 0xFF334155;
    float        border_radius    = 24.0f;   ///< Pill-shaped by default.
    float        width            = 320.0f;
    float        height           = 44.0f;
    float        font_size        = 14.0f;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a SearchBar widget.
std::shared_ptr<Widget> search_bar(SearchBarConfig config);

/// Declarative syntax alias for SearchBar
inline std::shared_ptr<Widget> SearchBar(SearchBarConfig config) {
    return search_bar(std::move(config));
}

} // namespace vaura
