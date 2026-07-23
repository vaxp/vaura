/// @file switch.cpp
/// @brief Switch widget implementation.

#include "vaura/widgets/switch.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Anu.h>
#include <chrono>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderSwitch
// ════════════════════════════════════════════════════════════════

class RenderSwitch : public RenderBox {
public:
    SwitchConfig config;
    float progress = 0.0f; // 0.0 = off, 1.0 = on

    RenderSwitch() {
        // Set default fixed size for the switch graphic
        ANUNodeStyleSetWidth(anuNode(), 44.0f);
        ANUNodeStyleSetHeight(anuNode(), 24.0f);
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        // Interpolate background color
        Color bg_color = interpolateColor(config.inactive_color, config.active_color, progress);

        // Draw track
        Paint track_paint;
        track_paint.setColor(bg_color);
        track_paint.setStyle(PaintStyle::Fill);
        
        float track_radius = bounds.height / 2.0f;
        canvas.drawRRect(bounds, BorderRadius::circular(track_radius), track_paint);

        // Draw thumb (circle)
        float thumb_padding = 2.0f;
        float thumb_diameter = bounds.height - (thumb_padding * 2.0f);
        
        // Calculate thumb X position based on progress
        float min_x = bounds.x + thumb_padding;
        float max_x = bounds.x + bounds.width - thumb_padding - thumb_diameter;
        float thumb_x = min_x + (max_x - min_x) * progress;
        float thumb_y = bounds.y + thumb_padding;

        Rect thumb_rect{thumb_x, thumb_y, thumb_diameter, thumb_diameter};

        Paint thumb_paint;
        thumb_paint.setColor(config.thumb_color);
        thumb_paint.setStyle(PaintStyle::Fill);

        // A circle is just an RRect with radius = size/2
        canvas.drawRRect(thumb_rect, BorderRadius::circular(thumb_diameter / 2.0f), thumb_paint);
    }

    bool hitTestSelf(Point localPoint) const override {
        Rect bounds = Rect::fromPointSize(Point{0.0f, 0.0f}, size_);
        return bounds.contains(localPoint);
    }

private:
    Color interpolateColor(Color a, Color b, float t) {
        uint8_t a_a = (a >> 24) & 0xFF;
        uint8_t a_r = (a >> 16) & 0xFF;
        uint8_t a_g = (a >>  8) & 0xFF;
        uint8_t a_b = (a >>  0) & 0xFF;

        uint8_t b_a = (b >> 24) & 0xFF;
        uint8_t b_r = (b >> 16) & 0xFF;
        uint8_t b_g = (b >>  8) & 0xFF;
        uint8_t b_b = (b >>  0) & 0xFF;

        uint8_t r_a = a_a + static_cast<uint8_t>((b_a - a_a) * t);
        uint8_t r_r = a_r + static_cast<uint8_t>((b_r - a_r) * t);
        uint8_t r_g = a_g + static_cast<uint8_t>((b_g - a_g) * t);
        uint8_t r_b = a_b + static_cast<uint8_t>((b_b - a_b) * t);

        return (r_a << 24) | (r_r << 16) | (r_g << 8) | r_b;
    }
};

// ════════════════════════════════════════════════════════════════
// SwitchGraphicsWidget
// ════════════════════════════════════════════════════════════════

class SwitchGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    SwitchConfig config;
    float progress;

    SwitchGraphicsWidget(SwitchConfig cfg, float p) : config(std::move(cfg)), progress(p) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "SwitchGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderSwitch>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rs = static_cast<RenderSwitch&>(renderObject);
        rs.config = config;
        rs.progress = progress;
        
        ANUNodeStyleSetWidth(rs.anuNode(), config.width);
        ANUNodeStyleSetHeight(rs.anuNode(), config.height);
        
        rs.markNeedsLayout();
        rs.markNeedsPaint();
    }
};

// ════════════════════════════════════════════════════════════════
// SwitchState
// ════════════════════════════════════════════════════════════════

class SwitchState : public State {
public:
    AnimatedValue<float> progress_{0.0f, 1.0f, std::chrono::milliseconds{200}};
    std::unique_ptr<Ticker> ticker_;
    bool current_value_ = false;

    void initState() override {
        State::initState();
        auto* sw = static_cast<const SwitchWidget*>(widget());
        current_value_ = sw->config.value;

        if (current_value_) {
            progress_.controller().forward();
            progress_.controller().tick(); // Jump
        }

        ticker_ = createTicker([this]() {
            float old_val = progress_.get();
            progress_.tick();
            if (old_val != progress_.get()) {
                setState([]{}); // Trigger rebuild to update graphics
            }
        });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) {
            ticker_->stop();
        }
        progress_.dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sw = static_cast<const SwitchWidget*>(widget());
        
        // If external value changed, trigger animation
        if (sw->config.value != current_value_) {
            current_value_ = sw->config.value;
            if (current_value_) progress_.forward();
            else progress_.reverse();
        }

        auto graphics = std::make_shared<SwitchGraphicsWidget>(sw->config, progress_.get());

        return gesture_detector({
            .child = graphics,
            .on_tap = [this, sw]() {
                if (sw->config.on_changed) {
                    sw->config.on_changed(!current_value_);
                }
            }
        });
    }
};

std::unique_ptr<State> SwitchWidget::createState() {
    return std::make_unique<SwitchState>();
}

} // namespace vaura
