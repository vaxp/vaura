#pragma once
/// @file stack.hpp
/// @brief Stack widget — overlaps children.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"

namespace vaura {

struct StackConfig {
    Alignment alignment = Alignment::TopLeft;
    std::vector<WidgetPtr> children;
};

class StackWidget : public MultiChildRenderObjectWidget {
public:
    StackConfig config;

    explicit StackWidget(StackConfig cfg)
        : config(std::move(cfg)) {
        this->children = config.children;
    }

    [[nodiscard]] std::string_view typeName() const override {
        return "Stack";
    }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override;
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override;
};

inline WidgetPtr stack(StackConfig config) {
    return std::make_shared<StackWidget>(std::move(config));
}

} // namespace vaura
