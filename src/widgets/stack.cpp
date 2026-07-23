#include "vaura/widgets/stack.hpp"

namespace vaura {

class RenderStack : public RenderBox {
public:
    Alignment alignment = Alignment::TopLeft;

    void syncLayout() override {
        // Enforce Anu properties for stack
        auto node = anuNode();
        ANUNodeStyleSetFlexDirection(node, static_cast<ANUFlexDirection>(FlexDirection::Column));

        // Map alignment to justify/align
        if (alignment == Alignment::Center) {
            ANUNodeStyleSetJustifyContent(node, static_cast<ANUJustify>(Justify::Center));
            ANUNodeStyleSetAlignItems(node, static_cast<ANUAlign>(Align::Center));
        } else if (alignment == Alignment::TopLeft) {
            ANUNodeStyleSetJustifyContent(node, static_cast<ANUJustify>(Justify::Start));
            ANUNodeStyleSetAlignItems(node, static_cast<ANUAlign>(Align::Start));
        } else if (alignment == Alignment::BottomRight) {
            ANUNodeStyleSetJustifyContent(node, static_cast<ANUJustify>(Justify::End));
            ANUNodeStyleSetAlignItems(node, static_cast<ANUAlign>(Align::End));
        }

        // Make all children except the first one absolute
        for (size_t i = 0; i < children_.size(); ++i) {
            auto cnode = children_[i]->anuNode();
            if (i == 0) {
                ANUNodeStyleSetPositionType(cnode, static_cast<ANUPositionType>(PositionType::Relative));
            } else {
                ANUNodeStyleSetPositionType(cnode, static_cast<ANUPositionType>(PositionType::Absolute));
                // Center absolute children by default if alignment is center?
                // For now, let Anu handle absolute positioning (top/left 0 by default)
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
