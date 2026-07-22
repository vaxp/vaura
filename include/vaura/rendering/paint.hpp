#pragma once
/// @file paint.hpp
/// @brief Painting style properties.

#include "vaura/core/types.hpp"

namespace vaura {

enum class PaintStyle {
    Fill,
    Stroke,
    StrokeAndFill
};

enum class StrokeCap {
    Butt,
    Round,
    Square
};

enum class StrokeJoin {
    Miter,
    Round,
    Bevel
};

/// Paint encapsulates styling information for drawing operations.
class Paint {
public:
    Paint();
    ~Paint() = default;

    void setColor(Color color) { color_ = color; }
    [[nodiscard]] Color getColor() const { return color_; }

    void setStyle(PaintStyle style) { style_ = style; }
    [[nodiscard]] PaintStyle getStyle() const { return style_; }

    void setStrokeWidth(float width) { stroke_width_ = width; }
    [[nodiscard]] float getStrokeWidth() const { return stroke_width_; }

    void setStrokeCap(StrokeCap cap) { stroke_cap_ = cap; }
    [[nodiscard]] StrokeCap getStrokeCap() const { return stroke_cap_; }

    void setStrokeJoin(StrokeJoin join) { stroke_join_ = join; }
    [[nodiscard]] StrokeJoin getStrokeJoin() const { return stroke_join_; }

    void setAntiAlias(bool aa) { anti_alias_ = aa; }
    [[nodiscard]] bool isAntiAlias() const { return anti_alias_; }

    void setShadow(Color color, float blur, float dx, float dy) {
        shadow_color_ = color;
        shadow_blur_ = blur;
        shadow_offset_dx_ = dx;
        shadow_offset_dy_ = dy;
        has_shadow_ = true;
    }
    [[nodiscard]] bool hasShadow() const { return has_shadow_; }
    [[nodiscard]] Color getShadowColor() const { return shadow_color_; }
    [[nodiscard]] float getShadowBlur() const { return shadow_blur_; }
    [[nodiscard]] float getShadowDx() const { return shadow_offset_dx_; }
    [[nodiscard]] float getShadowDy() const { return shadow_offset_dy_; }

private:
    Color      color_        = 0xFF000000;
    PaintStyle style_        = PaintStyle::Fill;
    float      stroke_width_ = 0.0f;
    StrokeCap  stroke_cap_   = StrokeCap::Butt;
    StrokeJoin stroke_join_  = StrokeJoin::Miter;
    bool       anti_alias_   = true;

    bool       has_shadow_       = false;
    Color      shadow_color_     = 0x00000000;
    float      shadow_blur_      = 0.0f;
    float      shadow_offset_dx_ = 0.0f;
    float      shadow_offset_dy_ = 0.0f;
};

}  // namespace vaura
