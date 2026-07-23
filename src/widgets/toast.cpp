/// @file toast.cpp
/// @brief Toast widget implementation.

#include "vaura/widgets/toast.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Anu.h>
#include <chrono>

namespace vaura {

class ToastWidget : public StatefulWidget {
public:
    ToastConfig config;
    explicit ToastWidget(ToastConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Toast"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ToastState : public State {
public:
    // 0 = invisible, 1 = fully visible
    AnimatedValue<float> fade_anim_{0.0f, 1.0f, std::chrono::milliseconds{200}};
    std::unique_ptr<Ticker> ticker_;
    bool dismissed_ = false;
    std::chrono::steady_clock::time_point shown_at_;

    void initState() override {
        State::initState();
        fade_anim_.addListener([this]() { setState([]{}); });
        ticker_ = createTicker([this]() {
            fade_anim_.tick();
            auto* tw = static_cast<const ToastWidget*>(widget());
            if (!dismissed_ && tw->config.duration_ms > 0) {
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                    std::chrono::steady_clock::now() - shown_at_).count();
                if (elapsed >= tw->config.duration_ms) {
                    dismissed_ = true;
                    fade_anim_.reverse();
                    if (tw->config.on_dismissed) tw->config.on_dismissed();
                }
            }
        });
        ticker_->start();
        fade_anim_.forward();
        shown_at_ = std::chrono::steady_clock::now();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        fade_anim_.dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* tw = static_cast<const ToastWidget*>(widget());
        const auto& cfg = tw->config;

        float t = fade_anim_.get();

        auto pill = container({
            .color         = cfg.background_color,
            .padding       = EdgeInsets{cfg.padding_v, cfg.padding_h, cfg.padding_v, cfg.padding_h},
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .child         = text(cfg.message, {
                .font_size = cfg.font_size,
                .color     = cfg.text_color,
                .align     = TextStyle::Center,
            }),
        });

        auto faded_pill = opacity(t, pill);

        // Positioning
        float edge_offset = 32.0f;
        auto wrapper = std::make_shared<FlexBox>();
        wrapper->positionType(PositionType::Absolute)
                .position(Edge::Left, 0.0f)
                .widthPercent(100)
                .align(Align::Center);

        switch (cfg.position) {
            case ToastConfig::Position::Top:
                wrapper->position(Edge::Top, edge_offset);
                break;
            case ToastConfig::Position::Center:
                wrapper->position(Edge::Top, 0.0f)
                        .heightPercent(100)
                        .justify(Justify::Center);
                break;
            case ToastConfig::Position::Bottom:
                wrapper->position(Edge::Bottom, edge_offset);
                break;
        }

        wrapper->child(faded_pill);
        return wrapper;
    }
};

std::unique_ptr<State> ToastWidget::createState() {
    return std::make_unique<ToastState>();
}

std::shared_ptr<Widget> toast(ToastConfig config) {
    return std::make_shared<ToastWidget>(std::move(config));
}

} // namespace vaura
