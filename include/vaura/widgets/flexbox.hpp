#pragma once

#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"
#include <string>
#include <vector>

namespace vaura {

class RenderFlexBox : public RenderBox {
public:
    RenderFlexBox() = default;
    ~RenderFlexBox() override = default;

    void setWidth(float width);
    void setWidthPercent(float percent);
    void setHeight(float height);
    void setHeightPercent(float percent);
    
    void setFlexDirection(YGFlexDirection direction);
    void setJustifyContent(YGJustify justify);
    void setAlignItems(YGAlign align);
    void setFlexWrap(YGWrap wrap);
    void setDirection(YGDirection direction);
    void setAlignContent(YGAlign align);
    void setAlignSelf(YGAlign align);
    void setFlex(float flex);
    void setFlexGrow(float grow);
    void setFlexShrink(float shrink);
    void setFlexBasis(float basis);
    void setFlexBasisPercent(float percent);
    void setFlexBasisAuto();
    void setAspectRatio(float ratio);
    void setGap(YGGutter gutter, float gapLength);
    void setGapPercent(YGGutter gutter, float percent);
    void setDisplay(YGDisplay display);
    void setOverflow(YGOverflow overflow);
    void setBorder(YGEdge edge, float border);
    void setBoxSizing(YGBoxSizing sizing);

    void setMinWidth(float width);
    void setMinWidthPercent(float percent);
    void setMinHeight(float height);
    void setMinHeightPercent(float percent);
    void setMaxWidth(float width);
    void setMaxWidthPercent(float percent);
    void setMaxHeight(float height);
    void setMaxHeightPercent(float percent);
    void setWidthAuto();
    void setHeightAuto();
    
    void setPadding(YGEdge edge, float padding);
    void setPaddingPercent(YGEdge edge, float percent);
    void setMargin(YGEdge edge, float margin);
    void setMarginPercent(YGEdge edge, float percent);
    void setMarginAuto(YGEdge edge);
    
    void setPositionType(YGPositionType type);
    void setPosition(YGEdge edge, float position);
    void setPositionPercent(YGEdge edge, float percent);
    void setPositionAuto(YGEdge edge);
    
    void setBackgroundColor(Color color);
    void setBorderRadius(float radius);

    void paint(PaintContext& context) override;
    bool hitTestChildren(HitTestResult& result, Point localPoint) override;

private:
    Color background_color_ = 0x00000000;
    float border_radius_ = 0.0f;
};

class FlexBox : public MultiChildRenderObjectWidget {
public:
    FlexBox(std::vector<std::shared_ptr<Widget>> children = {})
        : MultiChildRenderObjectWidget(Key(), std::move(children)) {}

    FlexBox& width(float w) { width_ = w; return *this; }
    FlexBox& widthPercent(float w) { width_pct_ = w; return *this; }
    FlexBox& widthAuto() { width_auto_ = true; return *this; }
    FlexBox& height(float h) { height_ = h; return *this; }
    FlexBox& heightPercent(float h) { height_pct_ = h; return *this; }
    FlexBox& heightAuto() { height_auto_ = true; return *this; }
    
    FlexBox& flexDirection(YGFlexDirection dir) { flex_direction_ = dir; return *this; }
    FlexBox& justifyContent(YGJustify justify) { justify_content_ = justify; return *this; }
    FlexBox& alignItems(YGAlign align) { align_items_ = align; return *this; }
    FlexBox& flexWrap(YGWrap wrap) { flex_wrap_ = wrap; return *this; }
    FlexBox& direction(YGDirection dir) { direction_ = dir; return *this; }
    FlexBox& alignContent(YGAlign align) { align_content_ = align; return *this; }
    FlexBox& alignSelf(YGAlign align) { align_self_ = align; return *this; }
    FlexBox& flex(float f) { flex_ = f; return *this; }
    FlexBox& flexGrow(float grow) { flex_grow_ = grow; return *this; }
    FlexBox& flexShrink(float shrink) { flex_shrink_ = shrink; return *this; }
    FlexBox& flexBasis(float basis) { flex_basis_ = basis; return *this; }
    FlexBox& flexBasisPercent(float pct) { flex_basis_pct_ = pct; return *this; }
    FlexBox& flexBasisAuto() { flex_basis_auto_ = true; return *this; }
    FlexBox& aspectRatio(float ratio) { aspect_ratio_ = ratio; return *this; }
    FlexBox& gap(YGGutter gutter, float gapLength) { gaps_.push_back({gutter, gapLength}); return *this; }
    FlexBox& gapPercent(YGGutter gutter, float percent) { gap_pct_.push_back({gutter, percent}); return *this; }
    FlexBox& display(YGDisplay display) { display_ = display; return *this; }
    FlexBox& overflow(YGOverflow overflow) { overflow_ = overflow; return *this; }
    FlexBox& border(YGEdge edge, float border) { borders_.push_back({edge, border}); return *this; }
    FlexBox& boxSizing(YGBoxSizing sizing) { box_sizing_ = sizing; return *this; }

