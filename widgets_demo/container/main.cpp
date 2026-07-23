#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ContainerDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .gap = 60.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                Text("Container Showcase", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Row({
                    .align = Align::Center,
                    .gap = 40.0f,
                    .children = {
                        // 1. Pill Shape Container
                        Container({
                            .color = 0xFF3B82F6, // Blue 500
                            .border_radius = BorderRadius(40.0f), // Half of height
                            .shadow = Shadow{0x603B82F6, 24.0f, {0, 12}},
                            .child = Column({
                                .justify = Justify::Center,
                                .align = Align::Center,
                                .width = 200.0f,
                                .height = 80.0f,
                                .children = {
                                    Text("Pill Shape", {.font_size = 20.0f, .color = 0xFFFFFFFF})
                                }
                            })
                        }),

                        // 2. Glassmorphism-style Container
                        Container({
                            .color = 0xFF1E293B, // Slate 800
                            .border_radius = BorderRadius(24.0f),
                            .border = Border(0xFF334155, 2.0f),
                            .shadow = Shadow{0x80000000, 32.0f, {0, 16}},
                            .child = Column({
                                .justify = Justify::Center,
                                .align = Align::Center,
                                .width = 180.0f,
                                .height = 180.0f,
                                .children = {
                                    Text("Deep Shadow", {.font_size = 20.0f, .color = 0xFFFFFFFF})
                                }
                            })
                        }),

                        // 3. Clipped Image Container (Asymmetric radius)
                        Container({
                            .color = 0x00000000,
                            .border_radius = BorderRadius{60.0f, 16.0f, 60.0f, 16.0f}, // Asymmetric
                            .shadow = Shadow{0x508B5CF6, 20.0f, {0, 10}},
                            .clip_content = true,
                            .child = Column({
                                .justify = Justify::Center,
                                .align = Align::Center,
                                .width = 180.0f,
                                .height = 180.0f,
                                .backgroundColor = 0xFF8B5CF6, // Violet 500
                                .children = {
                                    Text("Asymmetric", {.font_size = 20.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                                }
                            })
                        })
                    }
                })
            }
        });
    }
};

class ContainerDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ContainerDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ContainerDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Container Demo";
    config.width = 900;
    config.height = 600;

    return runApp(std::make_shared<ContainerDemo>(), config);
}
