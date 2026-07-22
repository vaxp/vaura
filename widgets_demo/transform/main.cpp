// widgets_demo/transform/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class TransformDemoState : public State {
    float rot_val = 0.0f;
    float scale_val = 1.0f;
    float trans_x = 0.0f;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto spacer_y = [](float h) {
            return Column({.height = h});
        };

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Transform Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                
                // Control Panel
                Column({
                    .width = 400.0f,
                    .padding = std::pair{YGEdgeAll, 20.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 12.0f,
                    .children = {
                        Text("Rotation", {.color = 0xFFAAAAAA}),
                        spacer_y(10.0f),
                        Slider({
                            .value = rot_val,
                            .min_value = 0.0f,
                            .max_value = 360.0f,
                            .on_changed = [this](float v) { setState([this, v]{ rot_val = v; }); },
                            .width = 360.0f
                        }),
                        spacer_y(20.0f),

                        Text("Scale", {.color = 0xFFAAAAAA}),
                        spacer_y(10.0f),
                        Slider({
                            .value = scale_val,
                            .min_value = 0.5f,
                            .max_value = 2.0f,
                            .on_changed = [this](float v) { setState([this, v]{ scale_val = v; }); },
                            .width = 360.0f
                        }),
                        spacer_y(20.0f),

                        Text("Translate X", {.color = 0xFFAAAAAA}),
                        spacer_y(10.0f),
                        Slider({
                            .value = trans_x,
                            .min_value = -100.0f,
                            .max_value = 100.0f,
                            .on_changed = [this](float v) { setState([this, v]{ trans_x = v; }); },
                            .width = 360.0f
                        }),
                        spacer_y(20.0f)
                    }
                }),

                Column({.height = 60.0f}),

                // The target to transform
                Transform({
                    .scale_x = scale_val,
                    .scale_y = scale_val,
                    .translate_x = trans_x,
                    .translate_y = 0.0f,
                    .rotation = rot_val,
                    .child = Column({
                        .justifyContent = YGJustifyCenter,
                        .alignItems = YGAlignCenter,
                        .width = 100.0f,
                        .height = 100.0f,
                        .backgroundColor = 0xFF3B82F6, // Blue
                        .borderRadius = 16.0f,
                        .children = {
                            Text("Box", {.font_size = 24.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                        }
                    })
                })
            }
        });
    }
};

class TransformDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "TransformDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<TransformDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Transform Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<TransformDemo>(), config);
}