    FlexBox& minWidth(float w) { min_width_ = w; return *this; }
    FlexBox& minWidthPercent(float w) { min_width_pct_ = w; return *this; }
    FlexBox& minHeight(float h) { min_height_ = h; return *this; }
    FlexBox& minHeightPercent(float h) { min_height_pct_ = h; return *this; }
    FlexBox& maxWidth(float w) { max_width_ = w; return *this; }
    FlexBox& maxWidthPercent(float w) { max_width_pct_ = w; return *this; }
    FlexBox& maxHeight(float h) { max_height_ = h; return *this; }
    FlexBox& maxHeightPercent(float h) { max_height_pct_ = h; return *this; }
    
    FlexBox& padding(YGEdge edge, float p) { padding_.push_back({edge, p}); return *this; }
    FlexBox& paddingPercent(YGEdge edge, float p) { padding_pct_.push_back({edge, p}); return *this; }
    FlexBox& margin(YGEdge edge, float m) { margin_.push_back({edge, m}); return *this; }
    FlexBox& marginPercent(YGEdge edge, float m) { margin_pct_.push_back({edge, m}); return *this; }
    FlexBox& marginAuto(YGEdge edge) { margin_auto_.push_back(edge); return *this; }
    
    FlexBox& positionType(YGPositionType type) { position_type_ = type; return *this; }
    FlexBox& position(YGEdge edge, float p) { position_.push_back({edge, p}); return *this; }
    FlexBox& positionPercent(YGEdge edge, float p) { position_pct_.push_back({edge, p}); return *this; }
    FlexBox& positionAuto(YGEdge edge) { position_auto_.push_back(edge); return *this; }
    
    FlexBox& backgroundColor(Color color) { background_color_ = color; return *this; }
    FlexBox& borderRadius(float r) { border_radius_ = r; return *this; }

    FlexBox& child(WidgetPtr c) { children.push_back(std::move(c)); return *this; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& context) override;
    void updateRenderObject(BuildContext& context, RenderObject& renderObject) override;

    [[nodiscard]] std::string_view typeName() const override { return "FlexBox"; }

private:
    float width_ = YGUndefined;
    float width_pct_ = YGUndefined;
    bool width_auto_ = false;
    float height_ = YGUndefined;
    float height_pct_ = YGUndefined;
    bool height_auto_ = false;
    
    YGFlexDirection flex_direction_ = YGFlexDirectionColumn;
    YGDirection direction_ = YGDirectionInherit;
    YGJustify justify_content_ = YGJustifyFlexStart;
    YGAlign align_items_ = YGAlignStretch;
    YGAlign align_content_ = YGAlignFlexStart;
    YGAlign align_self_ = YGAlignAuto;
    YGWrap flex_wrap_ = YGWrapNoWrap;
    float flex_ = YGUndefined;
    float flex_grow_ = 0.0f;
    float flex_shrink_ = 0.0f;
    float flex_basis_ = YGUndefined;
    float flex_basis_pct_ = YGUndefined;
    bool flex_basis_auto_ = false;
    float aspect_ratio_ = YGUndefined;
    YGDisplay display_ = YGDisplayFlex;
    YGOverflow overflow_ = YGOverflowVisible;
    YGBoxSizing box_sizing_ = YGBoxSizingBorderBox;

    std::vector<std::pair<YGGutter, float>> gaps_;
    std::vector<std::pair<YGGutter, float>> gap_pct_;
    std::vector<std::pair<YGEdge, float>> borders_;

    float min_width_ = YGUndefined;
    float min_width_pct_ = YGUndefined;
    float min_height_ = YGUndefined;
    float min_height_pct_ = YGUndefined;
    float max_width_ = YGUndefined;
    float max_width_pct_ = YGUndefined;
    float max_height_ = YGUndefined;
    float max_height_pct_ = YGUndefined;
    
    std::vector<std::pair<YGEdge, float>> padding_;
    std::vector<std::pair<YGEdge, float>> padding_pct_;
    std::vector<std::pair<YGEdge, float>> margin_;
    std::vector<std::pair<YGEdge, float>> margin_pct_;
    std::vector<YGEdge> margin_auto_;
    
    YGPositionType position_type_ = YGPositionTypeRelative;
    std::vector<std::pair<YGEdge, float>> position_;
    std::vector<std::pair<YGEdge, float>> position_pct_;
    std::vector<YGEdge> position_auto_;
    
    Color background_color_ = 0x00000000;
    float border_radius_ = 0.0f;
};

} // namespace vaura
