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
    
    void setFlexDirection(ANUFlexDirection direction);
    void setJustifyContent(ANUJustify justify);
    void setAlignItems(ANUAlign align);
    void setFlexWrap(ANUWrap wrap);
    void setDirection(ANUDirection direction);
    void setAlignContent(ANUAlign align);
    void setAlignSelf(ANUAlign align);
    void setFlex(float flex);
    void setFlexGrow(float grow);
    void setFlexShrink(float shrink);
    void setFlexBasis(float basis);
    void setFlexBasisPercent(float percent);
    void setFlexBasisAuto();
    void setAspectRatio(float ratio);
    void setGap(ANUGutter gutter, float gapLength);
    void setGapPercent(ANUGutter gutter, float percent);
    void setDisplay(ANUDisplay display);
    void setOverflow(ANUOverflow overflow);
    void setBorder(ANUEdge edge, float border);
    void setBoxSizing(ANUBoxSizing sizing);

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
    
    void setPadding(ANUEdge edge, float padding);
    void setPaddingPercent(ANUEdge edge, float percent);
    void setMargin(ANUEdge edge, float margin);
    void setMarginPercent(ANUEdge edge, float percent);
    void setMarginAuto(ANUEdge edge);
    
    void setPositionType(ANUPositionType type);
    void setPosition(ANUEdge edge, float position);
    void setPositionPercent(ANUEdge edge, float percent);
    void setPositionAuto(ANUEdge edge);
    
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
    
    FlexBox& direction(FlexDirection dir) { flex_direction_ = static_cast<ANUFlexDirection>(dir); return *this; }
    FlexBox& justify(Justify justify) { justify_content_ = static_cast<ANUJustify>(justify); return *this; }
    FlexBox& align(Align align) { align_items_ = static_cast<ANUAlign>(align); return *this; }
    FlexBox& wrap(FlexWrap wrap) { flex_wrap_ = static_cast<ANUWrap>(wrap); return *this; }
    FlexBox& direction(ANUDirection dir) { direction_ = dir; return *this; }
    FlexBox& alignContent(Align align) { align_content_ = static_cast<ANUAlign>(align); return *this; }
    FlexBox& alignSelf(Align align) { align_self_ = static_cast<ANUAlign>(align); return *this; }
    FlexBox& flex(float f) { flex_ = f; return *this; }
    FlexBox& flexGrow(float grow) { flex_grow_ = grow; return *this; }
    FlexBox& flexShrink(float shrink) { flex_shrink_ = shrink; return *this; }
    FlexBox& flexBasis(float basis) { flex_basis_ = basis; return *this; }
    FlexBox& flexBasisPercent(float pct) { flex_basis_pct_ = pct; return *this; }
    FlexBox& flexBasisAuto() { flex_basis_auto_ = true; return *this; }
    FlexBox& aspectRatio(float ratio) { aspect_ratio_ = ratio; return *this; }
    FlexBox& gap(ANUGutter gutter, float gapLength) { gaps_.push_back({gutter, gapLength}); return *this; }
    FlexBox& gapPercent(ANUGutter gutter, float percent) { gap_pct_.push_back({gutter, percent}); return *this; }
    FlexBox& display(ANUDisplay display) { display_ = display; return *this; }
    FlexBox& overflow(Overflow overflow) { overflow_ = static_cast<ANUOverflow>(overflow); return *this; }
    FlexBox& border(Edge edge, float border) { borders_.push_back({static_cast<ANUEdge>(edge), border}); return *this; }
    FlexBox& boxSizing(ANUBoxSizing sizing) { box_sizing_ = sizing; return *this; }

    FlexBox& minWidth(float w) { min_width_ = w; return *this; }
    FlexBox& minWidthPercent(float w) { min_width_pct_ = w; return *this; }
    FlexBox& minHeight(float h) { min_height_ = h; return *this; }
    FlexBox& minHeightPercent(float h) { min_height_pct_ = h; return *this; }
    FlexBox& maxWidth(float w) { max_width_ = w; return *this; }
    FlexBox& maxWidthPercent(float w) { max_width_pct_ = w; return *this; }
    FlexBox& maxHeight(float h) { max_height_ = h; return *this; }
    FlexBox& maxHeightPercent(float h) { max_height_pct_ = h; return *this; }
    
    FlexBox& padding(Edge edge, float p) { padding_.push_back({static_cast<ANUEdge>(edge), p}); return *this; }
    FlexBox& paddingPercent(Edge edge, float p) { padding_pct_.push_back({static_cast<ANUEdge>(edge), p}); return *this; }
    FlexBox& margin(Edge edge, float m) { margin_.push_back({static_cast<ANUEdge>(edge), m}); return *this; }
    FlexBox& marginPercent(Edge edge, float m) { margin_pct_.push_back({static_cast<ANUEdge>(edge), m}); return *this; }
    FlexBox& marginAuto(Edge edge) { margin_auto_.push_back(static_cast<ANUEdge>(edge)); return *this; }
    
    FlexBox& positionType(PositionType type) { position_type_ = static_cast<ANUPositionType>(type); return *this; }
    FlexBox& position(Edge edge, float p) { position_.push_back({static_cast<ANUEdge>(edge), p}); return *this; }
    FlexBox& positionPercent(Edge edge, float p) { position_pct_.push_back({static_cast<ANUEdge>(edge), p}); return *this; }
    FlexBox& positionAuto(Edge edge) { position_auto_.push_back(static_cast<ANUEdge>(edge)); return *this; }
    
    FlexBox& backgroundColor(Color color) { background_color_ = color; return *this; }
    FlexBox& borderRadius(float r) { border_radius_ = r; return *this; }

    FlexBox& child(WidgetPtr c) { children.push_back(std::move(c)); return *this; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& context) override;
    void updateRenderObject(BuildContext& context, RenderObject& renderObject) override;

    [[nodiscard]] std::string_view typeName() const override { return "FlexBox"; }

