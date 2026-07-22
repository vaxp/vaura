/// @file shader_background.cpp
/// @brief ShaderBackground implementation using SkRuntimeEffect.

#include "vaura/widgets/shader_background.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/state/state.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>
#include <chrono>

#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkRect.h>
#include <include/effects/SkRuntimeEffect.h>

namespace vaura {

class RenderShaderBackground : public RenderBox {
public:
    ShaderBackgroundConfig config;
    float time = 0.0f;
    sk_sp<SkRuntimeEffect> effect_;
    bool compile_failed_ = false;

    RenderShaderBackground() {
        YGNodeStyleSetFlexGrow(yogaNode(), 1.0f); // Default to fill parent
    }

    void initEffect() {
        if (effect_ || compile_failed_) return;

        auto result = SkRuntimeEffect::MakeForShader(SkString(config.sksl_code.c_str()));
        if (result.effect) {
            effect_ = result.effect;
        } else {
            fprintf(stderr, "SkSL Compile Error:\n%s\n", result.errorText.c_str());
            compile_failed_ = true;
        }
    }

    void paint(PaintContext& context) override {
        initEffect();
        if (!effect_) return;

        SkCanvas* sk_canvas = static_cast<SkCanvas*>(context.canvas.getNativeHandle());
        if (!sk_canvas) return;

        Rect bounds = Rect::fromPointSize(context.offset, size_);
        SkRect sk_bounds = SkRect::MakeXYWH(bounds.x, bounds.y, bounds.width, bounds.height);

        // Uniforms: u_time (float), u_resolution (float2)
        struct Uniforms {
            float time;
            float res_x;
            float res_y;
        } uniforms;

        uniforms.time = time;
        uniforms.res_x = bounds.width;
        uniforms.res_y = bounds.height;
        
        sk_sp<SkData> uniformData = SkData::MakeWithCopy(&uniforms, sizeof(uniforms));
        
        sk_sp<SkShader> shader = effect_->makeShader(std::move(uniformData), nullptr, 0);

        SkPaint paint;
        paint.setShader(shader);
        paint.setAntiAlias(false); // No need for AA on full screen shader

        sk_canvas->save();
        sk_canvas->translate(bounds.x, bounds.y);
        
        SkRect local_bounds = SkRect::MakeWH(bounds.width, bounds.height);
        sk_canvas->drawRect(local_bounds, paint);
        
        sk_canvas->restore();
    }
};

class ShaderBackgroundGraphicsWidget : public SingleChildRenderObjectWidget {
public:
    ShaderBackgroundConfig config;
    float time;

    ShaderBackgroundGraphicsWidget(ShaderBackgroundConfig cfg, float t) 
        : config(std::move(cfg)), time(t) {
        this->child = nullptr;
    }

    [[nodiscard]] std::string_view typeName() const override { return "ShaderBackgroundGraphics"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderShaderBackground>();
        updateRenderObject(ctx, *obj);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override {
        auto& rs = static_cast<RenderShaderBackground&>(renderObject);
        rs.config = config;
        rs.time = time;
        
        rs.markNeedsPaint();
    }
};

class ShaderBackgroundState : public State {
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
        auto* sb = static_cast<const ShaderBackgroundWidget*>(widget());
        return std::make_shared<ShaderBackgroundGraphicsWidget>(sb->config, current_time_);
    }
};

std::unique_ptr<State> ShaderBackgroundWidget::createState() {
    return std::make_unique<ShaderBackgroundState>();
}

} // namespace vaura
