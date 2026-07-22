/// @file navigation_rail.cpp
/// @brief NavigationRail widget implementation.

#include "vaura/widgets/navigation_rail.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

class NavigationRailWidget : public StatefulWidget {
public:
    NavigationRailConfig config;
    explicit NavigationRailWidget(NavigationRailConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "NavigationRail"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class NavigationRailState : public State {
public:
    int hovered_idx = -1;

    WidgetPtr build(BuildContext& ctx) override {
        auto* nw = static_cast<const NavigationRailWidget*>(widget());
        const auto& cfg = nw->config;

        float rail_width = cfg.extended ? cfg.extended_width : cfg.collapsed_width;

        auto rail = std::make_shared<FlexBox>();
        rail->flexDirection(YGFlexDirectionColumn)
             .alignItems(YGAlignCenter)
             .width(rail_width)
             .heightPercent(100)
             .backgroundColor(cfg.background_color);

        // Header
        if (cfg.header) {
            auto hw = std::make_shared<FlexBox>();
            hw->padding(YGEdgeAll, 12).child(cfg.header);
            rail->child(hw);
        }

        // Items
        for (int i = 0; i < (int)cfg.items.size(); ++i) {
            const auto& item = cfg.items[i];
            bool is_active  = (i == cfg.selected_index);
            bool is_hovered = (hovered_idx == i);

            Color icon_color  = is_active ? cfg.active_icon_color  : cfg.inactive_icon_color;
            Color label_color = is_active ? cfg.active_label_color  : cfg.inactive_label_color;

            // Indicator background pill
            Color pill_bg = (is_active || is_hovered)
                ? (is_active ? cfg.indicator_color : cfg.indicator_color & 0x66FFFFFF)
                : 0x00000000;

            auto icon_wrap = std::make_shared<FlexBox>();
            icon_wrap->width(cfg.extended ? rail_width - 32.0f : cfg.collapsed_width - 16.0f)
                      .height(cfg.item_height * 0.65f)
                      .borderRadius(cfg.indicator_radius)
                      .justifyContent(YGJustifyCenter)
                      .alignItems(YGAlignCenter)
                      .backgroundColor(pill_bg);

            if (item.icon) icon_wrap->child(item.icon);

            auto tile_col = std::make_shared<FlexBox>();
            tile_col->flexDirection(YGFlexDirectionColumn)
                     .alignItems(YGAlignCenter)
                     .width(rail_width)
                     .height(cfg.item_height)
                     .justifyContent(YGJustifyCenter)
                     .padding(YGEdgeHorizontal, 8);

            tile_col->child(icon_wrap);

            // Label (always shown in extended mode, hidden otherwise unless active)
            if (cfg.extended && !item.label.empty()) {
                auto lbl = text(item.label, {
                    .font_size = cfg.label_font_size,
                    .color     = label_color,
                    .align     = TextStyle::Center,
                });
                auto lbl_wrap = std::make_shared<FlexBox>();
                lbl_wrap->margin(YGEdgeTop, 4).child(lbl);
                tile_col->child(lbl_wrap);
            }

            int ci = i;
            auto gd = gesture_detector({
                .child          = tile_col,
                .on_tap         = [this, ci, cfg]() {
                    if (cfg.on_changed) cfg.on_changed(ci);
                },
                .on_hover_enter = [this, ci]() { setState([this, ci]{ hovered_idx = ci; }); },
                .on_hover_exit  = [this]()      { setState([this]{ hovered_idx = -1; }); },
            });

            rail->child(gd);
        }

        // Footer
        if (cfg.footer) {
            auto spacer = std::make_shared<FlexBox>();
            spacer->flexGrow(1.0f);
            rail->child(spacer);
            auto fw = std::make_shared<FlexBox>();
            fw->padding(YGEdgeAll, 12).child(cfg.footer);
            rail->child(fw);
        }

        return rail;
    }
};

std::unique_ptr<State> NavigationRailWidget::createState() {
    return std::make_unique<NavigationRailState>();
}

std::shared_ptr<Widget> navigation_rail(NavigationRailConfig config) {
    return std::make_shared<NavigationRailWidget>(std::move(config));
}

} // namespace vaura
