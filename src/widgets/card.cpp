#include "vaura/widgets/card.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/paint.hpp"
#include "vaura/tree/render_object.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

class RenderCard : public RenderBox {
public:
    CardConfig config;

    RenderCard(CardConfig config) : config(std::move(config)) {}

    void setConfig(const CardConfig& new_config) {
        bool needs_repaint = false;
        bool needs_layout = false;

        if (config.padding.left != new_config.padding.left ||
            config.padding.top != new_config.padding.top ||
            config.padding.right != new_config.padding.right ||
            config.padding.bottom != new_config.padding.bottom) {
            needs_layout = true;
        }

        if (config.color != new_config.color ||
            config.border_radius != new_config.border_radius ||
            config.elevation != new_config.elevation ||
            config.shadow_color != new_config.shadow_color ||
            config.shadow_dx != new_config.shadow_dx ||
            config.shadow_dy != new_config.shadow_dy) {
            needs_repaint = true;
        }

        config = new_config;

        if (needs_layout) {
            auto node = yogaNode();
            YGNodeStyleSetPadding(node, YGEdgeLeft, config.padding.left);
            YGNodeStyleSetPadding(node, YGEdgeTop, config.padding.top);
            YGNodeStyleSetPadding(node, YGEdgeRight, config.padding.right);
            YGNodeStyleSetPadding(node, YGEdgeBottom, config.padding.bottom);
            markNeedsLayout();
        } else if (needs_repaint) {
            markNeedsPaint();
        }
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        Paint bg_paint;
        bg_paint.setColor(config.color);
        bg_paint.setStyle(PaintStyle::Fill);
        bg_paint.setAntiAlias(true);
        
        // Add drop shadow
        if (config.elevation > 0.0f) {
            bg_paint.setShadow(config.shadow_color, config.elevation, config.shadow_dx, config.shadow_dy);
        }

        BorderRadius br{config.border_radius, config.border_radius, config.border_radius, config.border_radius};

        canvas.drawRRect(bounds, br, bg_paint);
        
        bool needs_clip = config.border_radius > 0;
        if (needs_clip) {
            canvas.save();
            canvas.clipRRect(bounds, br);
        }

        // Paint children
        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }

        if (needs_clip) {
            canvas.restore();
        }
    }
};

class CardWidget : public SingleChildRenderObjectWidget {
public:
    CardConfig config;

    CardWidget(CardConfig config, std::shared_ptr<Widget> child)
        : config(std::move(config)) {
        this->child = std::move(child);
    }

    [[nodiscard]] std::string_view typeName() const override { return "CardWidget"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderCard>(config);
        auto node = obj->yogaNode();
        YGNodeStyleSetPadding(node, YGEdgeLeft, config.padding.left);
        YGNodeStyleSetPadding(node, YGEdgeTop, config.padding.top);
        YGNodeStyleSetPadding(node, YGEdgeRight, config.padding.right);
        YGNodeStyleSetPadding(node, YGEdgeBottom, config.padding.bottom);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& render_object) override {
        auto& ro = static_cast<RenderCard&>(render_object);
        ro.setConfig(config);
    }
};

std::shared_ptr<Widget> card(CardConfig config, std::shared_ptr<Widget> child) {
    return std::make_shared<CardWidget>(std::move(config), std::move(child));
}

} // namespace vaura
