#include "vaura/app/app.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class ButtonDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ButtonDemo"; }

    WidgetPtr build(BuildContext& context) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .gap = 24.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                // Title
                Text("Button Showcase", {.font_size = 32.0f, .color = 0xFFF8FAFC}),

                // 1. Primary Button
                Button({
                    .child = Text("Primary Button", {.font_size = 18.0f, .color = 0xFFFFFFFF}),
                    .color = 0xFF3B82F6,
                    .hover_color = 0xFF2563EB,
                    .pressed_color = 0xFF1D4ED8,
                    .border_radius = 12.0f,
                    .padding = EdgeInsets(16, 32, 16, 32),
                    .elevation = 8.0f
                }),

                // 2. Success Button
                Button({
                    .child = Text("Success Pill", {.font_size = 18.0f, .color = 0xFFFFFFFF}),
                    .color = 0xFF10B981,
                    .hover_color = 0xFF059669,
                    .pressed_color = 0xFF047857,
                    .border_radius = 24.0f,
                    .padding = EdgeInsets(16, 40, 16, 40),
                    .elevation = 4.0f
                }),

                // 3. Outlined Button
                Button({
                    .child = Text("Outlined Button", {.font_size = 18.0f, .color = 0xFFF43F5E}),
                    .color = 0x00000000,
                    .hover_color = 0x20F43F5E,
                    .border_radius = 12.0f,
                    .padding = EdgeInsets(16, 32, 16, 32),
                    .border = Border{0xFFF43F5E, 2.0f},
                    .elevation = 0.0f
                }),

                // 4. Disabled Button
                Button({
                    .child = Text("Disabled State", {.font_size = 18.0f, .color = 0xFF94A3B8}),
                    .color = 0xFF334155,
                    .border_radius = 12.0f,
                    .padding = EdgeInsets(16, 32, 16, 32),
                    .elevation = 0.0f,
                    .enabled = false
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Button Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<ButtonDemo>(), config);
}
