#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ColumnDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto make_box = [](const std::string& text_val, Color color) {
            return Container({
                .color = color,
                .border_radius = BorderRadius(8.0f),
                .shadow = Shadow{0x40000000, 8.0f, {0, 4}},
                .child = Column({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .width = 100.0f,
                    .height = 60.0f,
                    .children = {
                        Text(text_val, {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                })
            });
        };

        return Row({
            .justify = Justify::SpaceEvenly,
            .align = Align::Stretch,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .padding = std::pair{Edge::All, 40.0f},
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                // Column 1: FlexStart (Left/Top aligned)
                Column({
                    .align = Align::Start,
                    .gap = 16.0f,
                    .width = 200.0f,
                    .padding = std::pair{Edge::All, 20.0f},
                    .backgroundColor = 0xFF1E293B, // Slate 800
                    .borderRadius = 16.0f,
                    .children = {
                        Column({
                            .margin = std::pair{Edge::Bottom, 10.0f},
                            .children = { Text("Flex Start", {.font_size = 20.0f, .color = 0xFF94A3B8}) }
                        }),
                        make_box("Box 1", 0xFF3B82F6), // Blue
                        make_box("Box 2", 0xFF8B5CF6), // Violet
                        make_box("Box 3", 0xFFEC4899)  // Pink
                    }
                }),

                // Column 2: Center with SpaceBetween
                Column({
                    .justify = Justify::SpaceBetween,
                    .align = Align::Center,
                    .width = 200.0f,
                    .padding = std::pair{Edge::All, 20.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 16.0f,
                    .children = {
                        Column({
                            .children = { Text("Space Between", {.font_size = 20.0f, .color = 0xFF94A3B8}) }
                        }),
                        make_box("Box A", 0xFF10B981), // Emerald
                        make_box("Box B", 0xFFF59E0B), // Amber
                        make_box("Box C", 0xFFEF4444)  // Red
                    }
                }),

                // Column 3: FlexEnd (Right aligned)
                Column({
                    .align = Align::End,
                    .gap = 16.0f,
                    .width = 200.0f,
                    .padding = std::pair{Edge::All, 20.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 16.0f,
                    .children = {
                        Column({
                            .margin = std::pair{Edge::Bottom, 10.0f},
                            .children = { Text("Flex End", {.font_size = 20.0f, .color = 0xFF94A3B8}) }
                        }),
                        make_box("Alpha", 0xFF06B6D4), // Cyan
                        make_box("Beta", 0xFF6366F1),  // Indigo
                        make_box("Gamma", 0xFF14B8A6)  // Teal
                    }
                })
            }
        });
    }
};

class ColumnDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ColumnDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ColumnDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Column Demo";
    config.width = 800;
    config.height = 600;

    return runApp(std::make_shared<ColumnDemo>(), config);
}
