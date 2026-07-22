#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/paint.hpp"
#include <cmath>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderFlexBox
// ════════════════════════════════════════════════════════════════

void RenderFlexBox::setWidth(float width) {
    YGNodeStyleSetWidth(yogaNode(), width);
}

void RenderFlexBox::setWidthPercent(float percent) {
    YGNodeStyleSetWidthPercent(yogaNode(), percent);
}

void RenderFlexBox::setWidthAuto() {
    YGNodeStyleSetWidthAuto(yogaNode());
}

void RenderFlexBox::setHeight(float height) {
    YGNodeStyleSetHeight(yogaNode(), height);
}

void RenderFlexBox::setHeightPercent(float percent) {
    YGNodeStyleSetHeightPercent(yogaNode(), percent);
}

void RenderFlexBox::setHeightAuto() {
    YGNodeStyleSetHeightAuto(yogaNode());
}

void RenderFlexBox::setFlexDirection(YGFlexDirection direction) {
    YGNodeStyleSetFlexDirection(yogaNode(), direction);
}

void RenderFlexBox::setJustifyContent(YGJustify justify) {
    YGNodeStyleSetJustifyContent(yogaNode(), justify);
}

void RenderFlexBox::setAlignItems(YGAlign align) {
    YGNodeStyleSetAlignItems(yogaNode(), align);
}

void RenderFlexBox::setFlexWrap(YGWrap wrap) {
    YGNodeStyleSetFlexWrap(yogaNode(), wrap);
}

void RenderFlexBox::setFlex(float flex) {
    YGNodeStyleSetFlex(yogaNode(), flex);
}

void RenderFlexBox::setFlexGrow(float grow) {
    YGNodeStyleSetFlexGrow(yogaNode(), grow);
}

void RenderFlexBox::setFlexShrink(float shrink) {
    YGNodeStyleSetFlexShrink(yogaNode(), shrink);
}

void RenderFlexBox::setDirection(YGDirection direction) {
    YGNodeStyleSetDirection(yogaNode(), direction);
}

void RenderFlexBox::setAlignContent(YGAlign align) {
    YGNodeStyleSetAlignContent(yogaNode(), align);
}

void RenderFlexBox::setAlignSelf(YGAlign align) {
    YGNodeStyleSetAlignSelf(yogaNode(), align);
}

void RenderFlexBox::setFlexBasis(float basis) {
    if (std::isnan(basis)) {
        YGNodeStyleSetFlexBasisAuto(yogaNode());
    } else {
        YGNodeStyleSetFlexBasis(yogaNode(), basis);
    }
}

void RenderFlexBox::setFlexBasisPercent(float percent) {
    YGNodeStyleSetFlexBasisPercent(yogaNode(), percent);
}

void RenderFlexBox::setFlexBasisAuto() {
    YGNodeStyleSetFlexBasisAuto(yogaNode());
}

void RenderFlexBox::setAspectRatio(float ratio) {
    YGNodeStyleSetAspectRatio(yogaNode(), ratio);
}

void RenderFlexBox::setGap(YGGutter gutter, float gapLength) {
    YGNodeStyleSetGap(yogaNode(), gutter, gapLength);
}

void RenderFlexBox::setGapPercent(YGGutter gutter, float percent) {
    YGNodeStyleSetGapPercent(yogaNode(), gutter, percent);
}

void RenderFlexBox::setDisplay(YGDisplay display) {
    YGNodeStyleSetDisplay(yogaNode(), display);
}

void RenderFlexBox::setOverflow(YGOverflow overflow) {
    YGNodeStyleSetOverflow(yogaNode(), overflow);
}

void RenderFlexBox::setBorder(YGEdge edge, float border) {
    YGNodeStyleSetBorder(yogaNode(), edge, border);
}

void RenderFlexBox::setBoxSizing(YGBoxSizing sizing) {
    YGNodeStyleSetBoxSizing(yogaNode(), sizing);
}

void RenderFlexBox::setMinWidth(float width) {
    YGNodeStyleSetMinWidth(yogaNode(), width);
}

