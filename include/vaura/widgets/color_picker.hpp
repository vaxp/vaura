#pragma once
/// @file color_picker.hpp
/// @brief ColorPicker widget — HSV picker with hue slider, alpha, and hex input.
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

struct ColorPickerConfig {
    Color    selected_color   = 0xFF0EA5E9;      ///< Initial color.
    std::function<void(Color)> on_changed;

    bool   show_alpha     = true;                ///< Show alpha (opacity) slider.
    bool   show_hex_input = true;                ///< Show hex text field.
    float  width          = 280.0f;
    Color  background_color = 0xFF1E293B;
    Color  text_color       = 0xFFF8FAFC;
    float  font_size        = 13.0f;
};

/// Create a ColorPicker widget.
std::shared_ptr<Widget> color_picker(ColorPickerConfig config);

} // namespace vaura
