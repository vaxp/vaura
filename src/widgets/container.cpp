#include "vaura/widgets/container.hpp"
#include "vaura/rendering/canvas.hpp"
#include <layout_engine/Anu.h>
#include <cmath>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderDecoratedBox
// ════════════════════════════════════════════════════════════════

void RenderDecoratedBox::setDecoration(const Decoration& decoration) {
    if (decoration_.color != decoration.color ||
        decoration_.border_radius.top_left != decoration.border_radius.top_left ||
        decoration_.border_radius.top_right != decoration.border_radius.top_right ||
        decoration_.border_radius.bottom_right != decoration.border_radius.bottom_right ||
        decoration_.border_radius.bottom_left != decoration.border_radius.bottom_left ||
        decoration_.opacity != decoration.opacity ||
        decoration_.clip_content != decoration.clip_content) {
        decoration_ = decoration;
        markNeedsPaint();
    }
}

void RenderDecoratedBox::paint(PaintContext& context) {
    if (decoration_.opacity <= 0.0f) return;

    auto& canvas = context.canvas;
    Rect bounds = Rect::fromPointSize(context.offset, size_);

    if (decoration_.opacity < 1.0f) {
        canvas.saveLayerAlpha(decoration_.opacity, &bounds);
    }

    if (((decoration_.color >> 24) & 0xFF) > 0) {
        Paint bg_paint;
        bg_paint.setColor(decoration_.color);
        bg_paint.setStyle(PaintStyle::Fill);
        
        if (decoration_.border_radius.top_left > 0) {
            canvas.drawRRect(bounds, decoration_.border_radius, bg_paint);
        } else {
            canvas.drawRect(bounds, bg_paint);
        }
    }

    bool needs_clip = decoration_.clip_content && decoration_.border_radius.top_left > 0;
    if (needs_clip) {
        canvas.save();
        canvas.clipRRect(bounds, decoration_.border_radius);
    }

    for (auto* child : children_) {
        PaintContext child_ctx = context.withOffset(child->offset());
        child->paint(child_ctx);
    }

    if (needs_clip) {
        canvas.restore();
    }

    if (decoration_.opacity < 1.0f) {
        canvas.restore();
    }
}

// ════════════════════════════════════════════════════════════════
// ContainerWidget
// ════════════════════════════════════════════════════════════════

std::unique_ptr<RenderObject> ContainerWidget::createRenderObject(BuildContext& ctx) {
    auto render_obj = std::make_unique<RenderDecoratedBox>();
    updateRenderObject(ctx, *render_obj);
    return render_obj;
}

void ContainerWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& box = static_cast<RenderDecoratedBox&>(renderObject);

    RenderDecoratedBox::Decoration dec;
    dec.color = config.color;
    dec.border_radius = config.border_radius;
    dec.border = config.border;
    dec.opacity = config.opacity;
    dec.clip_content = config.clip_content;
    box.setDecoration(dec);

    auto node = box.anuNode();

    // Sizing
    if (config.width_auto) {
        ANUNodeStyleSetWidthAuto(node);
    } else if (config.width_percent.has_value()) {
        ANUNodeStyleSetWidthPercent(node, config.width_percent.value());
    } else if (config.width > 0) {
        ANUNodeStyleSetWidth(node, config.width);
    } else {
        ANUNodeStyleSetWidth(node, ANUUndefined);
    }

    if (config.height_auto) {
        ANUNodeStyleSetHeightAuto(node);
    } else if (config.height_percent.has_value()) {
        ANUNodeStyleSetHeightPercent(node, config.height_percent.value());
    } else if (config.height > 0) {
        ANUNodeStyleSetHeight(node, config.height);
    } else {
        ANUNodeStyleSetHeight(node, ANUUndefined);
    }

    if (config.min_width_percent.has_value()) {
        ANUNodeStyleSetMinWidthPercent(node, config.min_width_percent.value());
    } else if (config.min_width > 0) {
        ANUNodeStyleSetMinWidth(node, config.min_width);
    } else {
        ANUNodeStyleSetMinWidth(node, ANUUndefined);
    }

    if (config.min_height_percent.has_value()) {
        ANUNodeStyleSetMinHeightPercent(node, config.min_height_percent.value());
    } else if (config.min_height > 0) {
        ANUNodeStyleSetMinHeight(node, config.min_height);
    } else {
        ANUNodeStyleSetMinHeight(node, ANUUndefined);
    }

    if (config.max_width_percent.has_value()) {
        ANUNodeStyleSetMaxWidthPercent(node, config.max_width_percent.value());
    } else if (config.max_width > 0) {
        ANUNodeStyleSetMaxWidth(node, config.max_width);
    } else {
        ANUNodeStyleSetMaxWidth(node, ANUUndefined);
    }

    if (config.max_height_percent.has_value()) {
        ANUNodeStyleSetMaxHeightPercent(node, config.max_height_percent.value());
    } else if (config.max_height > 0) {
        ANUNodeStyleSetMaxHeight(node, config.max_height);
    } else {
        ANUNodeStyleSetMaxHeight(node, ANUUndefined);
    }

    ANUNodeStyleSetFlexShrink(node, config.flex_shrink);
    ANUNodeStyleSetFlexGrow(node, config.flex_grow);

    if (config.flex_basis_auto) {
        ANUNodeStyleSetFlexBasisAuto(node);
    } else if (config.flex_basis_percent.has_value()) {
        ANUNodeStyleSetFlexBasisPercent(node, config.flex_basis_percent.value());
    } else {
        ANUNodeStyleSetFlexBasis(node, config.flex_basis);
    }

    ANUNodeStyleSetAspectRatio(node, config.aspect_ratio);
    ANUNodeStyleSetDirection(node, config.direction);
    ANUNodeStyleSetAlignContent(node, config.align_content);
    ANUNodeStyleSetAlignSelf(node, config.align_self);
    ANUNodeStyleSetDisplay(node, config.display);
    ANUNodeStyleSetOverflow(node, config.overflow);
    ANUNodeStyleSetBoxSizing(node, config.box_sizing);

    ANUNodeStyleSetPositionType(node, config.position_type);
    if (config.position_auto) {
        ANUNodeStyleSetPositionAuto(node, static_cast<ANUEdge>(Edge::All));
    } else {
        if (config.position_top_percent.has_value()) {
            ANUNodeStyleSetPositionPercent(node, static_cast<ANUEdge>(Edge::Top), config.position_top_percent.value());
        } else {
            ANUNodeStyleSetPosition(node, static_cast<ANUEdge>(Edge::Top), config.position_top);
        }
        
        if (config.position_bottom_percent.has_value()) {
            ANUNodeStyleSetPositionPercent(node, static_cast<ANUEdge>(Edge::Bottom), config.position_bottom_percent.value());
        } else {
            ANUNodeStyleSetPosition(node, static_cast<ANUEdge>(Edge::Bottom), config.position_bottom);
        }
        
        if (config.position_left_percent.has_value()) {
            ANUNodeStyleSetPositionPercent(node, static_cast<ANUEdge>(Edge::Left), config.position_left_percent.value());
        } else {
            ANUNodeStyleSetPosition(node, static_cast<ANUEdge>(Edge::Left), config.position_left);
        }
        
        if (config.position_right_percent.has_value()) {
            ANUNodeStyleSetPositionPercent(node, static_cast<ANUEdge>(Edge::Right), config.position_right_percent.value());
        } else {
            ANUNodeStyleSetPosition(node, static_cast<ANUEdge>(Edge::Right), config.position_right);
        }
        
        if (config.position_start_percent.has_value()) {
            ANUNodeStyleSetPositionPercent(node, static_cast<ANUEdge>(Edge::Start), config.position_start_percent.value());
        } else {
            ANUNodeStyleSetPosition(node, static_cast<ANUEdge>(Edge::Start), config.position_start);
        }
        
        if (config.position_end_percent.has_value()) {
            ANUNodeStyleSetPositionPercent(node, static_cast<ANUEdge>(Edge::End), config.position_end_percent.value());
        } else {
            ANUNodeStyleSetPosition(node, static_cast<ANUEdge>(Edge::End), config.position_end);
        }
    }

    if (config.gap_percent.has_value()) {
        ANUNodeStyleSetGapPercent(node, ANUGutterAll, config.gap_percent.value());
    } else if (!std::isnan(config.gap)) {
        ANUNodeStyleSetGap(node, ANUGutterAll, config.gap);
    }
    
    if (config.row_gap_percent.has_value()) {
        ANUNodeStyleSetGapPercent(node, ANUGutterRow, config.row_gap_percent.value());
    } else if (!std::isnan(config.row_gap)) {
        ANUNodeStyleSetGap(node, ANUGutterRow, config.row_gap);
    }
    
    if (config.column_gap_percent.has_value()) {
        ANUNodeStyleSetGapPercent(node, ANUGutterColumn, config.column_gap_percent.value());
    } else if (!std::isnan(config.column_gap)) {
        ANUNodeStyleSetGap(node, ANUGutterColumn, config.column_gap);
    }

    if (config.border.width > 0) {
        ANUNodeStyleSetBorder(node, static_cast<ANUEdge>(Edge::All), config.border.width);
    } else {
        ANUNodeStyleSetBorder(node, static_cast<ANUEdge>(Edge::All), ANUUndefined);
    }

    // Padding
    if (config.padding_percent.has_value()) {
        auto& p_pct = config.padding_percent.value();
        ANUNodeStyleSetPaddingPercent(node, static_cast<ANUEdge>(Edge::Left), p_pct.left);
        ANUNodeStyleSetPaddingPercent(node, static_cast<ANUEdge>(Edge::Top), p_pct.top);
        ANUNodeStyleSetPaddingPercent(node, static_cast<ANUEdge>(Edge::Right), p_pct.right);
        ANUNodeStyleSetPaddingPercent(node, static_cast<ANUEdge>(Edge::Bottom), p_pct.bottom);
        ANUNodeStyleSetPaddingPercent(node, static_cast<ANUEdge>(Edge::Start), p_pct.start);
        ANUNodeStyleSetPaddingPercent(node, static_cast<ANUEdge>(Edge::End), p_pct.end);
    } else {
        ANUNodeStyleSetPadding(node, static_cast<ANUEdge>(Edge::Left), config.padding.left);
        ANUNodeStyleSetPadding(node, static_cast<ANUEdge>(Edge::Top), config.padding.top);
        ANUNodeStyleSetPadding(node, static_cast<ANUEdge>(Edge::Right), config.padding.right);
        ANUNodeStyleSetPadding(node, static_cast<ANUEdge>(Edge::Bottom), config.padding.bottom);
        ANUNodeStyleSetPadding(node, static_cast<ANUEdge>(Edge::Start), config.padding.start);
        ANUNodeStyleSetPadding(node, static_cast<ANUEdge>(Edge::End), config.padding.end);
    }

    // Margin
    if (config.margin_auto) {
        ANUNodeStyleSetMarginAuto(node, static_cast<ANUEdge>(Edge::All));
    } else if (config.margin_percent.has_value()) {
        auto& m_pct = config.margin_percent.value();
        ANUNodeStyleSetMarginPercent(node, static_cast<ANUEdge>(Edge::Left), m_pct.left);
        ANUNodeStyleSetMarginPercent(node, static_cast<ANUEdge>(Edge::Top), m_pct.top);
        ANUNodeStyleSetMarginPercent(node, static_cast<ANUEdge>(Edge::Right), m_pct.right);
        ANUNodeStyleSetMarginPercent(node, static_cast<ANUEdge>(Edge::Bottom), m_pct.bottom);
        ANUNodeStyleSetMarginPercent(node, static_cast<ANUEdge>(Edge::Start), m_pct.start);
        ANUNodeStyleSetMarginPercent(node, static_cast<ANUEdge>(Edge::End), m_pct.end);
    } else {
        ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Left), config.margin.left);
        ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Top), config.margin.top);
        ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Right), config.margin.right);
        ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Bottom), config.margin.bottom);
        ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::Start), config.margin.start);
        ANUNodeStyleSetMargin(node, static_cast<ANUEdge>(Edge::End), config.margin.end);
    }

    // Alignment and Flex Properties
    if (config.flex_direction.has_value()) {
        ANUNodeStyleSetFlexDirection(node, config.flex_direction.value());
    } else {
        ANUNodeStyleSetFlexDirection(node, static_cast<ANUFlexDirection>(FlexDirection::Column));
    }

    if (config.flex_wrap.has_value()) {
        ANUNodeStyleSetFlexWrap(node, config.flex_wrap.value());
    }

    if (config.justify_content.has_value()) {
        ANUNodeStyleSetJustifyContent(node, config.justify_content.value());
    } else {
        if (config.alignment == Alignment::Center) {
            ANUNodeStyleSetJustifyContent(node, static_cast<ANUJustify>(Justify::Center));
        } else if (config.alignment == Alignment::TopLeft) {
            ANUNodeStyleSetJustifyContent(node, static_cast<ANUJustify>(Justify::Start));
        } else if (config.alignment == Alignment::BottomRight) {
            ANUNodeStyleSetJustifyContent(node, static_cast<ANUJustify>(Justify::End));
        }
    }
    
    if (config.align_items.has_value()) {
        ANUNodeStyleSetAlignItems(node, config.align_items.value());
    } else {
        if (config.alignment == Alignment::Center) {
            ANUNodeStyleSetAlignItems(node, static_cast<ANUAlign>(Align::Center));
        } else if (config.alignment == Alignment::TopLeft) {
            ANUNodeStyleSetAlignItems(node, static_cast<ANUAlign>(Align::Start));
        } else if (config.alignment == Alignment::BottomRight) {
            ANUNodeStyleSetAlignItems(node, static_cast<ANUAlign>(Align::End));
        }
    }
}

} // namespace vaura
