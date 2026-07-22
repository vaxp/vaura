#include "vaura/widgets/container.hpp"
#include "vaura/rendering/canvas.hpp"
#include <layout_engine/Yoga.h>
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

    auto node = box.yogaNode();

    // Sizing
    if (config.width_auto) {
        YGNodeStyleSetWidthAuto(node);
    } else if (config.width_percent.has_value()) {
        YGNodeStyleSetWidthPercent(node, config.width_percent.value());
    } else if (config.width > 0) {
        YGNodeStyleSetWidth(node, config.width);
    } else {
        YGNodeStyleSetWidth(node, YGUndefined);
    }

    if (config.height_auto) {
        YGNodeStyleSetHeightAuto(node);
    } else if (config.height_percent.has_value()) {
        YGNodeStyleSetHeightPercent(node, config.height_percent.value());
    } else if (config.height > 0) {
        YGNodeStyleSetHeight(node, config.height);
    } else {
        YGNodeStyleSetHeight(node, YGUndefined);
    }

    if (config.min_width_percent.has_value()) {
        YGNodeStyleSetMinWidthPercent(node, config.min_width_percent.value());
    } else if (config.min_width > 0) {
        YGNodeStyleSetMinWidth(node, config.min_width);
    } else {
        YGNodeStyleSetMinWidth(node, YGUndefined);
    }

    if (config.min_height_percent.has_value()) {
        YGNodeStyleSetMinHeightPercent(node, config.min_height_percent.value());
    } else if (config.min_height > 0) {
        YGNodeStyleSetMinHeight(node, config.min_height);
    } else {
        YGNodeStyleSetMinHeight(node, YGUndefined);
    }

    if (config.max_width_percent.has_value()) {
        YGNodeStyleSetMaxWidthPercent(node, config.max_width_percent.value());
    } else if (config.max_width > 0) {
        YGNodeStyleSetMaxWidth(node, config.max_width);
    } else {
        YGNodeStyleSetMaxWidth(node, YGUndefined);
    }

    if (config.max_height_percent.has_value()) {
        YGNodeStyleSetMaxHeightPercent(node, config.max_height_percent.value());
    } else if (config.max_height > 0) {
        YGNodeStyleSetMaxHeight(node, config.max_height);
    } else {
        YGNodeStyleSetMaxHeight(node, YGUndefined);
    }

    YGNodeStyleSetFlexShrink(node, config.flex_shrink);
    YGNodeStyleSetFlexGrow(node, config.flex_grow);

    if (config.flex_basis_auto) {
        YGNodeStyleSetFlexBasisAuto(node);
    } else if (config.flex_basis_percent.has_value()) {
        YGNodeStyleSetFlexBasisPercent(node, config.flex_basis_percent.value());
    } else {
        YGNodeStyleSetFlexBasis(node, config.flex_basis);
    }

    YGNodeStyleSetAspectRatio(node, config.aspect_ratio);
    YGNodeStyleSetDirection(node, config.direction);
    YGNodeStyleSetAlignContent(node, config.align_content);
    YGNodeStyleSetAlignSelf(node, config.align_self);
    YGNodeStyleSetDisplay(node, config.display);
    YGNodeStyleSetOverflow(node, config.overflow);
    YGNodeStyleSetBoxSizing(node, config.box_sizing);

    YGNodeStyleSetPositionType(node, config.position_type);
    if (config.position_auto) {
        YGNodeStyleSetPositionAuto(node, YGEdgeAll);
    } else {
        if (config.position_top_percent.has_value()) {
            YGNodeStyleSetPositionPercent(node, YGEdgeTop, config.position_top_percent.value());
        } else {
            YGNodeStyleSetPosition(node, YGEdgeTop, config.position_top);
        }
        
        if (config.position_bottom_percent.has_value()) {
            YGNodeStyleSetPositionPercent(node, YGEdgeBottom, config.position_bottom_percent.value());
        } else {
            YGNodeStyleSetPosition(node, YGEdgeBottom, config.position_bottom);
        }
        
        if (config.position_left_percent.has_value()) {
            YGNodeStyleSetPositionPercent(node, YGEdgeLeft, config.position_left_percent.value());
        } else {
            YGNodeStyleSetPosition(node, YGEdgeLeft, config.position_left);
        }
        
        if (config.position_right_percent.has_value()) {
            YGNodeStyleSetPositionPercent(node, YGEdgeRight, config.position_right_percent.value());
        } else {
            YGNodeStyleSetPosition(node, YGEdgeRight, config.position_right);
        }
        
        if (config.position_start_percent.has_value()) {
            YGNodeStyleSetPositionPercent(node, YGEdgeStart, config.position_start_percent.value());
        } else {
            YGNodeStyleSetPosition(node, YGEdgeStart, config.position_start);
        }
        
        if (config.position_end_percent.has_value()) {
            YGNodeStyleSetPositionPercent(node, YGEdgeEnd, config.position_end_percent.value());
        } else {
            YGNodeStyleSetPosition(node, YGEdgeEnd, config.position_end);
        }
    }

    if (config.gap_percent.has_value()) {
        YGNodeStyleSetGapPercent(node, YGGutterAll, config.gap_percent.value());
    } else if (!std::isnan(config.gap)) {
        YGNodeStyleSetGap(node, YGGutterAll, config.gap);
    }
    
    if (config.row_gap_percent.has_value()) {
        YGNodeStyleSetGapPercent(node, YGGutterRow, config.row_gap_percent.value());
    } else if (!std::isnan(config.row_gap)) {
        YGNodeStyleSetGap(node, YGGutterRow, config.row_gap);
    }
    
    if (config.column_gap_percent.has_value()) {
        YGNodeStyleSetGapPercent(node, YGGutterColumn, config.column_gap_percent.value());
    } else if (!std::isnan(config.column_gap)) {
        YGNodeStyleSetGap(node, YGGutterColumn, config.column_gap);
    }

    if (config.border.width > 0) {
        YGNodeStyleSetBorder(node, YGEdgeAll, config.border.width);
    } else {
        YGNodeStyleSetBorder(node, YGEdgeAll, YGUndefined);
    }

    // Padding
    if (config.padding_percent.has_value()) {
        auto& p_pct = config.padding_percent.value();
        YGNodeStyleSetPaddingPercent(node, YGEdgeLeft, p_pct.left);
        YGNodeStyleSetPaddingPercent(node, YGEdgeTop, p_pct.top);
        YGNodeStyleSetPaddingPercent(node, YGEdgeRight, p_pct.right);
        YGNodeStyleSetPaddingPercent(node, YGEdgeBottom, p_pct.bottom);
        YGNodeStyleSetPaddingPercent(node, YGEdgeStart, p_pct.start);
        YGNodeStyleSetPaddingPercent(node, YGEdgeEnd, p_pct.end);
    } else {
        YGNodeStyleSetPadding(node, YGEdgeLeft, config.padding.left);
        YGNodeStyleSetPadding(node, YGEdgeTop, config.padding.top);
        YGNodeStyleSetPadding(node, YGEdgeRight, config.padding.right);
        YGNodeStyleSetPadding(node, YGEdgeBottom, config.padding.bottom);
        YGNodeStyleSetPadding(node, YGEdgeStart, config.padding.start);
        YGNodeStyleSetPadding(node, YGEdgeEnd, config.padding.end);
    }

    // Margin
    if (config.margin_auto) {
        YGNodeStyleSetMarginAuto(node, YGEdgeAll);
    } else if (config.margin_percent.has_value()) {
        auto& m_pct = config.margin_percent.value();
        YGNodeStyleSetMarginPercent(node, YGEdgeLeft, m_pct.left);
        YGNodeStyleSetMarginPercent(node, YGEdgeTop, m_pct.top);
        YGNodeStyleSetMarginPercent(node, YGEdgeRight, m_pct.right);
        YGNodeStyleSetMarginPercent(node, YGEdgeBottom, m_pct.bottom);
        YGNodeStyleSetMarginPercent(node, YGEdgeStart, m_pct.start);
        YGNodeStyleSetMarginPercent(node, YGEdgeEnd, m_pct.end);
    } else {
        YGNodeStyleSetMargin(node, YGEdgeLeft, config.margin.left);
        YGNodeStyleSetMargin(node, YGEdgeTop, config.margin.top);
        YGNodeStyleSetMargin(node, YGEdgeRight, config.margin.right);
        YGNodeStyleSetMargin(node, YGEdgeBottom, config.margin.bottom);
        YGNodeStyleSetMargin(node, YGEdgeStart, config.margin.start);
        YGNodeStyleSetMargin(node, YGEdgeEnd, config.margin.end);
    }

    // Alignment and Flex Properties
    if (config.flex_direction.has_value()) {
        YGNodeStyleSetFlexDirection(node, config.flex_direction.value());
    } else {
        YGNodeStyleSetFlexDirection(node, YGFlexDirectionColumn);
    }

    if (config.flex_wrap.has_value()) {
        YGNodeStyleSetFlexWrap(node, config.flex_wrap.value());
    }

    if (config.justify_content.has_value()) {
        YGNodeStyleSetJustifyContent(node, config.justify_content.value());
    } else {
        if (config.alignment == Alignment::Center) {
            YGNodeStyleSetJustifyContent(node, YGJustifyCenter);
        } else if (config.alignment == Alignment::TopLeft) {
            YGNodeStyleSetJustifyContent(node, YGJustifyFlexStart);
        } else if (config.alignment == Alignment::BottomRight) {
            YGNodeStyleSetJustifyContent(node, YGJustifyFlexEnd);
        }
    }
    
    if (config.align_items.has_value()) {
        YGNodeStyleSetAlignItems(node, config.align_items.value());
    } else {
        if (config.alignment == Alignment::Center) {
            YGNodeStyleSetAlignItems(node, YGAlignCenter);
        } else if (config.alignment == Alignment::TopLeft) {
            YGNodeStyleSetAlignItems(node, YGAlignFlexStart);
        } else if (config.alignment == Alignment::BottomRight) {
            YGNodeStyleSetAlignItems(node, YGAlignFlexEnd);
        }
    }
}

} // namespace vaura
