#pragma once

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"

namespace vaura {

class RenderIgnorePointer : public RenderBox {
public:
    void paint(PaintContext& context) override {
        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }
    }

    bool hitTest(HitTestResult& result, Point localPoint) override {
        return false; // Ignore all hits
    }
};

class IgnorePointerWidget : public SingleChildRenderObjectWidget {
public:
    explicit IgnorePointerWidget(WidgetPtr child) {
        this->child = std::move(child);
    }
    [[nodiscard]] std::string_view typeName() const override { return "IgnorePointer"; }
    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        return std::make_unique<RenderIgnorePointer>();
    }
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {}
};

inline WidgetPtr ignore_pointer(WidgetPtr child) {
    return std::make_shared<IgnorePointerWidget>(std::move(child));
}

} // namespace vaura
