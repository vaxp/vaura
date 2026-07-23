// widgets_demo/stack/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/stack.hpp"

using namespace vaura;

class StackDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "StackDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Stack Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                Stack({
                    .alignment = Alignment::Center,
                    .children = {
                        // Base
                        Column({
                            .justify = Justify::End,
                            .align = Align::End,
                            .width = 300.0f,
                            .height = 300.0f,
                            .padding = std::pair{Edge::All, 16.0f},
                            .backgroundColor = 0xFF1E293B,
                            .borderRadius = 16.0f,
                            .children = { Text("Base", {.font_size = 18.0f, .color = 0xFF94A3B8}) }
                        }),
                        // Middle
                        Column({
                            .justify = Justify::End,
                            .align = Align::End,
                            .width = 200.0f,
                            .height = 200.0f,
                            .padding = std::pair{Edge::All, 16.0f},
                            .backgroundColor = 0xFF3B82F6,
                            .borderRadius = 12.0f,
                            .children = { Text("Middle", {.font_size = 18.0f, .color = 0xFFFFFFFF}) }
                        }),
                        // Top
                        Column({
                            .justify = Justify::End,
                            .align = Align::End,
                            .width = 100.0f,
                            .height = 100.0f,
                            .padding = std::pair{Edge::All, 8.0f},
                            .backgroundColor = 0xFFEF4444,
                            .borderRadius = 8.0f,
                            .children = { Text("Top", {.font_size = 16.0f, .color = 0xFFFFFFFF}) }
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Stack Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<StackDemo>(), config);
}
