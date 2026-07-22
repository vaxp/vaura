/// @file drawer.cpp
/// @brief Drawer widget implementation.

#include "vaura/widgets/drawer.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

class DrawerWidget : public StatefulWidget {
public:
    DrawerConfig config;
    explicit DrawerWidget(DrawerConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Drawer"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class DrawerState : public State {
public:
    bool current_open = false;
    // 0 = fully off-screen, 1 = fully visible
    AnimatedValue<float> slide_anim_{0.0f, 1.0f, std::chrono::milliseconds{280}};
    std::unique_ptr<Ticker> ticker_;

    void initState() override {
        State::initState();
        auto* dw = static_cast<const DrawerWidget*>(widget());
        current_open = dw->config.is_open;
        if (current_open) slide_anim_.controller().setValue(1.0f);
        slide_anim_.addListener([this]() { setState([]{}); });
        ticker_ = createTicker([this]() { slide_anim_.tick(); });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        slide_anim_.dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* dw = static_cast<const DrawerWidget*>(widget());
        const auto& cfg = dw->config;

        if (cfg.is_open != current_open) {
            current_open = cfg.is_open;
            if (current_open) slide_anim_.forward();
            else              slide_anim_.reverse();
        }

        float t = slide_anim_.get();
        bool visible = t > 0.005f;

        // Body (always present)
        WidgetPtr body = cfg.body;
        if (!body) {
            auto empty = std::make_shared<FlexBox>();
            empty->widthPercent(100).heightPercent(100);
            body = empty;
        }

        if (!visible) return body;

        // ── Backdrop ─────────────────────────────────────────
        uint8_t bd_alpha = static_cast<uint8_t>(((cfg.backdrop_color >> 24) & 0xFF) * t);
        Color   faded_bd = (cfg.backdrop_color & 0x00FFFFFF) | (bd_alpha << 24);

        auto bd_box = std::make_shared<FlexBox>();
        bd_box->positionType(YGPositionTypeAbsolute)
               .position(YGEdgeAll, 0.0f)
               .widthPercent(100).heightPercent(100)
               .backgroundColor(faded_bd);

        WidgetPtr bd_widget = bd_box;
        if (cfg.dismiss_on_backdrop && cfg.on_close) {
            bd_widget = gesture_detector({ .child = bd_box, .on_tap = cfg.on_close });
        }

        // ── Drawer Panel ─────────────────────────────────────
        // Translate from off-screen to on-screen
        bool is_left = (cfg.side == DrawerConfig::Side::Left);
        float translate_x = is_left
            ? -(1.0f - t) * cfg.width
            :  (1.0f - t) * cfg.width;

        auto panel = container({
            .color         = cfg.background_color,
            .width         = cfg.width,
            .height_percent = 100.0f,
            .shadow        = Shadow{0x60000000, 20.0f, is_left ? Point{4, 0} : Point{-4, 0}},
            .child         = cfg.child,
        });

        auto translated = transform({
            .translate_x = translate_x,
            .child       = panel,
        });

        auto drawer_wrapper = std::make_shared<FlexBox>();
        drawer_wrapper->positionType(YGPositionTypeAbsolute)
                       .position(YGEdgeTop, 0.0f)
                       .heightPercent(100);

        if (is_left) {
            drawer_wrapper->position(YGEdgeLeft, 0.0f);
        } else {
            drawer_wrapper->position(YGEdgeRight, 0.0f);
        }
        drawer_wrapper->child(translated);

        // Root stack
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100);
        root->child(body);
        root->child(bd_widget);
        root->child(drawer_wrapper);

        return root;
    }
};

std::unique_ptr<State> DrawerWidget::createState() {
    return std::make_unique<DrawerState>();
}

std::shared_ptr<Widget> drawer(DrawerConfig config) {
    return std::make_shared<DrawerWidget>(std::move(config));
}

} // namespace vaura
