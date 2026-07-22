#include "vaura/widgets/radio.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/path.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

class RenderRadio : public RenderBox {
public:
    RadioConfig config;
    float progress = 0.0f; // 0.0 = off, 1.0 = on

    RenderRadio() {
        YGNodeStyleSetWidth(yogaNode(), 24.0f);
        YGNodeStyleSetHeight(yogaNode(), 24.0f);
    }

    void setConfig(const RadioConfig& new_config, float new_progress) {
        bool needs_repaint = false;
        if (config.size != new_config.size) {
            YGNodeStyleSetWidth(yogaNode(), new_config.size);
            YGNodeStyleSetHeight(yogaNode(), new_config.size);
            markNeedsLayout();
        }
        if (progress != new_progress) {
            progress = new_progress;
            needs_repaint = true;
        }
        config = new_config;
        if (needs_repaint) {
            markNeedsPaint();
        }
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);
        float radius = bounds.width / 2.0f;
        Point center = {bounds.x + radius, bounds.y + radius};

        Color bg_color = interpolateColor(config.inactive_color, config.active_color, progress);

        Paint border_paint;
        border_paint.setColor(bg_color);
        border_paint.setStyle(PaintStyle::Stroke);
        border_paint.setStrokeWidth(config.stroke_width);
        border_paint.setAntiAlias(true);
        
        // Draw the outer ring
        canvas.drawCircle(center, radius - (config.stroke_width / 2.0f), border_paint);

        // Draw the inner dot based on progress
        if (progress > 0.0f) {
            Paint dot_paint;
            dot_paint.setColor(config.active_color);
            dot_paint.setStyle(PaintStyle::Fill);
            dot_paint.setAntiAlias(true);

            float inner_radius = radius - config.stroke_width - config.inner_padding;
            canvas.drawCircle(center, inner_radius * progress, dot_paint);
        }
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

class RawRadioWidget : public SingleChildRenderObjectWidget {
public:
    RadioConfig config;
    float progress;

    RawRadioWidget(RadioConfig config, float progress)
        : config(std::move(config)), progress(progress) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "RawRadioWidget"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderRadio>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& render_object) override {
        auto& ro = static_cast<RenderRadio&>(render_object);
        ro.setConfig(config, progress);
    }
};

class RadioStatefulWidget : public StatefulWidget {
public:
    RadioConfig config;
    explicit RadioStatefulWidget(RadioConfig config) : config(std::move(config)) {}
    [[nodiscard]] std::string_view typeName() const override { return "RadioWidget"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class RadioState : public State {
public:
    AnimatedValue<float> progress_{0.0f, 1.0f, std::chrono::milliseconds{200}};
    std::unique_ptr<Ticker> ticker_;
    bool current_value_ = false;

    void initState() override {
        State::initState();
        auto* rb = static_cast<const RadioStatefulWidget*>(widget());
        current_value_ = rb->config.value;

        if (current_value_) {
            progress_.controller().forward();
            progress_.controller().tick();
        }

        ticker_ = createTicker([this]() {
            float old_val = progress_.get();
            progress_.tick();
            if (old_val != progress_.get()) {
                setState([]{});
            }
        });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        progress_.dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* rb = static_cast<const RadioStatefulWidget*>(widget());
        
        if (rb->config.value != current_value_) {
            current_value_ = rb->config.value;
            if (current_value_) progress_.forward();
            else progress_.reverse();
        }

        auto graphics = std::make_shared<RawRadioWidget>(rb->config, progress_.get());

        return gesture_detector({
            .child = graphics,
            .on_tap = [this, rb]() {
                if (rb->config.on_changed) {
                    // Radio button only triggers when selected, usually. But we allow toggling for simplicity, or just selection.
                    // Usually you can't "un-select" a radio button by clicking it again, but we'll leave it to the user.
                    rb->config.on_changed(true);
                }
            }
        });
    }
};

std::unique_ptr<State> RadioStatefulWidget::createState() {
    return std::make_unique<RadioState>();
}

std::shared_ptr<Widget> radio(RadioConfig config) {
    return std::make_shared<RadioStatefulWidget>(std::move(config));
}

} // namespace vaura
