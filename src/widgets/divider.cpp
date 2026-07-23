#include "vaura/widgets/divider.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/paint.hpp"
#include <layout_engine/Anu.h>

namespace vaura {

class RenderDivider : public RenderBox {
public:
    DividerConfig config;

    RenderDivider(DividerConfig config) : config(std::move(config)) {}

    void setConfig(const DividerConfig& new_config) {
        bool needs_layout = false;
        bool needs_repaint = false;

        if (config.thickness != new_config.thickness ||
            config.vertical != new_config.vertical ||
            config.indent != new_config.indent ||
            config.end_indent != new_config.end_indent) {
            needs_layout = true;
        }

        if (config.color != new_config.color) {
            needs_repaint = true;
        }

        config = new_config;

        if (needs_layout) {
            updateLayout();
        } else if (needs_repaint) {
            markNeedsPaint();
        }
    }

    void updateLayout() {
        auto node = anuNode();
        ANUNodeStyleSetAlignSelf(node, static_cast<ANUAlign>(Align::Stretch));
        if (config.vertical) {
            ANUNodeStyleSetWidth(node, config.thickness);
            ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Top), config.indent);
            ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Bottom), config.end_indent);
        } else {
            ANUNodeStyleSetHeight(node, config.thickness);
            ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Left), config.indent);
            ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Right), config.end_indent);
        }
        markNeedsLayout();
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        Paint line_paint;
        line_paint.setColor(config.color);
        line_paint.setStyle(PaintStyle::Fill);
        
        canvas.drawRect(bounds, line_paint);
    }
};

class DividerWidget : public SingleChildRenderObjectWidget {
public:
    DividerConfig config;

    explicit DividerWidget(DividerConfig config) : config(std::move(config)) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "DividerWidget"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderDivider>(config);
        obj->updateLayout();
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& render_object) override {
        auto& ro = static_cast<RenderDivider&>(render_object);
        ro.setConfig(config);
    }
};

std::shared_ptr<Widget> divider(DividerConfig config) {
    return std::make_shared<DividerWidget>(std::move(config));
}

} // namespace vaura
