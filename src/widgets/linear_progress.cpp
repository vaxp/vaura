#include "vaura/widgets/linear_progress.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>
#include <algorithm>
#include <cmath>
#include <chrono>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderLinearProgress
// ════════════════════════════════════════════════════════════════

class RenderLinearProgress : public RenderBox {
public:
    LinearProgressConfig config;
    float anim_value = 0.0f; // Used for indeterminate animation

    explicit RenderLinearProgress(LinearProgressConfig config) : config(std::move(config)) {}

    void setConfig(const LinearProgressConfig& new_config, float new_anim) {
        bool needs_layout = false;
        if (config.thickness != new_config.thickness) {
            needs_layout = true;
        }
        config = new_config;
        anim_value = new_anim;

        if (needs_layout) {
            updateLayout();
        } else {
            markNeedsPaint();
        }
    }

    void updateLayout() {
        auto node = yogaNode();
        YGNodeStyleSetHeight(node, config.thickness);
        YGNodeStyleSetAlignSelf(node, YGAlignStretch); // Stretch to fill width
        markNeedsLayout();
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        // Draw track
        Paint track_paint;
        track_paint.setColor(config.inactive_color);
        track_paint.setStyle(PaintStyle::Fill);
        canvas.drawRRect(bounds, BorderRadius::circular(config.border_radius), track_paint);

        // Draw active part
        Paint active_paint;
        active_paint.setColor(config.active_color);
        active_paint.setStyle(PaintStyle::Fill);

        if (config.value.has_value()) {
            // Determinate
            float progress = std::clamp(config.value.value(), 0.0f, 1.0f);
            float width = bounds.width * progress;
            if (width > 0) {
                Rect active_rect{bounds.x, bounds.y, width, bounds.height};
                canvas.drawRRect(active_rect, BorderRadius::circular(config.border_radius), active_paint);
            }
        } else {
            // Indeterminate animation
            // Sine wave to ease in and out
            float width = bounds.width * 0.4f; // 40% of track width
            float move_range = bounds.width + width;
            float current_x = bounds.x - width + (move_range * anim_value);
            
            Rect active_rect{current_x, bounds.y, width, bounds.height};
            
            // Clip to bounds so it doesn't spill out
            canvas.save();
            canvas.clipRRect(bounds, BorderRadius::circular(config.border_radius));
            canvas.drawRRect(active_rect, BorderRadius::circular(config.border_radius), active_paint);
            canvas.restore();
        }
    }
};

// ════════════════════════════════════════════════════════════════
// LinearProgressGraphicsWidget
// ════════════════════════════════════════════════════════════════

class LinearProgressGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    LinearProgressConfig config;
    float anim_value;

    LinearProgressGraphicsWidget(LinearProgressConfig config, float anim_value)
        : config(std::move(config)), anim_value(anim_value) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "LinearProgressGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderLinearProgress>(config);
        obj->anim_value = anim_value;
        obj->updateLayout();
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& render_object) override {
        auto& ro = static_cast<RenderLinearProgress&>(render_object);
        ro.setConfig(config, anim_value);
    }
};

// ════════════════════════════════════════════════════════════════
// LinearProgressWidget
// ════════════════════════════════════════════════════════════════

class LinearProgressWidget : public StatefulWidget {
public:
    LinearProgressConfig config;
    explicit LinearProgressWidget(LinearProgressConfig config) : config(std::move(config)) {}
    [[nodiscard]] std::string_view typeName() const override { return "LinearProgressWidget"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class LinearProgressState : public State {
public:
    std::shared_ptr<Ticker> ticker;
    float anim_value = 0.0f;
    double start_time = 0.0;
    std::chrono::steady_clock::time_point last_time;

    void initState() override {
        State::initState();
        auto* lw = static_cast<const LinearProgressWidget*>(widget());
        
        if (!lw->config.value.has_value()) {
            startTicker();
        }
    }

    void didUpdateWidget(const Widget& oldWidget) override {
        State::didUpdateWidget(oldWidget);
        auto* lw = static_cast<const LinearProgressWidget*>(widget());
        
        if (lw->config.value.has_value()) {
            stopTicker();
        } else if (!ticker || !ticker->isActive()) {
            startTicker();
        }
    }

    void startTicker() {
        start_time = 0.0;
        last_time = std::chrono::steady_clock::now();
        ticker = std::make_shared<Ticker>([this]() {
            auto now = std::chrono::steady_clock::now();
            double dt = std::chrono::duration<double>(now - last_time).count();
            last_time = now;
            start_time += dt;
            setState([this]() {
                // Loop every 1.5 seconds
                float duration = 1.5f;
                float progress = std::fmod(start_time, duration) / duration;
                anim_value = progress;
            });
        });
        ticker->start();
    }

    void stopTicker() {
        if (ticker) {
            ticker->stop();
            ticker = nullptr;
        }
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* lw = static_cast<const LinearProgressWidget*>(widget());
        return std::make_shared<LinearProgressGraphicsWidget>(lw->config, anim_value);
    }
};

std::unique_ptr<State> LinearProgressWidget::createState() {
    return std::make_unique<LinearProgressState>();
}

std::shared_ptr<Widget> linear_progress(LinearProgressConfig config) {
    return std::make_shared<LinearProgressWidget>(std::move(config));
}

} // namespace vaura
