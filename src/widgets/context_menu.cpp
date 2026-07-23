/// @file context_menu.cpp
/// @brief ContextMenu widget implementation — uses OverlayController.
///
/// Strategy:
///   - Wraps `child` in a GestureDetector that listens for right-click.
///   - The right-click position received is already in screen/window coordinates.
///   - On right-click: inserts an overlay with:
///       * Invisible full-screen backdrop (closes menu on outside click)
///       * Menu panel positioned at the exact click coordinates
///   - On item selection or backdrop click: removes the overlay.

#include "vaura/widgets/context_menu.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/overlay/popup_window_controller.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Anu.h>

namespace vaura {

class ContextMenuWidget : public StatefulWidget {
public:
    ContextMenuConfig config;
    explicit ContextMenuWidget(ContextMenuConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "ContextMenu"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ContextMenuState : public State {
public:
    int     hovered_idx = -1;
    bool    has_popup_  = false;
    PopupId popup_id_   = 0;
    Point   click_pos_  = {0, 0};

    AnimatedValue<float> open_anim_{0.0f, 1.0f, std::chrono::milliseconds{120}};
    std::unique_ptr<Ticker> ticker_;

    void refreshPopup(const ContextMenuConfig& cfg) {
        if (has_popup_) {
            PopupWindowController::instance().close(popup_id_);
            // Build a fresh popup at the same position
            popup_id_ = PopupWindowController::instance().open(
                buildPopupConfig(cfg, click_pos_),
                buildMenuWidget(cfg));
        }
    }

    void initState() override {
        State::initState();
        open_anim_.addListener([this]() {
            if (auto* cw = static_cast<const ContextMenuWidget*>(widget())) {
                refreshPopup(cw->config);
            }
        });
        ticker_ = createTicker([this]() { open_anim_.tick(); });
        ticker_->start();
    }

    void dispose() override {
        closePopup();
        if (ticker_) ticker_->stop();
        open_anim_.dispose();
        State::dispose();
    }

    // Compute an appropriate popup size based on item count
    PopupWindowConfig buildPopupConfig(const ContextMenuConfig& cfg, Point screen_pos) {
        // Estimate height: sum of item heights + separators + vertical padding
        int estimated_height = 8; // top + bottom padding
        for (auto& item : cfg.items) {
            if (item.is_separator) {
                estimated_height += 9; // 1px + 4px margin * 2
            } else {
                estimated_height += static_cast<int>(cfg.item_height);
            }
        }
        return PopupWindowConfig{
            .x     = static_cast<int>(screen_pos.x),
            .y     = static_cast<int>(screen_pos.y),
            .width = static_cast<int>(cfg.min_width > 0 ? cfg.min_width : 200),
            .height= estimated_height,
            .clear_color = cfg.background_color,
        };
    }

    // Build the menu widget — just the panel itself, no backdrop or absolute positioning
    WidgetPtr buildMenuWidget(const ContextMenuConfig& cfg) {
        auto panel_col = std::make_shared<FlexBox>();
        panel_col->direction(FlexDirection::Column)
                  .padding(Edge::Vertical, 4);

        int idx = 0;
        for (auto& item : cfg.items) {
            if (item.is_separator) {
                auto sep = std::make_shared<FlexBox>();
                sep->height(1)
                    .margin(Edge::Vertical, 4)
                    .backgroundColor(cfg.separator_color);
                panel_col->child(sep);
                ++idx;
                continue;
            }

            bool is_hov = (hovered_idx == idx);
            Color item_bg  = is_hov ? cfg.hover_color : 0x00000000;
            Color text_col = !item.enabled   ? cfg.disabled_color
                           : item.is_danger  ? cfg.danger_color
                           :                   cfg.text_color;

            auto item_row = std::make_shared<FlexBox>();
            item_row->direction(FlexDirection::Row)
                     .align(Align::Center)
                     .height(cfg.item_height)
                     .padding(Edge::Left, 12)
                     .padding(Edge::Right, 16)
                     .backgroundColor(item_bg);

            if (item.icon) {
                auto icon_wrap = std::make_shared<FlexBox>();
                icon_wrap->margin(Edge::Right, 10)
                          .width(18).height(18)
                          .justify(Justify::Center)
                          .align(Align::Center)
                          .child(item.icon);
                item_row->child(icon_wrap);
            }

            item_row->child(text(item.label, {.font_size = cfg.font_size, .color = text_col}));

            int captured_idx = idx;
            auto fn = item.on_tap;
            bool enabled = item.enabled;
            auto item_gd = gesture_detector({
                .child          = item_row,
                .on_tap         = [this, fn, enabled]() {
                    if (!enabled) return;
                    close();
                    if (fn) fn();
                },
                .on_hover_enter = [this, captured_idx, cfg]() {
                    hovered_idx = captured_idx;
                    refreshPopup(cfg);
                },
                .on_hover_exit  = [this, cfg]() {
                    hovered_idx = -1;
                    refreshPopup(cfg);
                },
            });

            panel_col->child(item_gd);
            ++idx;
        }

        auto panel = container({
            .color         = cfg.background_color,
            .width         = cfg.min_width > 0 ? (float)cfg.min_width : 200.0f,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{cfg.border_color, 1.0f},
            .child         = panel_col,
        });

        return opacity(open_anim_.get(), panel);
    }

    void openAt(Point screen_pos, const ContextMenuConfig& cfg) {
        hovered_idx = -1;
        click_pos_ = screen_pos;
        closePopup(); // close any existing
        open_anim_.reset();
        open_anim_.forward();
        popup_id_ = PopupWindowController::instance().open(
            buildPopupConfig(cfg, screen_pos),
            buildMenuWidget(cfg));
        has_popup_ = (popup_id_ != 0);
    }

    void close() {
        closePopup();
        setState([]{});
    }

    void closePopup() {
        if (has_popup_) {
            PopupWindowController::instance().close(popup_id_);
            has_popup_ = false;
            popup_id_ = 0;
        }
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* cw = static_cast<const ContextMenuWidget*>(widget());
        const auto& cfg = cw->config;

        // Simply wrap the child with a right-click detector.
        // All menu rendering happens in the overlay layer.
        return gesture_detector({
            .child          = cfg.child,
            .on_right_click = [this, cfg](Point pos) {
                openAt(pos, cfg);
            },
        });
    }
};

std::unique_ptr<State> ContextMenuWidget::createState() {
    return std::make_unique<ContextMenuState>();
}

std::shared_ptr<Widget> context_menu(ContextMenuConfig config) {
    return std::make_shared<ContextMenuWidget>(std::move(config));
}

}  // namespace vaura
