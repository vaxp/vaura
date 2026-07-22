#include "vaura/app/app.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class CardDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CardDemo"; }

    WidgetPtr build(BuildContext& context) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Card({
                    .color = 0xFF1E293B, // Slate 800
                    .border_radius = 24.0f,
                    .elevation = 20.0f, // Deep shadow
                    .shadow_color = 0x80000000,
                    .shadow_dy = 10.0f,
                    .padding = EdgeInsets(40, 60, 40, 60)
                }, Column({
                    .justifyContent = YGJustifyFlexStart,
                    .alignItems = YGAlignCenter,
                    .gap = 16.0f,
                    .children = {
                        Text("VAURA Developer", {
                            .font_size = 24.0f,
                            .color = 0xFFFFFFFF
                        }),
                        Text("C++ UI Framework", {
                            .font_size = 16.0f,
                            .color = 0xFF94A3B8 // Slate 400
                        }),
                        Button({
                            .child = Text("Follow", {
                                .font_size = 16.0f,
                                .color = 0xFFFFFFFF
                            }),
                            .color = 0xFF8B5CF6, // Violet 500
                            .hover_color = 0xFF7C3AED, // Violet 600
                            .border_radius = 20.0f,
                            .padding = EdgeInsets(12, 32, 12, 32)
                        })
                    }
                }))
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Card Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<CardDemo>(), config);
}
