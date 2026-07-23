#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/paint.hpp"
#include <cmath>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderFlexBox
// ════════════════════════════════════════════════════════════════

void RenderFlexBox::setWidth(float width) {
    ANUNodeStyleSetWidth(anuNode(), width);
}

void RenderFlexBox::setWidthPercent(float percent) {
    ANUNodeStyleSetWidthPercent(anuNode(), percent);
}

void RenderFlexBox::setWidthAuto() {
    ANUNodeStyleSetWidthAuto(anuNode());
}

void RenderFlexBox::setHeight(float height) {
    ANUNodeStyleSetHeight(anuNode(), height);
}

void RenderFlexBox::setHeightPercent(float percent) {
    ANUNodeStyleSetHeightPercent(anuNode(), percent);
}

void RenderFlexBox::setHeightAuto() {
    ANUNodeStyleSetHeightAuto(anuNode());
}

void RenderFlexBox::setFlexDirection(ANUFlexDirection direction) {
    ANUNodeStyleSetFlexDirection(anuNode(), direction);
}

void RenderFlexBox::setJustifyContent(ANUJustify justify) {
    ANUNodeStyleSetJustifyContent(anuNode(), justify);
}

void RenderFlexBox::setAlignItems(ANUAlign align) {
    ANUNodeStyleSetAlignItems(anuNode(), align);
}

void RenderFlexBox::setFlexWrap(ANUWrap wrap) {
    ANUNodeStyleSetFlexWrap(anuNode(), wrap);
}

void RenderFlexBox::setFlex(float flex) {
    ANUNodeStyleSetFlex(anuNode(), flex);
}

void RenderFlexBox::setFlexGrow(float grow) {
    ANUNodeStyleSetFlexGrow(anuNode(), grow);
}

void RenderFlexBox::setFlexShrink(float shrink) {
    ANUNodeStyleSetFlexShrink(anuNode(), shrink);
}

void RenderFlexBox::setDirection(ANUDirection direction) {
    ANUNodeStyleSetDirection(anuNode(), direction);
}

void RenderFlexBox::setAlignContent(ANUAlign align) {
    ANUNodeStyleSetAlignContent(anuNode(), align);
}

void RenderFlexBox::setAlignSelf(ANUAlign align) {
    ANUNodeStyleSetAlignSelf(anuNode(), align);
}

void RenderFlexBox::setFlexBasis(float basis) {
    if (std::isnan(basis)) {
        ANUNodeStyleSetFlexBasisAuto(anuNode());
    } else {
        ANUNodeStyleSetFlexBasis(anuNode(), basis);
    }
}

void RenderFlexBox::setFlexBasisPercent(float percent) {
    ANUNodeStyleSetFlexBasisPercent(anuNode(), percent);
}

void RenderFlexBox::setFlexBasisAuto() {
    ANUNodeStyleSetFlexBasisAuto(anuNode());
}

void RenderFlexBox::setAspectRatio(float ratio) {
    ANUNodeStyleSetAspectRatio(anuNode(), ratio);
}

void RenderFlexBox::setGap(ANUGutter gutter, float gapLength) {
    ANUNodeStyleSetGap(anuNode(), gutter, gapLength);
}

void RenderFlexBox::setGapPercent(ANUGutter gutter, float percent) {
    ANUNodeStyleSetGapPercent(anuNode(), gutter, percent);
}

void RenderFlexBox::setDisplay(ANUDisplay display) {
    ANUNodeStyleSetDisplay(anuNode(), display);
}

void RenderFlexBox::setOverflow(ANUOverflow overflow) {
    ANUNodeStyleSetOverflow(anuNode(), overflow);
}

void RenderFlexBox::setBorder(ANUEdge edge, float border) {
    ANUNodeStyleSetBorder(anuNode(), edge, border);
}

void RenderFlexBox::setBoxSizing(ANUBoxSizing sizing) {
    ANUNodeStyleSetBoxSizing(anuNode(), sizing);
}

void RenderFlexBox::setMinWidth(float width) {
    ANUNodeStyleSetMinWidth(anuNode(), width);
}

void RenderFlexBox::setMinWidthPercent(float percent) {
    ANUNodeStyleSetMinWidthPercent(anuNode(), percent);
}

void RenderFlexBox::setMinHeight(float height) {
    ANUNodeStyleSetMinHeight(anuNode(), height);
}

void RenderFlexBox::setMinHeightPercent(float percent) {
    ANUNodeStyleSetMinHeightPercent(anuNode(), percent);
}

void RenderFlexBox::setMaxWidth(float width) {
    ANUNodeStyleSetMaxWidth(anuNode(), width);
}

void RenderFlexBox::setMaxWidthPercent(float percent) {
    ANUNodeStyleSetMaxWidthPercent(anuNode(), percent);
}

void RenderFlexBox::setMaxHeight(float height) {
    ANUNodeStyleSetMaxHeight(anuNode(), height);
}

void RenderFlexBox::setMaxHeightPercent(float percent) {
    ANUNodeStyleSetMaxHeightPercent(anuNode(), percent);
}

void RenderFlexBox::setPadding(ANUEdge edge, float padding) {
    ANUNodeStyleSetPadding(anuNode(), edge, padding);
}

void RenderFlexBox::setPaddingPercent(ANUEdge edge, float percent) {
    ANUNodeStyleSetPaddingPercent(anuNode(), edge, percent);
}

void RenderFlexBox::setMargin(ANUEdge edge, float margin) {
    ANUNodeStyleSetMargin(anuNode(), edge, margin);
}

void RenderFlexBox::setMarginPercent(ANUEdge edge, float percent) {
    ANUNodeStyleSetMarginPercent(anuNode(), edge, percent);
}

void RenderFlexBox::setMarginAuto(ANUEdge edge) {
    ANUNodeStyleSetMarginAuto(anuNode(), edge);
}

void RenderFlexBox::setPositionType(ANUPositionType type) {
    ANUNodeStyleSetPositionType(anuNode(), type);
}

void RenderFlexBox::setPosition(ANUEdge edge, float position) {
    ANUNodeStyleSetPosition(anuNode(), edge, position);
}

void RenderFlexBox::setPositionPercent(ANUEdge edge, float percent) {
    ANUNodeStyleSetPositionPercent(anuNode(), edge, percent);
}

void RenderFlexBox::setPositionAuto(ANUEdge edge) {
    ANUNodeStyleSetPositionAuto(anuNode(), edge);
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
