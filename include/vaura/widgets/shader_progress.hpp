#pragma once
/// @file shader_progress.hpp
/// @brief Advanced SkSL Shader-based loading indicators.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>

namespace vaura {

struct ShaderProgressConfig {
    float size = 80.0f;
    Color color1 = 0xFF00FFFF; // Primary color
    Color color2 = 0xFFFF00FF; // Secondary color
    std::string sksl_code;
};

class ShaderProgressWidget : public StatefulWidget {
public:
    ShaderProgressConfig config;

    explicit ShaderProgressWidget(ShaderProgressConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "ShaderProgress"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

/// @brief Pre-built Galaxy Spinner
inline WidgetPtr galaxy_progress(float size = 80.0f, Color c1 = 0xFF3B82F6, Color c2 = 0xFF8B5CF6) {
    ShaderProgressConfig cfg;
    cfg.size = size;
    cfg.color1 = c1;
    cfg.color2 = c2;
    // We inject the SkSL in the .cpp file, but pass a special ID here
    cfg.sksl_code = "galaxy";
    return std::make_shared<ShaderProgressWidget>(std::move(cfg));
}

/// @brief Pre-built Liquid Wave Spinner
inline WidgetPtr liquid_progress(float size = 80.0f, Color c1 = 0xFF06B6D4, Color c2 = 0xFF3B82F6) {
    ShaderProgressConfig cfg;
    cfg.size = size;
    cfg.color1 = c1;
    cfg.color2 = c2;
    cfg.sksl_code = "liquid";
    return std::make_shared<ShaderProgressWidget>(std::move(cfg));
}

} // namespace vaura
