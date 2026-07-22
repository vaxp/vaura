#pragma once
/// @file text_field.hpp
/// @brief TextField widget — interactive single-line text input.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>
#include <memory>
#include <string>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// TextFieldConfig
// ════════════════════════════════════════════════════════════════

struct TextFieldConfig {
    std::string placeholder = "Enter text...";
    std::string initial_text = "";
    std::function<void(std::string_view)> on_changed;
    std::function<void(std::string_view)> on_submitted;
    
    Color bg_color = 0x1AFFFFFF;
    Color text_color = 0xFFFFFFFF;
    Color placeholder_color = 0x66FFFFFF;
    Color cursor_color = 0xFF00D4FF;
    
    float font_size = 15.0f;
    float border_radius = 12.0f;
    EdgeInsets padding = EdgeInsets(16, 12, 16, 12);
    
    float height = 50.0f;
    float width = 0.0f;
    std::optional<float> width_percent;
};

// ════════════════════════════════════════════════════════════════
// TextFieldWidget
// ════════════════════════════════════════════════════════════════

class TextFieldWidget : public StatefulWidget {
public:
    TextFieldConfig config;

    explicit TextFieldWidget(TextFieldConfig cfg)
        : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override {
        return "TextField";
    }

    [[nodiscard]] std::unique_ptr<State> createState() override;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

inline WidgetPtr text_field(TextFieldConfig config) {
    return std::make_shared<TextFieldWidget>(std::move(config));
}

/// Declarative syntax alias for TextField
inline WidgetPtr TextField(TextFieldConfig config) {
    return text_field(std::move(config));
}

} // namespace vaura
