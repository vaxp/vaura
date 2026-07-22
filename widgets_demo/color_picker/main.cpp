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
        // Main Background Body
        auto body = std::make_shared<FlexBox>();
        body->flexDirection(YGFlexDirectionRow)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .gap(YGGutterAll, 80.0f)
            .widthPercent(100.0f)
            .heightPercent(100.0f)
            .backgroundColor(0xFF0F172A); // Slate 900

        // Left Panel (Color Picker)
        auto picker_panel = std::make_shared<FlexBox>();
        picker_panel->flexDirection(YGFlexDirectionColumn)
                    .justifyContent(YGJustifyCenter)
                    .alignItems(YGAlignCenter)
                    .gap(YGGutterAll, 24.0f);
        
        TextConfig title_cfg;
        title_cfg.font_size = 28.0f;
        title_cfg.color = 0xFFFFFFFF;
        picker_panel->child(std::make_shared<TextWidget>("Pick a Color", title_cfg));

        ColorPickerConfig cp_cfg;
        cp_cfg.selected_color = current_color;
        cp_cfg.show_alpha = true;
        cp_cfg.show_hex_input = true;
        cp_cfg.background_color = 0xFF1E293B; // Slate 800
        cp_cfg.on_changed = [this](Color new_color) {
            setState([this, new_color]() {
                current_color = new_color;
            });
        };
        picker_panel->child(ColorPicker(cp_cfg));
        body->child(picker_panel);

        // Right Panel (Preview Area)
        auto preview_panel = std::make_shared<FlexBox>();
        preview_panel->flexDirection(YGFlexDirectionColumn)
                     .justifyContent(YGJustifyCenter)
                     .alignItems(YGAlignCenter)
                     .gap(YGGutterAll, 24.0f);

        auto preview_box = std::make_shared<FlexBox>();
        preview_box->width(200.0f)
                   .height(200.0f)
                   .borderRadius(24.0f)
                   .backgroundColor(current_color);
        preview_panel->child(preview_box);

        TextConfig hex_cfg;
        hex_cfg.font_size = 24.0f;
        hex_cfg.color = current_color;
        preview_panel->child(std::make_shared<TextWidget>(colorToHex(current_color), hex_cfg));
        body->child(preview_panel);

        return body;
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
