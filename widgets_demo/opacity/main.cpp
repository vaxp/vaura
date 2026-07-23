// widgets_demo/opacity/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class OpacityDemoState : public State {
    float current_opacity = 1.0f;

public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .margin = std::pair{Edge::Bottom, 40.0f},
                    .children = {
                        Text("Opacity Control", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                }),
                
                // Control Panel
                Column({
                    .margin = std::pair{Edge::Bottom, 40.0f},
                    .children = {
                        Slider({
                            .value = current_opacity,
                            .min_value = 0.0f,
                            .max_value = 1.0f,
                            .on_changed = [this](float v) {
                                setState([this, v]() { current_opacity = v; });
                            },
                            .width = 300.0f,
                            .active_color = 0xFF10B981
                        })
                    }
                }),

                // Target content inside opacity widget
                Opacity(current_opacity, Card({
                    .color = 0xFF1E293B,
                    .border_radius = 20.0f,
                    .elevation = 10.0f,
                    .padding = {20, 20, 20, 20}
                }, Column({
                    .align = Align::Center,
                    .children = {
                        Column({
                            .justify = Justify::Center,
                            .align = Align::Center,
                            .width = 200.0f,
                            .height = 200.0f,
                            .backgroundColor = 0xFF3B82F6, // Blue
                            .borderRadius = 20.0f,
                            .children = {
                                Text("Fading Box", {.font_size = 20.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                            }
                        })
                    }
                })))
            }
        });
    }
};

class OpacityDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "OpacityDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<OpacityDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Opacity Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<OpacityDemo>(), config);
}
