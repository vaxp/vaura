/// @file opacity.cpp
/// @brief Opacity widget implementation.

#include "vaura/widgets/opacity.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"

namespace vaura {

class RenderOpacity : public RenderBox {
public:
    float opacity = 1.0f;

    void paint(PaintContext& context) override {
        if (opacity <= 0.0f) {
            return; // Fully transparent, don't paint
        }
        
        if (opacity >= 1.0f) {
            for (auto* child : children_) {
                PaintContext child_ctx = context.withOffset(child->offset());
                child->paint(child_ctx);
            }
            return;
        }

        // Partially transparent
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);
        canvas.saveLayerAlpha(opacity, &bounds);
        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }
        canvas.restore();
    }
};

std::unique_ptr<RenderObject> OpacityWidget::createRenderObject(BuildContext& ctx) {
    auto obj = std::make_unique<RenderOpacity>();
    updateRenderObject(ctx, *obj);
    return obj;
}

void OpacityWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& ro = static_cast<RenderOpacity&>(renderObject);
    if (ro.opacity != config.opacity) {
        ro.opacity = config.opacity;
        ro.markNeedsPaint();
    }
}

} // namespace vaura
