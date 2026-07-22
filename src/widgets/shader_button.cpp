#include "vaura/widgets/shader_button.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/animation/ticker.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/flexbox.hpp"
#include <layout_engine/Yoga.h>
#include <chrono>

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>
#include <include/core/SkRRect.h>
#include <include/effects/SkRuntimeEffect.h>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderShaderButtonBackground
// ════════════════════════════════════════════════════════════════

class RenderShaderButtonBackground : public RenderBox {
public:
    ShaderButtonConfig config;
    float time = 0.0f;
    float hover = 0.0f;
    float pressed = 0.0f;
    sk_sp<SkRuntimeEffect> effect_;

    RenderShaderButtonBackground() {
        // Let it expand in a flex container if width is -1, or set fixed
    }

    void initEffect() {
        if (effect_) return;
        auto result = SkRuntimeEffect::MakeForShader(SkString(config.sksl_code));
        if (result.effect) {
            effect_ = result.effect;
        } else {
            fprintf(stderr, "SkSL Compile Error (ShaderButton): %s\n", result.errorText.c_str());
            // Fallback to a valid, empty shader or something
            auto fallback = SkRuntimeEffect::MakeForShader(SkString("half4 main(float2 p) { return half4(1,0,0,1); }"));
            effect_ = fallback.effect;
        }
    }

    void paint(PaintContext& context) override {
        initEffect();
        if (!effect_) return;

        SkCanvas* sk_canvas = static_cast<SkCanvas*>(context.canvas.getNativeHandle());
        if (!sk_canvas) return;

        Rect bounds = Rect::fromPointSize(context.offset, size_);
        SkRect sk_bounds = SkRect::MakeXYWH(bounds.x, bounds.y, bounds.width, bounds.height);
        SkRRect rrect = SkRRect::MakeRectXY(sk_bounds, config.border_radius, config.border_radius);

        struct Uniforms {
            float time;
            float res_x;
            float res_y;
            float hover;
            float pressed;
            float c1_r, c1_g, c1_b, c1_a;
            float c2_r, c2_g, c2_b, c2_a;
        } uniforms;

        uniforms.time = time;
        uniforms.res_x = bounds.width;
        uniforms.res_y = bounds.height;
        uniforms.hover = hover;
        uniforms.pressed = pressed;
        
        auto toFloat4 = [](Color c, float* r, float* g, float* b, float* a) {
            *a = ((c >> 24) & 0xFF) / 255.0f;
            *r = ((c >> 16) & 0xFF) / 255.0f;
            *g = ((c >> 8)  & 0xFF) / 255.0f;
            *b = ((c)       & 0xFF) / 255.0f;
        };

        toFloat4(config.color1, &uniforms.c1_r, &uniforms.c1_g, &uniforms.c1_b, &uniforms.c1_a);
        toFloat4(config.color2, &uniforms.c2_r, &uniforms.c2_g, &uniforms.c2_b, &uniforms.c2_a);

        sk_sp<SkData> uniformData = SkData::MakeWithCopy(&uniforms, sizeof(uniforms));
        sk_sp<SkShader> shader = effect_->makeShader(std::move(uniformData), nullptr, 0);

        SkPaint paint;
        paint.setShader(shader);
        paint.setAntiAlias(true);

        sk_canvas->save();
        sk_canvas->drawRRect(rrect, paint);
        sk_canvas->restore();

        // Paint child widget (e.g., text)
        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }
    }
};

// ════════════════════════════════════════════════════════════════
// ShaderButtonGraphicsWidget
// ════════════════════════════════════════════════════════════════

class ShaderButtonGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    ShaderButtonConfig config;
    float time;
    float hover;
    float pressed;

    ShaderButtonGraphicsWidget(ShaderButtonConfig cfg, float t, float h, float p) 
        : config(std::move(cfg)), time(t), hover(h), pressed(p) {
        this->child = config.child;
    }

    [[nodiscard]] std::string_view typeName() const override { return "ShaderButtonGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderShaderButtonBackground>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rs = static_cast<RenderShaderButtonBackground&>(renderObject);
        // Only update shader config if changed (to prevent recompilation loop)
        if (rs.config.sksl_code != config.sksl_code) {
            rs.effect_.reset();
        }
        rs.config = config;
        rs.time = time;
        rs.hover = hover;
        rs.pressed = pressed;
        
        if (config.width >= 0) {
            YGNodeStyleSetWidth(rs.yogaNode(), config.width);
        } else {
            YGNodeStyleSetWidthAuto(rs.yogaNode());
        }
        
        if (config.height >= 0) {
            YGNodeStyleSetHeight(rs.yogaNode(), config.height);
        } else {
            YGNodeStyleSetHeightAuto(rs.yogaNode());
        }
        
        YGNodeStyleSetJustifyContent(rs.yogaNode(), YGJustifyCenter);
        YGNodeStyleSetAlignItems(rs.yogaNode(), YGAlignCenter);
        
        rs.markNeedsLayout();
        rs.markNeedsPaint();
    }
};

// ════════════════════════════════════════════════════════════════
// ShaderButtonState
// ════════════════════════════════════════════════════════════════

class ShaderButtonState : public State {
public:
    std::unique_ptr<Ticker> ticker_;
    float current_time_ = 0.0f;
    std::chrono::steady_clock::time_point last_time_;
    
    // Animation targets
    AnimatedValue<float> hover_anim_{0.0f, 1.0f, std::chrono::milliseconds(200)};
    AnimatedValue<float> pressed_anim_{0.0f, 1.0f, std::chrono::milliseconds(100)};

    void initState() override {
        State::initState();
        last_time_ = std::chrono::steady_clock::now();
        
        hover_anim_.addListener([this] { setState([]{}); });
        pressed_anim_.addListener([this] { setState([]{}); });
        
        ticker_ = createTicker([this]() {
            auto now = std::chrono::steady_clock::now();
            float dt = std::chrono::duration<float>(now - last_time_).count();
            last_time_ = now;
            
            hover_anim_.tick();
            pressed_anim_.tick();
            
            float speed = 1.0f + hover_anim_.get() * 2.0f + pressed_anim_.get() * 4.0f;
            current_time_ += dt * speed;
            
            setState([]{}); // Always tick for time shader
        });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        hover_anim_.dispose();
        pressed_anim_.dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sb = static_cast<const ShaderButtonWidget*>(widget());
        
        auto graphics = std::make_shared<ShaderButtonGraphicsWidget>(
            sb->config, current_time_, hover_anim_.get(), pressed_anim_.get());

        // We wrap it in a Gesture Detector
        auto fn = sb->config.on_tap;
        return gesture_detector({
            .child = graphics,
            .on_tap = [this, fn]() {
                if (fn) fn();
            },
            .on_tap_down = [this]() {
                pressed_anim_.forward();
            },
            .on_tap_up = [this]() {
                pressed_anim_.reverse();
            },
            .on_hover_enter = [this]() {
                hover_anim_.forward();
            },
            .on_hover_exit = [this]() {
                hover_anim_.reverse();
            }
        });
    }
};

std::unique_ptr<State> ShaderButtonWidget::createState() {
    return std::make_unique<ShaderButtonState>();
}

} // namespace vaura
