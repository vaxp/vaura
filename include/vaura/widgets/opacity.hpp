#pragma once
/// @file opacity.hpp
/// @brief Opacity widget to make its child partially transparent.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/tree/widget.hpp"

namespace vaura {

struct OpacityConfig {
    float opacity = 1.0f; // [0.0, 1.0]
    WidgetPtr child = nullptr;
};

class OpacityWidget : public SingleChildRenderObjectWidget {
public:
    OpacityConfig config;

    explicit OpacityWidget(OpacityConfig cfg) : config(std::move(cfg)) {
        this->child = config.child;
    }

    [[nodiscard]] std::string_view typeName() const override { return "Opacity"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override;
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override;
};

inline WidgetPtr opacity(float value, WidgetPtr child) {
    return std::make_shared<OpacityWidget>(OpacityConfig{
        .opacity = value,
        .child = std::move(child)
    });
}

/// Declarative syntax alias for Opacity
inline WidgetPtr Opacity(OpacityConfig config) {
    return std::make_shared<OpacityWidget>(std::move(config));
}

/// Declarative syntax alias for Opacity (value and child)
inline WidgetPtr Opacity(float value, WidgetPtr child) {
    return opacity(value, std::move(child));
}

} // namespace vaura
