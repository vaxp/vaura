#pragma once
/// @file button.hpp
/// @brief Button widget — interactive button with states and ripple effect.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/tree/build_context.hpp"
#include <functional>
#include <memory>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// ButtonConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Button widget.
struct ButtonConfig {
    WidgetPtr child;                                  ///< Button content (e.g., text).
    std::function<void()> on_pressed;                 ///< Press callback.
    std::function<void()> on_long_press;              ///< Long press callback.
    Color color = 0xFF2196F3;                         ///< Background color (Material Blue).
    Color hover_color = 0;                            ///< Hover color (0 = auto).
    Color pressed_color = 0;                          ///< Pressed color (0 = auto).
    Color disabled_color = 0;                         ///< Disabled color (0 = auto).
    Color splash_color = 0x40FFFFFF;                  ///< Ripple/splash color.
    float border_radius = 8.0f;                       ///< Corner radius.
    EdgeInsets padding = EdgeInsets(12, 24, 12, 24);  ///< Inner padding.
    Border border;                                    ///< Optional border.
    float elevation = 2.0f;                           ///< Elevation (shadow depth).
    bool enabled = true;                              ///< Whether the button is enabled.
    float min_width = 64.0f;                          ///< Minimum width.
    float min_height = 36.0f;                         ///< Minimum height.
    float width = 0.0f;                               ///< Explicit width (0 = auto).
    std::optional<float> width_percent;               ///< Width percent.
};

// ════════════════════════════════════════════════════════════════
// ButtonWidget
// ════════════════════════════════════════════════════════════════

/// @brief An interactive button with hover, press, and ripple states.
class ButtonWidget : public StatefulWidget {
public:
    ButtonConfig config;

    explicit ButtonWidget(ButtonConfig cfg)
        : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override {
        return "Button";
    }

    [[nodiscard]] std::unique_ptr<State> createState() override;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Button widget.
inline WidgetPtr button(ButtonConfig config) {
    return std::make_shared<ButtonWidget>(std::move(config));
}

}  // namespace vaura
