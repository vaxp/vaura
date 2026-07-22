#pragma once
/// @file text.hpp
/// @brief Text widget — renders styled text.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/tree/build_context.hpp"
#include <string>
#include <memory>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// TextStyle — comprehensive text styling
// ════════════════════════════════════════════════════════════════

/// @brief Comprehensive text style configuration.
struct TextStyle {
    float font_size = 14.0f;                   ///< Font size in logical pixels.
    Color color = Colors::Black;               ///< Text color.
    std::string font_family = "sans-serif";    ///< Font family name.

    /// Font weight constants (CSS-style numeric values).
    enum Weight : int {
        Thin = 100, ExtraLight = 200, Light = 300, Regular = 400,
        Medium = 500, SemiBold = 600, Bold = 700, ExtraBold = 800,
        Black = 900
    } weight = Regular;

    /// Text alignment.
    enum Align { Left, Center, Right, Justify } align = Left;

    /// Text overflow behavior.
    enum Overflow { Clip, Ellipsis, Fade } overflow = Clip;

    /// Font style.
    enum Style { Normal, Italic } style = Normal;

    int max_lines = 0;                         ///< 0 = unlimited.
    float line_height = 0;                     ///< 0 = auto.
    float letter_spacing = 0;                  ///< Additional letter spacing.
    bool underline = false;                    ///< Underline decoration.
    bool strikethrough = false;                ///< Strikethrough decoration.
};

// ════════════════════════════════════════════════════════════════
// TextConfig — declarative configuration
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Text widget.
/// @code
///   text("Hello World", {
///       .font_size = 24,
///       .color = Colors::White,
///       .weight = TextStyle::Bold,
///       .align = TextStyle::Center,
///   });
/// @endcode
struct TextConfig {
    float font_size = 14.0f;
    Color color = Colors::Black;
    std::string font_family = "sans-serif";
    TextStyle::Weight weight = TextStyle::Regular;
    TextStyle::Align align = TextStyle::Left;
    TextStyle::Style style = TextStyle::Normal;
    int max_lines = 0;
    TextStyle::Overflow overflow = TextStyle::Clip;
    float line_height = 0;
    float letter_spacing = 0;
    bool underline = false;
    bool strikethrough = false;
};

// ════════════════════════════════════════════════════════════════
// TextWidget
// ════════════════════════════════════════════════════════════════

/// @brief Renders a text string with styling.
class TextWidget : public SingleChildRenderObjectWidget {
public:
    std::string content;
    TextConfig config;

    TextWidget(std::string text, TextConfig cfg = {})
        : content(std::move(text)), config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override {
        return "Text";
    }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(
        BuildContext& ctx) override;

    void updateRenderObject(BuildContext& ctx,
                           RenderObject& renderObject) override;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Text widget.
inline WidgetPtr text(std::string_view content, TextConfig config = {}) {
    return std::make_shared<TextWidget>(std::string(content), std::move(config));
}

}  // namespace vaura
