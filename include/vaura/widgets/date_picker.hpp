#pragma once
/// @file date_picker.hpp
/// @brief DatePicker widget — a text field that opens a Calendar popup on click.
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

struct DatePickerConfig {
    int  selected_year  = 2025;
    int  selected_month = 1;
    int  selected_day   = 1;

    std::function<void(int, int, int)> on_changed; ///< year, month, day

    std::string  placeholder     = "Select date...";
    Color  background_color      = 0xFF1E293B;
    Color  text_color            = 0xFFF8FAFC;
    Color  border_color          = 0xFF334155;
    Color  focus_color           = 0xFF0EA5E9;
    float  border_radius         = 8.0f;
    float  width                 = 200.0f;
    float  height                = 44.0f;
    float  font_size             = 14.0f;
};

/// Create a DatePicker widget.
std::shared_ptr<Widget> date_picker(DatePickerConfig config);

/// Declarative syntax alias for DatePicker
inline std::shared_ptr<Widget> DatePicker(DatePickerConfig config) {
    return date_picker(std::move(config));
}

} // namespace vaura
