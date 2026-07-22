/// @file slider.cpp
/// @brief Slider widget implementation.

#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/tree/element.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include <layout_engine/Yoga.h>
#include <algorithm>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderSlider
// ════════════════════════════════════════════════════════════════

class RenderSlider : public RenderBox {
public:
    SliderConfig config;

    RenderSlider() {
        // Set default minimum height based on thumb size + padding
        YGNodeStyleSetMinHeight(yogaNode(), 30.0f);
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        float progress = 0.0f;
        if (config.max_value > config.min_value) {
            progress = (config.value - config.min_value) / (config.max_value - config.min_value);
            progress = std::clamp(progress, 0.0f, 1.0f);
        }

        // Draw track
        float track_y = bounds.y + (bounds.height - config.track_height) / 2.0f;
        float track_width = bounds.width - config.thumb_radius * 2.0f;
        float track_x = bounds.x + config.thumb_radius; // Padding so thumb doesn't overflow

        Rect track_rect{track_x, track_y, track_width, config.track_height};
        
        // Inactive track
        Paint inactive_paint;
        inactive_paint.setColor(config.inactive_color);
        inactive_paint.setStyle(PaintStyle::Fill);
        canvas.drawRRect(track_rect, BorderRadius::circular(config.track_height / 2.0f), inactive_paint);

        // Active track
        float active_width = track_width * progress;
        Rect active_rect{track_x, track_y, active_width, config.track_height};
        
        Paint active_paint;
        active_paint.setColor(config.active_color);
        active_paint.setStyle(PaintStyle::Fill);
        canvas.drawRRect(active_rect, BorderRadius::circular(config.track_height / 2.0f), active_paint);

        // Draw thumb
        float thumb_x = track_x + active_width;
        float thumb_y = bounds.y + bounds.height / 2.0f;
        
        // Thumb is a circle
        Rect thumb_rect{
            thumb_x - config.thumb_radius,
            thumb_y - config.thumb_radius,
            config.thumb_radius * 2.0f,
            config.thumb_radius * 2.0f
        };

        Paint thumb_paint;
        thumb_paint.setColor(config.thumb_color);
        thumb_paint.setStyle(PaintStyle::Fill);
        
        // Optionally add a shadow to the thumb
        // For now, just draw the white circle
        canvas.drawRRect(thumb_rect, BorderRadius::circular(config.thumb_radius), thumb_paint);
    }

    bool hitTestSelf(Point localPoint) const override {
        Rect bounds = Rect::fromPointSize(Point{0.0f, 0.0f}, size_);
        return bounds.contains(localPoint);
    }
};

// ════════════════════════════════════════════════════════════════
// SliderGraphicsWidget
// ════════════════════════════════════════════════════════════════

class SliderGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    SliderConfig config;

    explicit SliderGraphicsWidget(SliderConfig cfg) : config(std::move(cfg)) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "SliderGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderSlider>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rs = static_cast<RenderSlider&>(renderObject);
        rs.config = config;
        
        YGNodeStyleSetWidth(rs.yogaNode(), config.width);
        // Height is determined by the minimum height and parent constraints.
        
        rs.markNeedsLayout();
        rs.markNeedsPaint();
    }
};

// ════════════════════════════════════════════════════════════════
// SliderState
// ════════════════════════════════════════════════════════════════

class SliderState : public State {
public:
    bool is_dragging = false;

    float calculateValueFromPosition(float x, const SliderConfig& cfg) {
        // Calculate based on the track boundaries
        auto* render_box = static_cast<RenderBox*>(context().element()->findRenderObject());
        if (!render_box) return cfg.value;
        
        float width = render_box->size().width;
        float track_width = width - cfg.thumb_radius * 2.0f;
        float track_x = cfg.thumb_radius;

        float progress = (x - track_x) / track_width;
        progress = std::clamp(progress, 0.0f, 1.0f);
        
        return cfg.min_value + progress * (cfg.max_value - cfg.min_value);
    }

    void updateValue(float x) {
        auto* sw = static_cast<const SliderWidget*>(widget());
        float new_value = calculateValueFromPosition(x, sw->config);
        
        if (sw->config.on_changed && new_value != sw->config.value) {
            sw->config.on_changed(new_value);
        }
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sw = static_cast<const SliderWidget*>(widget());
        
        auto graphics = std::make_shared<SliderGraphicsWidget>(sw->config);

        return gesture_detector({
            .child = graphics,
            .on_tap = [this, sw]() {
                // onTap acts like on_change_end here if tapped without moving
                if (is_dragging) {
                    is_dragging = false;
                    if (sw->config.on_change_end) {
                        sw->config.on_change_end(sw->config.value);
                    }
                }
            },
            .on_tap_down = [this, sw]() {
                is_dragging = true;
                if (sw->config.on_change_start) {
                    sw->config.on_change_start(sw->config.value);
                }
            },
            .on_pointer_move = [this, sw](PointerEvent e) {
                if (is_dragging) {
                    updateValue(e.localPosition.x);
                }
            }
        });
    }
};

std::unique_ptr<State> SliderWidget::createState() {
    return std::make_unique<SliderState>();
}

std::shared_ptr<Widget> slider(SliderConfig config) {
    return std::make_shared<SliderWidget>(std::move(config));
}

} // namespace vaura
