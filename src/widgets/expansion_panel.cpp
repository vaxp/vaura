/// @file expansion_panel.cpp
/// @brief ExpansionPanel widget implementation.

#include "vaura/widgets/expansion_panel.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>
#include <vector>

namespace vaura {

class ExpansionPanelWidget : public StatefulWidget {
public:
    ExpansionPanelConfig config;
    explicit ExpansionPanelWidget(ExpansionPanelConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "ExpansionPanel"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ExpansionPanelState : public State {
public:
    std::vector<AnimatedValue<float>*> anims_;
    std::unique_ptr<Ticker> ticker_;
    std::vector<bool> open_states_;
    bool initialized_ = false;

    void initState() override {
        State::initState();
        auto* ew = static_cast<const ExpansionPanelWidget*>(widget());
        const auto& items = ew->config.items;
        open_states_.resize(items.size(), false);

        for (int i = 0; i < (int)items.size(); ++i) {
            auto* av = new AnimatedValue<float>(0.0f, 1.0f, std::chrono::milliseconds{220});
            if (items[i].is_expanded) {
                av->controller().setValue(1.0f);
                open_states_[i] = true;
            }
            av->addListener([this]() { setState([]{}); });
            anims_.push_back(av);
        }

        ticker_ = createTicker([this]() {
            for (auto* av : anims_) av->tick();
        });
        ticker_->start();
        initialized_ = true;
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        for (auto* av : anims_) { av->dispose(); delete av; }
        anims_.clear();
        State::dispose();
    }

    void toggle(int idx, const ExpansionPanelConfig& cfg) {
        bool new_state = !open_states_[idx];

        if (!cfg.allow_multiple_expanded && new_state) {
            // Close all others
            for (int i = 0; i < (int)open_states_.size(); ++i) {
                if (i != idx && open_states_[i]) {
                    open_states_[i] = false;
                    anims_[i]->reverse();
                    if (cfg.on_expansion_changed) cfg.on_expansion_changed(i, false);
                }
            }
        }

        open_states_[idx] = new_state;
        if (new_state) anims_[idx]->forward();
        else           anims_[idx]->reverse();
        if (cfg.on_expansion_changed) cfg.on_expansion_changed(idx, new_state);
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* ew = static_cast<const ExpansionPanelWidget*>(widget());
        const auto& cfg = ew->config;

        if (!initialized_ || anims_.size() != cfg.items.size()) return std::make_shared<FlexBox>();

        auto root_col = std::make_shared<FlexBox>();
        root_col->flexDirection(YGFlexDirectionColumn);

        for (int i = 0; i < (int)cfg.items.size(); ++i) {
            const auto& item = cfg.items[i];
            float t = anims_[i]->get();
            bool is_open = open_states_[i];

            // Radius: top rounded always, bottom rounded only when closed
            float top_r    = (i == 0) ? cfg.border_radius : 0.0f;
            float bottom_r = (i == (int)cfg.items.size() - 1 && !is_open) ? cfg.border_radius : 0.0f;

            // ── Header ────────────────────────────────────────
            auto hdr_row = std::make_shared<FlexBox>();
            hdr_row->flexDirection(YGFlexDirectionRow)
                    .alignItems(YGAlignCenter)
                    .justifyContent(YGJustifySpaceBetween)
                    .height(cfg.header_height)
                    .padding(YGEdgeLeft, 16)
                    .padding(YGEdgeRight, 12)
                    .backgroundColor(cfg.header_color);

            auto text_col = std::make_shared<FlexBox>();
            text_col->flexDirection(YGFlexDirectionColumn).flexGrow(1.0f);
            text_col->child(text(item.title, {
                .font_size = cfg.title_font_size,
                .color     = cfg.title_color,
                .weight    = is_open ? TextStyle::SemiBold : TextStyle::Regular,
            }));
            if (!item.subtitle.empty()) {
                text_col->child(text(item.subtitle, {
                    .font_size = cfg.subtitle_font_size,
                    .color     = cfg.subtitle_color,
                }));
            }
            hdr_row->child(text_col);

            // Chevron rotates 180° when open
            float chevron_rot = t * 180.0f;
            auto chev = icon(Icons::ChevronDown, {
                .size  = 22.0f,
                .color = is_open ? cfg.active_icon_color : cfg.icon_color,
            });
            auto chev_wrap = std::make_shared<FlexBox>();
            chev_wrap->width(28).height(28)
                      .justifyContent(YGJustifyCenter)
                      .alignItems(YGAlignCenter);
            if (chevron_rot > 0.5f) {
                chev_wrap->child(rotate(chevron_rot, chev));
            } else {
                chev_wrap->child(chev);
            }
            hdr_row->child(chev_wrap);

            int ii = i;
            auto hdr_gd = gesture_detector({
                .child  = hdr_row,
                .on_tap = [this, ii, cfg]() { toggle(ii, cfg); },
            });

            // ── Content (animated) ─────────────────────────────
            auto panel_col = std::make_shared<FlexBox>();
            panel_col->flexDirection(YGFlexDirectionColumn)
                      .backgroundColor(is_open ? cfg.expanded_bg : cfg.background_color);
            panel_col->child(hdr_gd);

            if (t > 0.005f && item.content) {
                auto content_wrap = std::make_shared<FlexBox>();
                content_wrap->padding(YGEdgeAll, 16)
                             .widthPercent(100);
                content_wrap->child(item.content);

                auto faded_content = opacity(t, content_wrap);
                panel_col->child(faded_content);
            }

            // Divider between panels
            if (i < (int)cfg.items.size() - 1) {
                auto div = std::make_shared<FlexBox>();
                div->height(1).widthPercent(100).backgroundColor(cfg.border_color);
                panel_col->child(div);
            }

            root_col->child(panel_col);
        }

        auto outer = container({
            .color         = cfg.background_color,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{cfg.border_color, 1.0f},
            .clip_content  = true,
            .child         = root_col,
        });

        return outer;
    }
};

std::unique_ptr<State> ExpansionPanelWidget::createState() {
    return std::make_unique<ExpansionPanelState>();
}

std::shared_ptr<Widget> expansion_panel(ExpansionPanelConfig config) {
    return std::make_shared<ExpansionPanelWidget>(std::move(config));
}

} // namespace vaura
