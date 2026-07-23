#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/color_picker.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"
#include <iomanip>
#include <sstream>

using namespace vaura;

class ColorPickerDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ColorPickerDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ColorPickerDemoState : public State {
    Color current_color = 0xFF0EA5E9; // Sky Blue 500

    std::string colorToHex(Color color) {
        std::stringstream ss;
        ss << "#" << std::uppercase << std::hex << std::setfill('0') << std::setw(8) << color;
        return ss.str();
    }

public:
    WidgetPtr build(BuildContext& context) override {
        return Row({
            .justify = Justify::Center,
            .align = Align::Center,
            .gap = 80.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                // Left Panel (Color Picker)
                Column({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .gap = 24.0f,
                    .children = {
                        Text("Pick a Color", {.font_size = 28.0f, .color = 0xFFFFFFFF}),
                        ColorPicker({
                            .selected_color = current_color,
                            .on_changed = [this](Color new_color) {
                                setState([this, new_color]() {
                                    current_color = new_color;
                                });
                            },
                            .show_alpha = true,
                            .show_hex_input = true,
                            .width = 300.0f,
                            .background_color = 0xFF1E293B // Slate 800
                        })
                    }
                }),

                // Right Panel (Preview Area)
                Column({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .gap = 24.0f,
                    .children = {
                        Column({
                            .width = 200.0f,
                            .height = 200.0f,
                            .backgroundColor = current_color,
                            .borderRadius = 24.0f
                        }),
                        Text(colorToHex(current_color), {.font_size = 24.0f, .color = current_color})
                    }
                })
            }
        });
    }
};

std::unique_ptr<State> ColorPickerDemo::createState() {
    return std::make_unique<ColorPickerDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Color Picker Showcase",
        .width = 900,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<ColorPickerDemo>(), config);
}
