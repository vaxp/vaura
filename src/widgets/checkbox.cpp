#include "vaura/widgets/checkbox.hpp"
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

class RenderCheckbox : public RenderBox {
public:
    CheckboxConfig config;
    float progress = 0.0f; // 0.0 = off, 1.0 = on

    RenderCheckbox() {
        YGNodeStyleSetWidth(yogaNode(), 24.0f);
        YGNodeStyleSetHeight(yogaNode(), 24.0f);
    }

    void setConfig(const CheckboxConfig& new_config, float new_progress) {
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

        Color bg_color = interpolateColor(config.inactive_color, config.active_color, progress);

        Paint box_paint;
        box_paint.setColor(bg_color);
        if (progress == 0.0f) {
            box_paint.setStyle(PaintStyle::Stroke);
            box_paint.setStrokeWidth(config.stroke_width);
        } else {
            box_paint.setStyle(PaintStyle::Fill);
        }
        
        canvas.drawRRect(bounds, BorderRadius::circular(config.border_radius), box_paint);

        if (progress > 0.0f) {
            Paint check_paint;
            check_paint.setColor(config.checkmark_color);
            check_paint.setStyle(PaintStyle::Stroke);
            check_paint.setStrokeWidth(config.stroke_width);
            check_paint.setStrokeCap(StrokeCap::Round);
            check_paint.setStrokeJoin(StrokeJoin::Round);

            float p1x = bounds.x + bounds.width * 0.25f;
            float p1y = bounds.y + bounds.height * 0.5f;
            float p2x = bounds.x + bounds.width * 0.45f;
            float p2y = bounds.y + bounds.height * 0.7f;
            float p3x = bounds.x + bounds.width * 0.75f;
            float p3y = bounds.y + bounds.height * 0.3f;

            Path path;
            path.moveTo(p1x, p1y);
            
            if (progress < 0.5f) {
                float t = progress * 2.0f; // 0 to 1
                path.lineTo(p1x + (p2x - p1x) * t, p1y + (p2y - p1y) * t);
            } else {
                path.lineTo(p2x, p2y);
                float t = (progress - 0.5f) * 2.0f; // 0 to 1
                path.lineTo(p2x + (p3x - p2x) * t, p2y + (p3y - p2y) * t);
            }

            canvas.drawPath(path, check_paint);
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

class RawCheckboxWidget : public SingleChildRenderObjectWidget {
public:
    CheckboxConfig config;
    float progress;

    RawCheckboxWidget(CheckboxConfig config, float progress)
        : config(std::move(config)), progress(progress) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "RawCheckboxWidget"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderCheckbox>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& render_object) override {
        auto& ro = static_cast<RenderCheckbox&>(render_object);
        ro.setConfig(config, progress);
    }
};

class CheckboxStatefulWidget : public StatefulWidget {
public:
    CheckboxConfig config;
    explicit CheckboxStatefulWidget(CheckboxConfig config) : config(std::move(config)) {}
    [[nodiscard]] std::string_view typeName() const override { return "CheckboxWidget"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class CheckboxState : public State {
public:
    AnimatedValue<float> progress_{0.0f, 1.0f, std::chrono::milliseconds{200}};
    std::unique_ptr<Ticker> ticker_;
    bool current_value_ = false;

    void initState() override {
        State::initState();
        auto* cb = static_cast<const CheckboxStatefulWidget*>(widget());
        current_value_ = cb->config.value;

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
        auto* cb = static_cast<const CheckboxStatefulWidget*>(widget());
        
        if (cb->config.value != current_value_) {
            current_value_ = cb->config.value;
            if (current_value_) progress_.forward();
            else progress_.reverse();
        }

        auto graphics = std::make_shared<RawCheckboxWidget>(cb->config, progress_.get());

        return gesture_detector({
            .child = graphics,
            .on_tap = [this, cb]() {
                if (cb->config.on_changed) {
                    cb->config.on_changed(!current_value_);
                }
            }
        });
    }
};

std::unique_ptr<State> CheckboxStatefulWidget::createState() {
    return std::make_unique<CheckboxState>();
}

std::shared_ptr<Widget> checkbox(CheckboxConfig config) {
    return std::make_shared<CheckboxStatefulWidget>(std::move(config));
}

} // namespace vaura