private:
    float width_ = ANUUndefined;
    float width_pct_ = ANUUndefined;
    bool width_auto_ = false;
    float height_ = ANUUndefined;
    float height_pct_ = ANUUndefined;
    bool height_auto_ = false;
    
    ANUFlexDirection flex_direction_ = ANUFlexDirectionColumn;
    ANUDirection direction_ = ANUDirectionInherit;
    ANUJustify justify_content_ = ANUJustifyFlexStart;
    ANUAlign align_items_ = ANUAlignStretch;
    ANUAlign align_content_ = ANUAlignFlexStart;
    ANUAlign align_self_ = ANUAlignAuto;
    ANUWrap flex_wrap_ = ANUWrapNoWrap;
    float flex_ = ANUUndefined;
    float flex_grow_ = 0.0f;
    float flex_shrink_ = 0.0f;
    float flex_basis_ = ANUUndefined;
    float flex_basis_pct_ = ANUUndefined;
    bool flex_basis_auto_ = false;
    float aspect_ratio_ = ANUUndefined;
    ANUDisplay display_ = ANUDisplayFlex;
    ANUOverflow overflow_ = ANUOverflowVisible;
    ANUBoxSizing box_sizing_ = ANUBoxSizingBorderBox;

    std::vector<std::pair<ANUGutter, float>> gaps_;
    std::vector<std::pair<ANUGutter, float>> gap_pct_;
    std::vector<std::pair<ANUEdge, float>> borders_;

    float min_width_ = ANUUndefined;
    float min_width_pct_ = ANUUndefined;
    float min_height_ = ANUUndefined;
    float min_height_pct_ = ANUUndefined;
    float max_width_ = ANUUndefined;
    float max_width_pct_ = ANUUndefined;
    float max_height_ = ANUUndefined;
    float max_height_pct_ = ANUUndefined;
    
    std::vector<std::pair<ANUEdge, float>> padding_;
    std::vector<std::pair<ANUEdge, float>> padding_pct_;
    std::vector<std::pair<ANUEdge, float>> margin_;
    std::vector<std::pair<ANUEdge, float>> margin_pct_;
    std::vector<ANUEdge> margin_auto_;
    
    ANUPositionType position_type_ = ANUPositionTypeRelative;
    std::vector<std::pair<ANUEdge, float>> position_;
    std::vector<std::pair<ANUEdge, float>> position_pct_;
    std::vector<ANUEdge> position_auto_;
    
    Color background_color_ = 0x00000000;
    float border_radius_ = 0.0f;
};


