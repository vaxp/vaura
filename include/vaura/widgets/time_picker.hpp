#pragma once
/// @file time_picker.hpp
/// @brief TimePicker widget — clock-style or spinner time selector.
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

struct TimePickerConfig {
    int  hour   = 12;     ///< 0–23
    int  minute = 0;      ///< 0–59
    bool use_24h = true;

    std::function<void(int, int)> on_changed; ///< hour (0-23), minute

    Color  background_color  = 0xFF1E293B;
    Color  face_color        = 0xFF0F172A;
    Color  hand_color        = 0xFF0EA5E9;
    Color  center_dot_color  = 0xFF0EA5E9;
    Color  text_color        = 0xFFF8FAFC;
    Color  selected_text     = 0xFF0EA5E9;
    Color  dial_bg           = 0xFF334155;
    float  clock_size        = 220.0f;    ///< Diameter of the clock face.
    float  font_size         = 14.0f;
};

/// Create a TimePicker widget.
std::shared_ptr<Widget> time_picker(TimePickerConfig config);

/// Declarative syntax alias for TimePicker
inline std::shared_ptr<Widget> TimePicker(TimePickerConfig config) {
    return time_picker(std::move(config));
}

} // namespace vaura
