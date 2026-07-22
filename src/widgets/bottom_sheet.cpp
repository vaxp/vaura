/// @file bottom_sheet.cpp
/// @brief BottomSheet widget implementation — uses OverlayController.
///
/// Strategy:
///   - When is_open becomes true: inserts a full-screen overlay containing:
///       * Semi-transparent animated backdrop
///       * Sheet panel sliding up from the bottom
///   - When is_open becomes false: removes the overlay.
///   - The overlay content is a full-screen FlexBox stack.

#include "vaura/widgets/bottom_sheet.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/overlay/overlay.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>
#include <cmath>

namespace vaura {

class BottomSheetWidget : public StatefulWidget {
public:
    BottomSheetConfig config;
    explicit BottomSheetWidget(BottomSheetConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "BottomSheet"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class BottomSheetState : public State {
public:
    bool current_open = false;
    // anim: 0 = fully closed, 1 = fully open
    AnimatedValue<float> slide_anim_{0.0f, 1.0f, std::chrono::milliseconds{300}};
    std::unique_ptr<Ticker> ticker_;

    bool      has_overlay_ = false;
    OverlayId overlay_id_  = 0;

    void initState() override {
        State::initState();
        if (auto* bw = static_cast<const BottomSheetWidget*>(widget())) {
            current_open = bw->config.is_open;
        }
        if (current_open) {
            slide_anim_.controller().setValue(1.0f);
        }
        slide_anim_.addListener([this]() {
            // Rebuild overlay content each frame during animation
            if (has_overlay_) {
                refreshOverlay();
            }
            // When animation completes while closing, remove overlay
            if (!current_open && slide_anim_.get() < 0.01f && has_overlay_) {
                removeOverlay();
            }
        });
        ticker_ = createTicker([this]() { slide_anim_.tick(); });
        ticker_->start();

        if (current_open) {
            insertOverlay();
        }
    }

    void dispose() override {
        removeOverlay();
        if (ticker_) ticker_->stop();
        slide_anim_.dispose();
        State::dispose();
    }

    // ── Overlay management ───────────────────────────────────────

    WidgetPtr buildOverlayContent() {
        auto* bw = static_cast<const BottomSheetWidget*>(widget());
        if (!bw) return std::make_shared<FlexBox>();
        const auto& cfg = bw->config;

        float t = slide_anim_.get();

        // ── Backdrop ──────────────────────────────────────────
        uint8_t bd_alpha = static_cast<uint8_t>(((cfg.backdrop_color >> 24) & 0xFF) * t);
        Color   faded_bd = (cfg.backdrop_color & 0x00FFFFFF) | (static_cast<Color>(bd_alpha) << 24);

        auto backdrop_box = std::make_shared<FlexBox>();
        backdrop_box->positionType(YGPositionTypeAbsolute)
                     .position(YGEdgeAll, 0.0f)
                     .widthPercent(100)
                     .heightPercent(100)
                     .backgroundColor(faded_bd);

        WidgetPtr backdrop_widget = backdrop_box;
        if (cfg.dismiss_on_backdrop && cfg.on_close) {
            backdrop_widget = gesture_detector({
                .child  = backdrop_box,
                .on_tap = cfg.on_close,
            });
        }

        // ── Sheet Panel ───────────────────────────────────────
        auto panel_col = std::make_shared<FlexBox>();
        panel_col->flexDirection(YGFlexDirectionColumn)
                  .widthPercent(100)
                  .alignItems(YGAlignCenter);

        if (cfg.show_handle) {
            auto handle = std::make_shared<FlexBox>();
            handle->width(40).height(4)
                   .margin(YGEdgeTop, 10)
                   .margin(YGEdgeBottom, 8)
                   .backgroundColor(cfg.handle_color)
                   .borderRadius(2.0f);
            panel_col->child(handle);
        }

        if (cfg.child) {
            auto content_wrap = std::make_shared<FlexBox>();
            content_wrap->widthPercent(100)
                          .padding(YGEdgeLeft, 16)
                          .padding(YGEdgeRight, 16)
                          .padding(YGEdgeBottom, 24)
                          .child(cfg.child);
            panel_col->child(content_wrap);
        }

        auto sheet_container = container({
            .color         = cfg.background_color,
            .width_percent = 100.0f,
            .border_radius = BorderRadius{cfg.border_radius, cfg.border_radius, 0.0f, 0.0f},
            .shadow        = Shadow{0x50000000, 24.0f, {0, -4}},
            .child         = panel_col,
        });

        // Slide transform: translateY from +sheetHeight to 0
        float sheet_height_approx = 400.0f * (cfg.max_height_percent / 100.0f);
        float translate_y = (1.0f - t) * sheet_height_approx;

        auto transformed_sheet = transform({
            .translate_y = translate_y,
            .child       = sheet_container,
        });

        auto sheet_wrapper = std::make_shared<FlexBox>();
        sheet_wrapper->positionType(YGPositionTypeAbsolute)
                      .position(YGEdgeBottom, 0.0f)
                      .position(YGEdgeLeft, 0.0f)
                      .widthPercent(100);
        sheet_wrapper->child(transformed_sheet);

        // Full-screen overlay stack
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100);
        root->child(backdrop_widget);
        root->child(sheet_wrapper);

        return root;
    }

    void insertOverlay() {
        if (has_overlay_) return;
        overlay_id_ = OverlayController::instance().insert({
            .content = buildOverlayContent(),
            .absorbs_pointer = true,
        });
        has_overlay_ = true;
    }

    void refreshOverlay() {
        // Re-insert with fresh content to update animation frame
        if (has_overlay_) {
            OverlayController::instance().remove(overlay_id_);
            overlay_id_ = OverlayController::instance().insert({
                .content = buildOverlayContent(),
                .absorbs_pointer = true,
            });
        }
    }

    void removeOverlay() {
        if (has_overlay_) {
            OverlayController::instance().remove(overlay_id_);
            has_overlay_ = false;
            overlay_id_ = 0;
        }
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* bw = static_cast<const BottomSheetWidget*>(widget());
        const auto& cfg = bw->config;

        // React to external is_open changes
        if (cfg.is_open != current_open) {
            current_open = cfg.is_open;
            if (current_open) {
                slide_anim_.forward();
                insertOverlay();
            } else {
                slide_anim_.reverse();
                // Overlay removed by animation listener when complete
            }
        }

        // The widget itself renders nothing — it only manages the overlay.
        // Return an empty transparent box (zero size is fine here).
        auto empty = std::make_shared<FlexBox>();
        return empty;
    }
};

std::unique_ptr<State> BottomSheetWidget::createState() {
    return std::make_unique<BottomSheetState>();
}

std::shared_ptr<Widget> bottom_sheet(BottomSheetConfig config) {
    return std::make_shared<BottomSheetWidget>(std::move(config));
}

}  // namespace vaura