struct FlexBoxConfig {
    FlexDirection direction = FlexDirection::Column;
    Justify justify = Justify::Start;
    Align align = Align::Stretch;
    Align alignContent = Align::Start;
    Align alignSelf = Align::Auto;
    FlexWrap wrap = FlexWrap::NoWrap;
    float gap = ANUUndefined;
    
    float width = ANUUndefined;
    float widthPercent = ANUUndefined;
    float height = ANUUndefined;
    float heightPercent = ANUUndefined;
    
    float flex = ANUUndefined;
    float flexGrow = 0.0f;
    float flexShrink = 0.0f;
    float flexBasis = ANUUndefined;
    float flexBasisPercent = ANUUndefined;

    std::optional<std::pair<Edge, float>> padding = std::nullopt;
    std::optional<std::pair<Edge, float>> margin = std::nullopt;
    std::optional<std::pair<Edge, float>> position = std::nullopt;
    PositionType positionType = PositionType::Relative;

    Color backgroundColor = 0x00000000;
    float borderRadius = 0.0f;
    Overflow overflow = Overflow::Visible;
    
    std::vector<std::shared_ptr<Widget>> children;
};

inline std::shared_ptr<FlexBox> createFlexBox(const FlexBoxConfig& cfg) {
    auto fb = std::make_shared<FlexBox>();
    fb->direction(cfg.direction);
    fb->justify(cfg.justify);
    fb->align(cfg.align);
    fb->alignContent(cfg.alignContent);
    fb->alignSelf(cfg.alignSelf);
    fb->wrap(cfg.wrap);
    
    if (cfg.gap != ANUUndefined) fb->gap(ANUGutterAll, cfg.gap);
    
    if (cfg.width != ANUUndefined) fb->width(cfg.width);
    if (cfg.widthPercent != ANUUndefined) fb->widthPercent(cfg.widthPercent);
    if (cfg.height != ANUUndefined) fb->height(cfg.height);
    if (cfg.heightPercent != ANUUndefined) fb->heightPercent(cfg.heightPercent);
    
    if (cfg.flex != ANUUndefined) fb->flex(cfg.flex);
    if (cfg.flexGrow != 0.0f) fb->flexGrow(cfg.flexGrow);
    if (cfg.flexShrink != 0.0f) fb->flexShrink(cfg.flexShrink);
    if (cfg.flexBasis != ANUUndefined) fb->flexBasis(cfg.flexBasis);
    if (cfg.flexBasisPercent != ANUUndefined) fb->flexBasisPercent(cfg.flexBasisPercent);

    if (cfg.padding) fb->padding(cfg.padding->first, cfg.padding->second);
    if (cfg.margin) fb->margin(cfg.margin->first, cfg.margin->second);
    
    fb->positionType(cfg.positionType);
    if (cfg.position) fb->position(cfg.position->first, cfg.position->second);
    
    fb->backgroundColor(cfg.backgroundColor);
    if (cfg.borderRadius > 0.0f) fb->borderRadius(cfg.borderRadius);
    fb->overflow(cfg.overflow);
    
    for (auto& child : cfg.children) {
        if (child) fb->child(child);
    }
    
    return fb;
}

inline std::shared_ptr<FlexBox> Column(FlexBoxConfig cfg = {}) {
    cfg.direction = FlexDirection::Column;
    return createFlexBox(cfg);
}

inline std::shared_ptr<FlexBox> Row(FlexBoxConfig cfg = {}) {
    cfg.direction = FlexDirection::Row;
    return createFlexBox(cfg);
}

} // namespace vaura
