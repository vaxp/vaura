#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/rendering/color.hpp"
#include <iostream>

using namespace vaura;

class SliderDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "SliderDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SliderDemoState : public State {
    float brightness = 0.5f;

public:
    WidgetPtr build(BuildContext& context) override {
        // Dynamically change background brightness based on slider
        uint8_t luma = static_cast<uint8_t>(brightness * 255);
        Color bg_color = (0xFF << 24) | (luma << 16) | (luma << 8) | luma;

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .gap = 40.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = bg_color,
            .children = {
                // Title
                Text("Brightness Control", {
                    .font_size = 32.0f,
                    .color = (luma > 128) ? 0xFF000000 : 0xFFFFFFFF
                }),

                // Slider
                Slider({
                    .value = brightness,
                    .min_value = 0.0f,
                    .max_value = 1.0f,
                    .on_changed = [this](float val) {
                        setState([this, val]() {
                            brightness = val;
                        });
                    },
                    .width = 300.0f,
                    .active_color = 0xFFF59E0B, // Amber 500
                    .inactive_color = 0xFF94A3B8, // Slate 400
                    .thumb_color = 0xFFFFFFFF
                }),

                // Value text
                Text(std::to_string(static_cast<int>(brightness * 100)) + "%", {
                    .font_size = 24.0f,
                    .color = (luma > 128) ? 0xFF334155 : 0xFFCBD5E1
                })
            }
        });
    }
};

std::unique_ptr<State> SliderDemo::createState() {
    return std::make_unique<SliderDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Slider Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<SliderDemo>(), config);
}
