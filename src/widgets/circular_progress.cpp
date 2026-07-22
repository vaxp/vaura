/// @file circular_progress.cpp
/// @brief CircularProgressIndicator widget implementation.

#include "vaura/widgets/circular_progress.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderCircularProgress
// ════════════════════════════════════════════════════════════════

class RenderCircularProgress : public RenderBox {
public:
    CircularProgressConfig config;
    float rotation = 0.0f; // Used for indeterminate animation

    RenderCircularProgress() {
        YGNodeStyleSetWidth(yogaNode(), 40.0f);
        YGNodeStyleSetHeight(yogaNode(), 40.0f);
    }

    void paint(PaintContext& context) override {
        auto& canvas = context.canvas;
        Rect bounds = Rect::fromPointSize(context.offset, size_);

        // Background circle
        if (config.background_color != 0x00000000) {
            Paint bg_paint;
            bg_paint.setColor(config.background_color);
            bg_paint.setStyle(PaintStyle::Stroke);
            bg_paint.setStrokeWidth(config.stroke_width);
            
            Point center = {bounds.x + bounds.width / 2.0f, bounds.y + bounds.height / 2.0f};
            float radius = (std::min(bounds.width, bounds.height) - config.stroke_width) / 2.0f;
            canvas.drawCircle(center, radius, bg_paint);
        }

        // Active arc
        Paint fg_paint;
        fg_paint.setColor(config.color);
        fg_paint.setStyle(PaintStyle::Stroke);
        fg_paint.setStrokeWidth(config.stroke_width);
        
        Rect arc_bounds = {
            bounds.x + config.stroke_width / 2.0f,
            bounds.y + config.stroke_width / 2.0f,
            bounds.width - config.stroke_width,
            bounds.height - config.stroke_width
        };

        if (config.value.has_value()) {
            // Determinate mode
            float sweep = std::clamp(config.value.value(), 0.0f, 1.0f) * 360.0f;
            canvas.drawArc(arc_bounds, -90.0f, sweep, false, fg_paint);
        } else {
            // Indeterminate mode
            float start = -90.0f + rotation;
            // The sweep angle changes slightly as it rotates for a better effect
            // We can just use a fixed sweep angle like 270 degrees for a simple spinner
            canvas.drawArc(arc_bounds, start, 270.0f, false, fg_paint);
        }
    }
};

// ════════════════════════════════════════════════════════════════
// CircularProgressWidget & State
// ════════════════════════════════════════════════════════════════

class CircularProgressGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    CircularProgressConfig config;
    float rotation;

    CircularProgressGraphicsWidget(CircularProgressConfig cfg, float r) 
        : config(std::move(cfg)), rotation(r) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "CircularProgressGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderCircularProgress>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rp = static_cast<RenderCircularProgress&>(renderObject);
        rp.config = config;
        rp.rotation = rotation;
        
        YGNodeStyleSetWidth(rp.yogaNode(), config.size);
        YGNodeStyleSetHeight(rp.yogaNode(), config.size);
        
        rp.markNeedsLayout();
        rp.markNeedsPaint();
    }
};

class CircularProgressState : public State {
public:
    AnimatedValue<float> rotation_;
    std::unique_ptr<Ticker> ticker_;
    bool is_indeterminate_ = false;

    CircularProgressState() 
        : rotation_(0.0f, 360.0f, std::chrono::milliseconds{1500}, &Curves::linear) {}

    void initState() override {
        State::initState();
        auto* cp = static_cast<const CircularProgressWidget*>(widget());
        is_indeterminate_ = !cp->config.value.has_value();

        if (is_indeterminate_) {
            rotation_.controller().setRepeats(true);
            rotation_.forward();
            
            ticker_ = createTicker([this]() {
                if (rotation_.tick()) {
                    setState([]{});
                }
            });
            ticker_->start();
        }
    }

    void dispose() override {
        if (ticker_) {
            ticker_->stop();
        }
        rotation_.dispose();
        State::dispose();
    }

    void didUpdateWidget(const Widget& oldWidget) override {
        auto* cp = static_cast<const CircularProgressWidget*>(widget());
        auto* old_cp = static_cast<const CircularProgressWidget*>(&oldWidget);
        (void)old_cp;

        bool new_is_indet = !cp->config.value.has_value();
        if (new_is_indet != is_indeterminate_) {
            is_indeterminate_ = new_is_indet;
            if (is_indeterminate_) {
                rotation_.controller().setRepeats(true);
                rotation_.forward();
                if (!ticker_) {
                    ticker_ = createTicker([this]() {
                        if (rotation_.tick()) {
                            setState([]{});
                        }
                    });
                }
                ticker_->start();
            } else {
                rotation_.reverse(); // or stop if we had a stop method
                if (ticker_) {
                    ticker_->stop();
                }
            }
        }
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* cp = static_cast<const CircularProgressWidget*>(widget());
        
        return std::make_shared<CircularProgressGraphicsWidget>(cp->config, rotation_.get());
    }
};

std::unique_ptr<State> CircularProgressWidget::createState() {
    return std::make_unique<CircularProgressState>();
}

} // namespace vaura
