#include "vaura/app/app.hpp"
#include "vaura/widgets/accordion.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class AccordionDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AccordionDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .alignItems = YGAlignStretch,
                    .width = 400.0f,
                    .children = {
                        Column({
                            .margin = std::pair{YGEdgeBottom, 20.0f},
                            .children = {
                                Text("FAQ", {
                                    .font_size = 28,
                                    .color = 0xFFFFFFFF,
                                    .weight = TextStyle::Bold
                                })
                            }
                        }),
                        Column({
                            .margin = std::pair{YGEdgeBottom, 10.0f},
                            .children = {
                                Accordion({
                                    .title = "What is VAURA?",
                                    .content = Text("VAURA is a next-generation retained-mode C++ GUI framework designed for building luxurious and highly animated user interfaces.", {
                                        .font_size = 15,
                                        .color = 0xFF94A3B8
                                    }),
                                    .initially_expanded = true,
                                    .background_color = 0xFF1E293B
                                })
                            }
                        }),
                        Column({
                            .children = {
                                Accordion({
                                    .title = "Is it hardware accelerated?",
                                    .content = Text("Yes! It utilizes Yoga for flexbox layouts, Skia for high-performance GPU-accelerated rendering, and an architecture inspired by modern declarative frameworks.", {
                                        .font_size = 15,
                                        .color = 0xFF94A3B8
                                    }),
                                    .initially_expanded = false,
                                    .background_color = 0xFF1E293B
                                })
                            }
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Accordion Demo",
        .width = 600,
        .height = 500,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<AccordionDemoApp>(), config);
}
