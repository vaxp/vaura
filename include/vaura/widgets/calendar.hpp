#pragma once
/// @file calendar.hpp
/// @brief Calendar widget — full monthly calendar view.
///
/// Usage:
///   calendar({
///       .year  = 2025, .month = 7,
///       .selected_day  = 15,
///       .on_day_selected = [this](int y, int m, int d) {
///           setState([this, y, m, d]{ selected_ = {y, m, d}; });
///       },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>
#include <vector>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// CalendarConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Calendar widget.
struct CalendarConfig {
    int  year              = 2025;   ///< Currently displayed year.
    int  month             = 1;      ///< Currently displayed month (1–12).
    int  selected_day      = -1;     ///< Selected day (-1 = none).

    /// Fired when the user taps a day. Arguments: year, month, day.
    std::function<void(int, int, int)> on_day_selected;

    /// Fired when the user navigates to a different month. Arguments: year, month.
    std::function<void(int, int)> on_month_changed;

    Color  background_color = 0xFF1E293B;   ///< Overall background.
    Color  header_color     = 0xFF0F172A;   ///< Month/year header background.
    Color  header_text_color = 0xFFF8FAFC;
    Color  weekday_color    = 0xFF64748B;   ///< Mon–Sun label color.
    Color  day_color        = 0xFFF8FAFC;   ///< Normal day number color.
    Color  weekend_color    = 0xFFEF4444;   ///< Saturday/Sunday color.
    Color  selected_color   = 0xFF0EA5E9;   ///< Selected day highlight.
    Color  today_color      = 0xFF22C55E;   ///< Today's date indicator.
    Color  other_month_color = 0xFF334155;  ///< Days from prev/next month.
    Color  hover_color      = 0xFF334155;   ///< Hover highlight on days.
    float  cell_size        = 40.0f;        ///< Size of each day cell.
    float  header_height    = 52.0f;
    float  font_size        = 13.5f;
    bool   show_week_numbers = false;       ///< Show ISO week numbers (future).
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Calendar widget.
std::shared_ptr<Widget> calendar(CalendarConfig config);

} // namespace vaura