void RenderFlexBox::setMinWidthPercent(float percent) {
    YGNodeStyleSetMinWidthPercent(yogaNode(), percent);
}

void RenderFlexBox::setMinHeight(float height) {
    YGNodeStyleSetMinHeight(yogaNode(), height);
}

void RenderFlexBox::setMinHeightPercent(float percent) {
    YGNodeStyleSetMinHeightPercent(yogaNode(), percent);
}

void RenderFlexBox::setMaxWidth(float width) {
    YGNodeStyleSetMaxWidth(yogaNode(), width);
}

void RenderFlexBox::setMaxWidthPercent(float percent) {
    YGNodeStyleSetMaxWidthPercent(yogaNode(), percent);
}

void RenderFlexBox::setMaxHeight(float height) {
    YGNodeStyleSetMaxHeight(yogaNode(), height);
}

void RenderFlexBox::setMaxHeightPercent(float percent) {
    YGNodeStyleSetMaxHeightPercent(yogaNode(), percent);
}

void RenderFlexBox::setPadding(YGEdge edge, float padding) {
    YGNodeStyleSetPadding(yogaNode(), edge, padding);
}

void RenderFlexBox::setPaddingPercent(YGEdge edge, float percent) {
    YGNodeStyleSetPaddingPercent(yogaNode(), edge, percent);
}

void RenderFlexBox::setMargin(YGEdge edge, float margin) {
    YGNodeStyleSetMargin(yogaNode(), edge, margin);
}

void RenderFlexBox::setMarginPercent(YGEdge edge, float percent) {
    YGNodeStyleSetMarginPercent(yogaNode(), edge, percent);
}

void RenderFlexBox::setMarginAuto(YGEdge edge) {
    YGNodeStyleSetMarginAuto(yogaNode(), edge);
}

void RenderFlexBox::setPositionType(YGPositionType type) {
    YGNodeStyleSetPositionType(yogaNode(), type);
}

void RenderFlexBox::setPosition(YGEdge edge, float position) {
    YGNodeStyleSetPosition(yogaNode(), edge, position);
}

void RenderFlexBox::setPositionPercent(YGEdge edge, float percent) {
    YGNodeStyleSetPositionPercent(yogaNode(), edge, percent);
}

void RenderFlexBox::setPositionAuto(YGEdge edge) {
    YGNodeStyleSetPositionAuto(yogaNode(), edge);
}

void RenderFlexBox::setBackgroundColor(Color color) {
    if (background_color_ != color) {
        background_color_ = color;
        markNeedsPaint();
    }
}

void RenderFlexBox::setBorderRadius(float radius) {
    if (border_radius_ != radius) {
        border_radius_ = radius;
        markNeedsPaint();
    }
}

