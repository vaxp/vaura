/// @file transform.cpp
/// @brief Transform widget implementation.

#include "vaura/widgets/transform.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"

namespace vaura {

class RenderTransform : public RenderBox {
public:
    TransformConfig config;

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        
        canvas.save();

        float origin_dx = context.offset.x + size_.width * config.origin_x;
        float origin_dy = context.offset.y + size_.height * config.origin_y;

        // Apply transformations around origin
        canvas.translate(config.translate_x, config.translate_y);
        
        canvas.translate(origin_dx, origin_dy);
        canvas.rotate(config.rotation);
        canvas.scale(config.scale_x, config.scale_y);
        canvas.translate(-origin_dx, -origin_dy);

        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }

        canvas.restore();
    }
};

std::unique_ptr<RenderObject> TransformWidget::createRenderObject(BuildContext& ctx) {
    auto obj = std::make_unique<RenderTransform>();
    updateRenderObject(ctx, *obj);
    return obj;
}

void TransformWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& ro = static_cast<RenderTransform&>(renderObject);
    ro.config = config;
    ro.markNeedsPaint();
}

} // namespace vaura
