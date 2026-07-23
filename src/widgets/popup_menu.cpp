/// @file popup_menu.cpp
/// @brief PopupMenu widget implementation.

#include "vaura/widgets/popup_menu.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Anu.h>

namespace vaura {

class PopupMenuWidget : public StatefulWidget {
public:
    PopupMenuConfig config;
    explicit PopupMenuWidget(PopupMenuConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "PopupMenu"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class PopupMenuState : public State {
public:
    bool  is_open     = false;
    int   hovered_idx = -1;
    AnimatedValue<float> open_anim_{0.0f, 1.0f, std::chrono::milliseconds{150}};
    std::unique_ptr<Ticker> ticker_;

    void initState() override {
        State::initState();
        open_anim_.addListener([this]() { setState([]{}); });
        ticker_ = createTicker([this]() { open_anim_.tick(); });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        open_anim_.dispose();
        State::dispose();
    }

    void open()  { is_open = true;  open_anim_.forward(); }
    void close() { is_open = false; hovered_idx = -1; open_anim_.reverse(); }

    WidgetPtr build(BuildContext& ctx) override {
        auto* pw = static_cast<const PopupMenuWidget*>(widget());
        const auto& cfg = pw->config;

        // Anchor wrapped with tap detector
        auto anchor_gd = gesture_detector({
            .child  = cfg.anchor,
            .on_tap = [this]() { is_open ? close() : open(); },
        });

        // If panel is invisible, just return anchor
        if (open_anim_.get() < 0.01f && !is_open) {
            return anchor_gd;
        }

        // ── Build panel ───────────────────────────────────────
        auto panel_col = std::make_shared<FlexBox>();
        panel_col->direction(FlexDirection::Column)
                  .padding(Edge::Vertical, 4);

        int idx = 0;
        for (auto& item : cfg.items) {
            if (item.is_separator) {
                auto sep = std::make_shared<FlexBox>();
                sep->height(1).margin(Edge::Vertical, 4).backgroundColor(cfg.separator_color);
                panel_col->child(sep);
                ++idx;
                continue;
            }

            bool is_hov  = (hovered_idx == idx);
            Color item_bg  = is_hov ? cfg.hover_color : 0x00000000;
            Color text_col = !item.enabled  ? cfg.disabled_color
                           : item.is_danger ? cfg.danger_color
                           :                  cfg.text_color;

            auto item_row = std::make_shared<FlexBox>();
            item_row->direction(FlexDirection::Row)
                     .align(Align::Center)
                     .height(cfg.item_height)
                     .padding(Edge::Left, 12)
                     .padding(Edge::Right, 16)
                     .backgroundColor(item_bg);

            if (item.icon) {
                auto iw = std::make_shared<FlexBox>();
                iw->margin(Edge::Right, 10).width(18).height(18)
                  .justify(Justify::Center).align(Align::Center)
                  .child(item.icon);
                item_row->child(iw);
            }

            item_row->child(text(item.label, {.font_size = cfg.font_size, .color = text_col}));

            int ci = idx;
            auto fn = item.on_tap;
            bool en = item.enabled;
            auto item_gd = gesture_detector({
                .child          = item_row,
                .on_tap         = [this, fn, en]() { if (!en) return; close(); if (fn) fn(); },
                .on_hover_enter = [this, ci]() { setState([this, ci]{ hovered_idx = ci; }); },
                .on_hover_exit  = [this]()     { setState([this]{ hovered_idx = -1; }); },
            });
            panel_col->child(item_gd);
            ++idx;
        }

        auto panel = container({
            .color         = cfg.background_color,
            .min_width     = cfg.min_width,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{cfg.border_color, 1.0f},
            .shadow        = Shadow{0x50000000, 16.0f, {0, 6}},
            .child         = panel_col,
        });

        auto faded_panel = opacity(open_anim_.get(), panel);

        // Offset based on placement
        float top_offset  = 0.0f;
        float left_offset = 0.0f;

        switch (cfg.placement) {
            case PopupMenuConfig::Placement::BelowLeft:
                top_offset  = cfg.anchor_height + 4.0f;
                left_offset = 0.0f;
                break;
            case PopupMenuConfig::Placement::BelowRight:
                top_offset  = cfg.anchor_height + 4.0f;
                left_offset = -(cfg.min_width); // simplified
                break;
            case PopupMenuConfig::Placement::AboveLeft:
                top_offset  = -(cfg.item_height * (float)cfg.items.size() + 8.0f + cfg.anchor_height);
                left_offset = 0.0f;
                break;
            case PopupMenuConfig::Placement::AboveRight:
                top_offset  = -(cfg.item_height * (float)cfg.items.size() + 8.0f + cfg.anchor_height);
                left_offset = -(cfg.min_width);
                break;
        }

        auto panel_wrapper = std::make_shared<FlexBox>();
        panel_wrapper->positionType(PositionType::Absolute)
                      .position(Edge::Top,  top_offset)
                      .position(Edge::Left, left_offset);
        panel_wrapper->child(faded_panel);

        // Backdrop to dismiss
        auto backdrop = std::make_shared<FlexBox>();
        backdrop->positionType(PositionType::Absolute)
                 .position(Edge::All, 0.0f)
                 .widthPercent(100)
                 .heightPercent(100);
        auto backdrop_gd = gesture_detector({
            .child  = backdrop,
            .on_tap = [this]() { close(); },
        });

        // Root
        auto root = std::make_shared<FlexBox>();
        root->direction(FlexDirection::Column);
        root->child(anchor_gd);
        root->child(backdrop_gd);
        root->child(panel_wrapper);

        return root;
    }
};

std::unique_ptr<State> PopupMenuWidget::createState() {
    return std::make_unique<PopupMenuState>();
}

std::shared_ptr<Widget> popup_menu(PopupMenuConfig config) {
    return std::make_shared<PopupMenuWidget>(std::move(config));
}

} // namespace vaura