void RenderFlexBox::paint(PaintContext& context) {
    Canvas& canvas = context.canvas;
    Rect bounds = Rect::fromPointSize(context.offset, size_);

    // Draw background if not fully transparent
    if ((background_color_ & 0xFF000000) != 0) {
        Paint bg_paint;
        
        // Apply context opacity
        float effective_opacity = context.opacity;
        uint8_t alpha = static_cast<uint8_t>(((background_color_ >> 24) & 0xFF) * effective_opacity);
        Color final_color = (static_cast<uint32_t>(alpha) << 24) | (background_color_ & 0x00FFFFFF);
        
        bg_paint.setColor(final_color);
        bg_paint.setStyle(PaintStyle::Fill);
        
        if (border_radius_ > 0) {
            canvas.drawRRect(bounds, BorderRadius::circular(border_radius_), bg_paint);
        } else {
            canvas.drawRect(bounds, bg_paint);
        }
    }

    // If we have border radius, we should ideally clip children.
    // For now, if we have rounded corners, we'll clip the canvas context.
    bool needs_clip = border_radius_ > 0;
    if (needs_clip) {
        canvas.save();
        canvas.clipRRect(bounds, BorderRadius::circular(border_radius_));
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

bool RenderFlexBox::hitTestChildren(HitTestResult& result, Point localPoint) {
    // Test children in reverse order (last child is topmost)
    for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
        RenderObject* child = *it;
        if (!child) continue;

        Point child_local = {
            localPoint.x - child->offset().x,
            localPoint.y - child->offset().y
        };

        if (child->hitTest(result, child_local)) {
            return true;
        }
    }
    return false;
}

// ════════════════════════════════════════════════════════════════
// FlexBox Widget
// ════════════════════════════════════════════════════════════════

std::unique_ptr<RenderObject> FlexBox::createRenderObject(BuildContext& context) {
    auto render_box = std::make_unique<RenderFlexBox>();
    updateRenderObject(context, *render_box);
    return render_box;
}

void FlexBox::updateRenderObject(BuildContext& context, RenderObject& renderObject) {
    auto* flex_box = static_cast<RenderFlexBox*>(&renderObject);

    if (width_auto_) {
        flex_box->setWidthAuto();
    } else if (!std::isnan(width_)) {
        flex_box->setWidth(width_);
    } else if (!std::isnan(width_pct_)) {
        flex_box->setWidthPercent(width_pct_);
    }
    
    if (height_auto_) {
        flex_box->setHeightAuto();
    } else if (!std::isnan(height_)) {
        flex_box->setHeight(height_);
    } else if (!std::isnan(height_pct_)) {
        flex_box->setHeightPercent(height_pct_);
    }

    flex_box->setFlexDirection(flex_direction_);
    flex_box->setDirection(direction_);
    flex_box->setJustifyContent(justify_content_);
    flex_box->setAlignItems(align_items_);
    flex_box->setAlignContent(align_content_);
    flex_box->setAlignSelf(align_self_);
    flex_box->setFlexWrap(flex_wrap_);
    if (!std::isnan(flex_)) flex_box->setFlex(flex_);
    flex_box->setFlexGrow(flex_grow_);
    flex_box->setFlexShrink(flex_shrink_);
    
    if (flex_basis_auto_) {
        flex_box->setFlexBasisAuto();
    } else if (!std::isnan(flex_basis_pct_)) {
        flex_box->setFlexBasisPercent(flex_basis_pct_);
    } else {
        flex_box->setFlexBasis(flex_basis_);
    }

    flex_box->setAspectRatio(aspect_ratio_);
    flex_box->setDisplay(display_);
    flex_box->setOverflow(overflow_);
    flex_box->setBoxSizing(box_sizing_);

    flex_box->setMinWidth(min_width_);
    if (!std::isnan(min_width_pct_)) flex_box->setMinWidthPercent(min_width_pct_);
    flex_box->setMinHeight(min_height_);
    if (!std::isnan(min_height_pct_)) flex_box->setMinHeightPercent(min_height_pct_);
    flex_box->setMaxWidth(max_width_);
    if (!std::isnan(max_width_pct_)) flex_box->setMaxWidthPercent(max_width_pct_);
    flex_box->setMaxHeight(max_height_);
    if (!std::isnan(max_height_pct_)) flex_box->setMaxHeightPercent(max_height_pct_);

    for (const auto& p : padding_) flex_box->setPadding(p.first, p.second);
    for (const auto& p : padding_pct_) flex_box->setPaddingPercent(p.first, p.second);
    for (const auto& m : margin_) flex_box->setMargin(m.first, m.second);
    for (const auto& m : margin_pct_) flex_box->setMarginPercent(m.first, m.second);
    for (const auto& m : margin_auto_) flex_box->setMarginAuto(m);
    
    for (const auto& g : gaps_) flex_box->setGap(g.first, g.second);
    for (const auto& g : gap_pct_) flex_box->setGapPercent(g.first, g.second);
    for (const auto& b : borders_) flex_box->setBorder(b.first, b.second);

    flex_box->setPositionType(position_type_);
    for (const auto& p : position_) flex_box->setPosition(p.first, p.second);
    for (const auto& p : position_pct_) flex_box->setPositionPercent(p.first, p.second);
    for (const auto& p : position_auto_) flex_box->setPositionAuto(p);

    flex_box->setBackgroundColor(background_color_);
    flex_box->setBorderRadius(border_radius_);
    
    // Changing layout properties requires layout pass
    flex_box->markNeedsLayout();
}

} // namespace vaura
