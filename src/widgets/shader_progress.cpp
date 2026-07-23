/// @file shader_progress.cpp
/// @brief ShaderProgress implementation using SkRuntimeEffect.

#include "vaura/widgets/shader_progress.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Anu.h>
#include <chrono>

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>
#include <include/effects/SkRuntimeEffect.h>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// SkSL Shaders
// ════════════════════════════════════════════════════════════════

const char* GALAXY_SKSL = R"(
    uniform float u_time;
    uniform float2 u_resolution;
    uniform half4 u_color1;
    uniform half4 u_color2;

    float2x2 rot(float a) {
        float s = sin(a), c = cos(a);
        return float2x2(c, -s, s, c);
    }

    half4 main(float2 fragCoord) {
        float2 uv = (fragCoord - 0.5 * u_resolution) / min(u_resolution.x, u_resolution.y);
        
        float t = u_time * 0.5;
        
        float2 p = uv;
        for(float i = 1.0; i < 6.0; i++) {
            p = p * rot(t * 0.1);
            p += float2(sin(t + p.y * i), cos(t + p.x * i)) * 0.1;
        }

        float r = length(uv);
        float circle = smoothstep(0.5, 0.45, r);
        
        float intensity = 0.05 / length(p);
        
        // Ring
        float ring = smoothstep(0.05, 0.0, abs(r - 0.35));
        intensity += ring * (0.5 + 0.5 * sin(t * 5.0 + atan(uv.y, uv.x) * 3.0));

        half4 col = mix(u_color1, u_color2, sin(length(p) * 10.0 + t) * 0.5 + 0.5);
        return col * intensity * circle;
    }
)";

const char* LIQUID_SKSL = R"(
    uniform float u_time;
    uniform float2 u_resolution;
    uniform half4 u_color1;
    uniform half4 u_color2;

    half4 main(float2 fragCoord) {
        float2 uv = (fragCoord - 0.5 * u_resolution) / min(u_resolution.x, u_resolution.y);
        float a = atan(uv.y, uv.x);
        float r = length(uv);

        float t = u_time * 2.0;
        
        // Base radius
        float rad = 0.35;
        // Wavy displacement
        rad += 0.05 * sin(a * 3.0 + t);
        rad += 0.02 * sin(a * 7.0 - t * 1.5);
        rad += 0.01 * sin(a * 11.0 + t * 0.5);

        float d = r - rad;
        
        // Smooth edge
        float alpha = smoothstep(0.01, -0.01, d);
        
        // Inner glow/highlight
        float highlight = smoothstep(0.0, -0.05, d) * smoothstep(-0.15, -0.05, d);
        
        // Gradient color based on y position and time
        half4 baseColor = mix(u_color1, u_color2, 0.5 + 0.5 * sin(uv.y * 3.0 + t));
        
        half4 finalColor = baseColor + half4(highlight * 0.5);
        finalColor.a = alpha;
        
        return finalColor;
    }
)";

// ════════════════════════════════════════════════════════════════
// RenderBox
// ════════════════════════════════════════════════════════════════

class RenderShaderProgress : public RenderBox {
public:
    ShaderProgressConfig config;
    float time = 0.0f;
    sk_sp<SkRuntimeEffect> effect_;

    RenderShaderProgress() {
        ANUNodeStyleSetWidth(anuNode(), 80.0f);
        ANUNodeStyleSetHeight(anuNode(), 80.0f);
    }

    void initEffect() {
        if (effect_) return;
        const char* sksl = nullptr;
        if (config.sksl_code == "galaxy") sksl = GALAXY_SKSL;
        else if (config.sksl_code == "liquid") sksl = LIQUID_SKSL;
        else sksl = config.sksl_code.c_str();

        auto result = SkRuntimeEffect::MakeForShader(SkString(sksl));
        if (result.effect) {
            effect_ = result.effect;
        } else {
            fprintf(stderr, "SkSL Compile Error: %s\n", result.errorText.c_str());
        }
    }

    void paint(PaintContext& context) override {
        initEffect();
        if (!effect_) return;

        SkCanvas* sk_canvas = static_cast<SkCanvas*>(context.canvas.getNativeHandle());
        if (!sk_canvas) return;

        Rect bounds = Rect::fromPointSize(context.offset, size_);
        SkRect sk_bounds = SkRect::MakeXYWH(bounds.x, bounds.y, bounds.width, bounds.height);

        // Uniforms: u_time (float), u_resolution (float2), u_color1 (half4), u_color2 (half4)
        struct Uniforms {
            float time;
            float res_x;
            float res_y;
            float c1_r, c1_g, c1_b, c1_a;
            float c2_r, c2_g, c2_b, c2_a;
        } uniforms;

        uniforms.time = time;
        uniforms.res_x = bounds.width;
        uniforms.res_y = bounds.height;
        
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
        sk_canvas->translate(bounds.x, bounds.y);
        
        SkRect local_bounds = SkRect::MakeWH(bounds.width, bounds.height);
        sk_canvas->drawRect(local_bounds, paint);
        
        sk_canvas->restore();
    }
};

// ════════════════════════════════════════════════════════════════
// Widget & State
// ════════════════════════════════════════════════════════════════

class ShaderProgressGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    ShaderProgressConfig config;
    float time;

    ShaderProgressGraphicsWidget(ShaderProgressConfig cfg, float t) 
        : config(std::move(cfg)), time(t) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "ShaderProgressGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderShaderProgress>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rs = static_cast<RenderShaderProgress&>(renderObject);
        rs.config = config;
        rs.time = time;
        
        ANUNodeStyleSetWidth(rs.anuNode(), config.size);
        ANUNodeStyleSetHeight(rs.anuNode(), config.size);
        
        rs.markNeedsLayout();
        rs.markNeedsPaint();
    }
};

class ShaderProgressState : public State {
public:
    std::unique_ptr<Ticker> ticker_;
    float current_time_ = 0.0f;
    std::chrono::steady_clock::time_point start_time_;

    void initState() override {
        State::initState();
        start_time_ = std::chrono::steady_clock::now();
        
        ticker_ = createTicker([this]() {
            auto now = std::chrono::steady_clock::now();
            float elapsed = std::chrono::duration<float>(now - start_time_).count();
            current_time_ = elapsed;
            setState([]{});
        });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sp = static_cast<const ShaderProgressWidget*>(widget());
        return std::make_shared<ShaderProgressGraphicsWidget>(sp->config, current_time_);
    }
};

std::unique_ptr<State> ShaderProgressWidget::createState() {
    return std::make_unique<ShaderProgressState>();
}

} // namespace vaura
