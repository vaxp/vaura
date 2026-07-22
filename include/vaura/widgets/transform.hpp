#pragma once
/// @file transform.hpp
/// @brief Transform widget to scale, rotate, and translate its child.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/tree/widget.hpp"

namespace vaura {

struct TransformConfig {
    float scale_x = 1.0f;
    float scale_y = 1.0f;
    float translate_x = 0.0f;
    float translate_y = 0.0f;
    float rotation = 0.0f; // degrees
    
    // Origin for scaling and rotation (relative to child's bounding box [0..1])
    float origin_x = 0.5f;
    float origin_y = 0.5f;
    
    WidgetPtr child = nullptr;
};

class TransformWidget : public SingleChildRenderObjectWidget {
public:
    TransformConfig config;

    explicit TransformWidget(TransformConfig cfg) : config(std::move(cfg)) {
        this->child = config.child;
    }

    [[nodiscard]] std::string_view typeName() const override { return "Transform"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override;
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override;
};

inline WidgetPtr transform(TransformConfig config) {
    return std::make_shared<TransformWidget>(std::move(config));
}

// Helpers
inline WidgetPtr scale(float scale, WidgetPtr child) {
    return transform({.scale_x = scale, .scale_y = scale, .child = std::move(child)});
}

inline WidgetPtr rotate(float degrees, WidgetPtr child) {
    return transform({.rotation = degrees, .child = std::move(child)});
}

inline WidgetPtr translate(float dx, float dy, WidgetPtr child) {
    return transform({.translate_x = dx, .translate_y = dy, .child = std::move(child)});
}

/// Declarative syntax alias for Transform
inline WidgetPtr Transform(TransformConfig config) {
    return transform(std::move(config));
}

/// Declarative syntax alias for Scale
inline WidgetPtr Scale(float scale, WidgetPtr child) {
    return vaura::scale(scale, std::move(child));
}

/// Declarative syntax alias for Rotate
inline WidgetPtr Rotate(float degrees, WidgetPtr child) {
    return vaura::rotate(degrees, std::move(child));
}

/// Declarative syntax alias for Translate
inline WidgetPtr Translate(float dx, float dy, WidgetPtr child) {
    return vaura::translate(dx, dy, std::move(child));
}

} // namespace vaura
