#pragma once
/// @file radio.hpp
/// @brief Radio button widget — single selection circular toggle.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>
#include <memory>

namespace vaura {

/// Configuration for a RadioButton widget.
struct RadioConfig {
    bool value = false;                                      ///< Current state (true = selected, false = unselected).
    std::function<void(bool)> on_changed;                    ///< Callback when state changes.
    float size = 24.0f;                                      ///< Size of the radio button (width & height).
    Color active_color = Colors::Blue;                       ///< Color when selected (border and inner circle).
    Color inactive_color = Color(0xFF9CA3AF);                ///< Color of the border when unselected.
    float stroke_width = 2.0f;                               ///< Width of the border.
    float inner_padding = 4.0f;                              ///< Padding between the border and the inner circle.
};

/// Create a RadioButton widget.
std::shared_ptr<Widget> radio(RadioConfig config = {});

/// Declarative syntax alias for Radio
inline std::shared_ptr<Widget> Radio(RadioConfig config = {}) {
    return radio(std::move(config));
}

} // namespace vaura
