#pragma once
/// @file scroll_view.hpp
/// @brief ScrollView widget — clips and scrolls its child.
///
/// A container that allows its child to grow larger than the container itself,
/// clipping the overflow and providing native scroll event handling to pan the child.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"

namespace vaura {

// ════════════════════════════════════════════════════════════════
// ScrollViewConfig
// ════════════════════════════════════════════════════════════════

struct ScrollViewConfig {
    WidgetPtr child;
    bool show_scrollbar = true; // For future implementation
    float scroll_speed = 30.0f;
    float border_radius = 0.0f;
};

// ════════════════════════════════════════════════════════════════
// ScrollViewWidget
// ════════════════════════════════════════════════════════════════

class ScrollViewWidget : public SingleChildRenderObjectWidget {
public:
    ScrollViewConfig config;

    explicit ScrollViewWidget(ScrollViewConfig cfg)
        : config(std::move(cfg)) {
        this->child = config.child;
    }

    [[nodiscard]] std::string_view typeName() const override {
        return "ScrollView";
    }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override;
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

inline WidgetPtr scroll_view(ScrollViewConfig config) {
    return std::make_shared<ScrollViewWidget>(std::move(config));
}

/// Declarative syntax alias for ScrollView
inline WidgetPtr ScrollView(ScrollViewConfig config) {
    return scroll_view(std::move(config));
}

} // namespace vaura
