#include "vaura/widgets/stack.hpp"

namespace vaura {

class RenderStack : public RenderBox {
public:
    Alignment alignment = Alignment::TopLeft;

    void syncLayout() override {
        // Enforce Yoga properties for stack
        auto node = yogaNode();
        YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumn);

        // Map alignment to justify/align
        if (alignment == Alignment::Center) {
            YGNodeStyleSetJustifyContent(node, YGJustifyCenter);
            YGNodeStyleSetAlignItems(node, YGAlignCenter);
        } else if (alignment == Alignment::TopLeft) {
            YGNodeStyleSetJustifyContent(node, YGJustifyFlexStart);
            YGNodeStyleSetAlignItems(node, YGAlignFlexStart);
        } else if (alignment == Alignment::BottomRight) {
            YGNodeStyleSetJustifyContent(node, YGJustifyFlexEnd);
            YGNodeStyleSetAlignItems(node, YGAlignFlexEnd);
        }

        // Make all children except the first one absolute
        for (size_t i = 0; i < children_.size(); ++i) {
            auto cnode = children_[i]->yogaNode();
            if (i == 0) {
                YGNodeStyleSetPositionType(cnode, YGPositionTypeRelative);
            } else {
                YGNodeStyleSetPositionType(cnode, YGPositionTypeAbsolute);
                // Center absolute children by default if alignment is center?
                // For now, let Yoga handle absolute positioning (top/left 0 by default)
            }
        }

        RenderBox::syncLayout();
    }

    void paint(PaintContext& context) override {
        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }
    }
};

std::unique_ptr<RenderObject> StackWidget::createRenderObject(BuildContext& ctx) {
    auto obj = std::make_unique<RenderStack>();
    updateRenderObject(ctx, *obj);
    return obj;
}

void StackWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& stack = static_cast<RenderStack&>(renderObject);
    stack.alignment = config.alignment;
    stack.markNeedsLayout();
}

} // namespace vaura
