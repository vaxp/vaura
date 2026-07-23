/// @file snackbar.cpp
/// @brief Snackbar widget implementation.
///
/// The Snackbar:
///   - Slides up from the bottom (translateY animation, 0 → visible).
///   - After duration_ms, slides back down and calls on_dismissed.
///   - Has an optional action button.
///   - Renders as an absolutely-positioned bar at the bottom of its parent.

#include "vaura/widgets/snackbar.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Anu.h>
#include <chrono>

namespace vaura {

class SnackbarWidget : public StatefulWidget {
public:
    SnackbarConfig config;
    explicit SnackbarWidget(SnackbarConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Snackbar"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SnackbarState : public State {
public:
    // 0 = below screen, 1 = fully visible
    AnimatedValue<float> slide_anim_{0.0f, 1.0f, std::chrono::milliseconds{220}};
    std::unique_ptr<Ticker> ticker_;
    bool dismissed_ = false;
    std::chrono::steady_clock::time_point shown_at_;
    bool timer_started_ = false;

    void initState() override {
        State::initState();
        slide_anim_.addListener([this]() { setState([]{}); });
        ticker_ = createTicker([this]() {
            slide_anim_.tick();

            // Auto-dismiss timer
            auto* sw = static_cast<const SnackbarWidget*>(widget());
            if (!dismissed_ && timer_started_ && sw->config.duration_ms > 0) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - shown_at_).count();
                if (elapsed >= sw->config.duration_ms) {
                    dismiss();
                }
            }
        });
        ticker_->start();

        // Appear
        slide_anim_.forward();
        shown_at_      = std::chrono::steady_clock::now();
        timer_started_ = true;
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        slide_anim_.dispose();
        State::dispose();
    }

    void dismiss() {
        if (dismissed_) return;
        dismissed_     = true;
        timer_started_ = false;
        slide_anim_.reverse();
        auto* sw = static_cast<const SnackbarWidget*>(widget());
        if (sw->config.on_dismissed) sw->config.on_dismissed();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sw = static_cast<const SnackbarWidget*>(widget());
        const auto& cfg = sw->config;

        float t = slide_anim_.get();

        // Content row
        auto content_row = std::make_shared<FlexBox>();
        content_row->direction(FlexDirection::Row)
                    .align(Align::Center)
                    .justify(Justify::SpaceBetween)
                    .padding(Edge::Left, 16)
                    .padding(Edge::Right, cfg.action_label.empty() ? 16.0f : 8.0f)
                    .padding(Edge::Vertical, 14);

        content_row->child(text(cfg.message, {
            .font_size = cfg.font_size,
            .color     = cfg.text_color,
        }));

        if (!cfg.action_label.empty()) {
            auto action_btn = gesture_detector({
                .child  = container({
                    .padding = EdgeInsets{8, 12, 8, 12},
                    .child   = text(cfg.action_label, {
                        .font_size = cfg.font_size,
                        .color     = cfg.action_color,
                        .weight    = TextStyle::SemiBold,
                    }),
                }),
                .on_tap = [this, &cfg]() {
                    if (cfg.on_action) cfg.on_action();
                    dismiss();
                },
            });
            content_row->child(action_btn);
        }

        auto bar = container({
            .color         = cfg.background_color,
            .max_width     = cfg.max_width,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .shadow        = Shadow{0x50000000, 12.0f, {0, 4}},
            .child         = content_row,
        });

        // Slide from bottom: t=0 → fully below, t=1 → in position
        float translate_y = (1.0f - t) * 80.0f;

        auto transformed = transform({
            .translate_y = translate_y,
            .child       = bar,
        });

        // Center horizontally, stick to bottom
        auto wrapper = std::make_shared<FlexBox>();
        wrapper->positionType(PositionType::Absolute)
                .position(Edge::Bottom, 16.0f)
                .position(Edge::Left, 0.0f)
                .widthPercent(100)
                .align(Align::Center);
        wrapper->child(transformed);

        return wrapper;
    }
};

std::unique_ptr<State> SnackbarWidget::createState() {
    return std::make_unique<SnackbarState>();
}

std::shared_ptr<Widget> snackbar(SnackbarConfig config) {
    return std::make_shared<SnackbarWidget>(std::move(config));
}

} // namespace vaura
