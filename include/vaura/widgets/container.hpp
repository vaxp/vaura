#pragma once
/// @file container.hpp
/// @brief Container widget — the fundamental decorated box.
///
/// Container is the most versatile layout primitive. It combines:
///   - Background color / gradient
///   - Padding and margin
///   - Border and border radius
///   - Shadow
///   - Size constraints
///   - A single optional child
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/tree/build_context.hpp"
#include <optional>
#include <memory>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// Shadow
// ════════════════════════════════════════════════════════════════

/// @brief Shadow configuration for visual depth.
struct Shadow {
    Color color = 0x40000000;      ///< Shadow color (semi-transparent black).
    float blur_radius = 4.0f;      ///< Blur radius in pixels.
    Point offset = {0, 2};         ///< Shadow offset.
    float spread_radius = 0.0f;    ///< Spread radius (expand/contract shadow).

    constexpr bool operator==(const Shadow&) const = default;
};

// ════════════════════════════════════════════════════════════════
// Gradient
// ════════════════════════════════════════════════════════════════

/// @brief Gradient configuration for background fills.
struct Gradient {
    enum Type { Linear, Radial, Sweep } type = Linear;
    std::vector<Color> colors;              ///< Gradient color stops.
    std::vector<float> stops;               ///< Position of each stop [0..1].
    Point begin = {0, 0};                   ///< Start point (for Linear/Radial).
    Point end = {1, 1};                     ///< End point (for Linear).
    float radius = 0.5f;                    ///< Radius (for Radial).
};

// ════════════════════════════════════════════════════════════════
// ContainerConfig — declarative configuration
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Container widget.
///
/// Uses C++20 designated initializers for a clean, declarative syntax:
/// @code
///   container({
///       .color = Colors::Blue,
///       .width = 200,
///       .height = 100,
///       .padding = EdgeInsets::all(16),
///       .border_radius = BorderRadius::circular(12),
///       .child = text("Hello"),
///   });
/// @endcode
struct ContainerConfig {
    Color color = Colors::Transparent;        ///< Background color.
    float width = 0;                          ///< Explicit width (0 = auto).
    float height = 0;                         ///< Explicit height (0 = auto).
    std::optional<float> width_percent;       ///< Width as a percentage (e.g., 100.0f).
    std::optional<float> height_percent;      ///< Height as a percentage (e.g., 100.0f).
    bool width_auto = false;                  ///< Explicitly auto width.
    bool height_auto = false;                 ///< Explicitly auto height.
    float min_width = 0;                      ///< Minimum width.
    float min_height = 0;                     ///< Minimum height.
    std::optional<float> min_width_percent;
    std::optional<float> min_height_percent;
    float max_width = 0;                      ///< Maximum width (0 = unconstrained).
    float max_height = 0;                     ///< Maximum height (0 = unconstrained).
    std::optional<float> max_width_percent;
    std::optional<float> max_height_percent;
    EdgeInsets padding;                        ///< Inner padding.
    std::optional<EdgeInsets> padding_percent;
    EdgeInsets margin;                         ///< Outer margin.
    std::optional<EdgeInsets> margin_percent;
    bool margin_auto = false;                  ///< Auto margin for all edges (if true) or specific edge via YGNodeStyleSetMarginAuto.
    BorderRadius border_radius;               ///< Corner radii.
    Border border;                            ///< Border style.
    std::optional<Shadow> shadow;             ///< Box shadow.
    std::optional<Gradient> gradient;         ///< Background gradient.
    float opacity = 1.0f;                     ///< Opacity [0..1].
    float flex_shrink = 0.0f;                 ///< Flex shrink factor (0 = do not shrink).
    float flex_grow = 0.0f;                   ///< Flex grow factor.
    float flex_basis = YGUndefined;           ///< Flex basis.
    std::optional<float> flex_basis_percent;
    bool flex_basis_auto = false;
    float aspect_ratio = YGUndefined;         ///< Aspect ratio.
    float gap = YGUndefined;                  ///< Gap between all items.
    float row_gap = YGUndefined;              ///< Gap between rows.
    float column_gap = YGUndefined;           ///< Gap between columns.
    std::optional<float> gap_percent;
    std::optional<float> row_gap_percent;
    std::optional<float> column_gap_percent;
    YGDirection direction = YGDirectionInherit; ///< Layout direction (LTR/RTL).
    YGDisplay display = YGDisplayFlex;        ///< Display property.
    YGOverflow overflow = YGOverflowVisible;  ///< Overflow property.
    YGBoxSizing box_sizing = YGBoxSizingBorderBox; ///< Box sizing.
    YGPositionType position_type = YGPositionTypeRelative; ///< Position type.
    float position_top = YGUndefined;         ///< Top position.
    float position_bottom = YGUndefined;      ///< Bottom position.
    float position_left = YGUndefined;        ///< Left position.
    float position_right = YGUndefined;       ///< Right position.
    float position_start = YGUndefined;       ///< Start position (RTL-aware).
    float position_end = YGUndefined;         ///< End position (RTL-aware).
    bool position_auto = false;               ///< Position auto.
    std::optional<float> position_top_percent;
    std::optional<float> position_bottom_percent;
    std::optional<float> position_left_percent;
    std::optional<float> position_right_percent;
    std::optional<float> position_start_percent;
    std::optional<float> position_end_percent;
    YGAlign align_content = YGAlignFlexStart; ///< Cross-axis alignment of multiple lines.
    YGAlign align_self = YGAlignAuto;         ///< Self alignment override.
    std::optional<YGFlexDirection> flex_direction; ///< Flex direction override.
    std::optional<YGJustify> justify_content; ///< Justify content override.
    std::optional<YGAlign> align_items;       ///< Align items override.
    std::optional<YGWrap> flex_wrap;          ///< Flex wrap override.
    bool clip_content = false;                ///< Clip children to bounds.
    Alignment alignment = Alignment::Center;  ///< Child alignment.
    WidgetPtr child;                          ///< Optional child widget.
};

// ════════════════════════════════════════════════════════════════
// ContainerWidget
// ════════════════════════════════════════════════════════════════

/// @brief A decorated box widget with optional child.
class ContainerWidget : public SingleChildRenderObjectWidget {
public:
    ContainerConfig config;

    explicit ContainerWidget(ContainerConfig cfg)
        : config(std::move(cfg)) {
        this->child = config.child;
    }

    [[nodiscard]] std::string_view typeName() const override {
        return "Container";
    }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(
        BuildContext& ctx) override;

    void updateRenderObject(BuildContext& ctx,
                           RenderObject& renderObject) override;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Container widget.
inline WidgetPtr container(ContainerConfig config) {
    return std::make_shared<ContainerWidget>(std::move(config));
}

}  // namespace vaura
