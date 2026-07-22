#pragma once
/// @file checkbox.hpp
/// @brief Checkbox widget — an interactive boolean toggle.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/animation/animation_controller.hpp"
#include <functional>

namespace vaura {

/// @brief Configuration for the Checkbox widget.
struct CheckboxConfig {
    bool value = false;                                      ///< Current checked state.
    std::function<void(bool)> on_changed;                    ///< Callback when state changes.
    float size = 24.0f;                                      ///< Size of the checkbox (width & height).
    Color active_color = Colors::Blue;                       ///< Color when checked.
    Color inactive_color = Color(0xFF9CA3AF);                ///< Color of the border when unchecked.
    Color checkmark_color = Colors::White;                   ///< Color of the checkmark.
    float border_radius = 4.0f;                              ///< Border radius of the box.
    float stroke_width = 2.0f;                               ///< Width of the border and checkmark.
};

/// @brief A standard Checkbox widget.
/// @param config Appearance and behavior configuration.
/// @return A configured Checkbox widget.
std::shared_ptr<Widget> checkbox(CheckboxConfig config = {});

/// Declarative syntax alias for Checkbox
inline std::shared_ptr<Widget> Checkbox(CheckboxConfig config = {}) {
    return checkbox(std::move(config));
}

} // namespace vaura
