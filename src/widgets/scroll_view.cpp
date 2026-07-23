/// @file scroll_view.cpp
/// @brief ScrollView widget implementation.

#include "vaura/widgets/scroll_view.hpp"
#include "vaura/rendering/canvas.hpp"
#include <layout_engine/Anu.h>
#include <algorithm>

namespace vaura {

class RenderScrollView : public RenderBox {
public:
    ScrollViewConfig config;
    float scroll_y_ = 0.0f;
    float max_scroll_y_ = 0.0f;

    RenderScrollView() {
        // Allow children to overflow vertically without increasing the container's height
        ANUNodeStyleSetOverflow(anuNode(), static_cast<ANUOverflow>(Overflow::Scroll));
        // ScrollView should expand to fill remaining space, and shrink if space is tight
        ANUNodeStyleSetFlexGrow(anuNode(), 1.0f);
        ANUNodeStyleSetFlexShrink(anuNode(), 1.0f);
    }

    void syncLayout() override {
        RenderBox::syncLayout();

        // Calculate max_scroll_y_ based on child's actual height vs our height
        if (!children_.empty() && children_[0]) {
            float child_height = children_[0]->size().height;
            float my_height = size().height;
            max_scroll_y_ = std::max(0.0f, child_height - my_height);
        } else {
            max_scroll_y_ = 0.0f;
        }

        // Clamp scroll_y_ in case the child shrank
        scroll_y_ = std::clamp(scroll_y_, 0.0f, max_scroll_y_);
    }

    void paint(PaintContext& context) override {
        if (children_.empty() || !children_[0]) return;

        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        canvas.save();

        // Clip to the scroll view bounds
        if (config.border_radius > 0.0f) {
            canvas.clipRRect(bounds, BorderRadius::circular(config.border_radius));
        } else {
            canvas.clipRect(bounds);
        }

        // Paint child with translation offset
        PaintContext child_ctx = context;
        // The child's local offset (from Anu) is already handled in hitTestChildren / paint normally?
        // Wait, normally `paint` iterates children and does:
        // `PaintContext child_ctx = context.withOffset(child->offset());`
        // We must do the same, BUT subtract the scroll offset!
        Point child_anu_offset = children_[0]->offset();
        Point final_offset = {
            child_anu_offset.x,
            child_anu_offset.y - scroll_y_
        };

        PaintContext scrolled_ctx = context.withOffset(final_offset);
        children_[0]->paint(scrolled_ctx);

        canvas.restore();
        
        // TODO: Draw scrollbar
    }

    bool hitTest(HitTestResult& result, Point localPoint) override {
        // Clip hit testing to our bounds since we clip painting
        if (localPoint.x < 0 || localPoint.x >= size_.width ||
            localPoint.y < 0 || localPoint.y >= size_.height) {
            return false;
        }
        return RenderBox::hitTest(result, localPoint);
    }

    bool hitTestChildren(HitTestResult& result, Point localPoint) override {
        if (children_.empty() || !children_[0]) return false;

        auto* child = children_[0];
        
        // Convert the local point (relative to ScrollView) 
        // to child's local point, taking into account the scroll offset!
        Point child_local = {
            localPoint.x - child->offset().x,
            localPoint.y - child->offset().y + scroll_y_
        };

        return child->hitTest(result, child_local);
    }

    bool handlesScroll() const override { return true; }

    void handlePointerScroll(float dx, float dy) override {
        // In SDL/most platforms, scrolling down gives positive dy, or negative?
        // Let's just assume we subtract dy * speed. We can invert if necessary.
        float new_scroll = scroll_y_ - dy * config.scroll_speed;
        scroll_y_ = std::clamp(new_scroll, 0.0f, max_scroll_y_);
        markNeedsPaint();
    }
};

std::unique_ptr<RenderObject> ScrollViewWidget::createRenderObject(BuildContext& ctx) {
    auto obj = std::make_unique<RenderScrollView>();
    updateRenderObject(ctx, *obj);
    return obj;
}

void ScrollViewWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& sv = static_cast<RenderScrollView&>(renderObject);
    sv.config = config;
    sv.markNeedsLayout();
}

} // namespace vaura
