#pragma once
/// @file shader_button.hpp
/// @brief ShaderButton — A button whose background is rendered using a Skia SkSL Shader.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/tree/widget.hpp"
#include "vaura/core/types.hpp"
#include <string>
#include <functional>
#include <memory>

namespace vaura {

struct ShaderButtonConfig {
    std::string sksl_code;         ///< The SkSL shader code (must define main(float2))
    Color color1 = 0xFF4A90E2;     ///< Primary color passed to shader
    Color color2 = 0xFF50E3C2;     ///< Secondary color passed to shader
    float border_radius = 8.0f;    ///< Button border radius
    float width = -1.0f;           ///< -1 for auto
    float height = 40.0f;          ///< Fixed height
    
    std::function<void()> on_tap;  ///< Callback for click
    WidgetPtr child;               ///< Child widget (e.g., text)
};

class ShaderButtonWidget : public StatefulWidget {
public:
    ShaderButtonConfig config;

    explicit ShaderButtonWidget(ShaderButtonConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "ShaderButton"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

inline WidgetPtr shader_button(ShaderButtonConfig config) {
    return std::make_shared<ShaderButtonWidget>(std::move(config));
}

} // namespace vaura
