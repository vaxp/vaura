// widgets_demo/row/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"

using namespace vaura;

class RowDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "RowDemo"; }

    WidgetPtr createRow(Justify justify, const std::string& label) {
        return Column({
            .margin = std::pair{Edge::Bottom, 32.0f},
            .children = {
                Text(label, {.font_size = 18.0f, .color = 0xFF94A3B8, .weight = TextStyle::Bold}),
                Row({
                    .justify = justify,
                    .align = Align::Center,
                    .width = 600.0f,
                    .padding = std::pair{Edge::All, 16.0f},
                    .margin = std::pair{Edge::Top, 12.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 8.0f,
                    .children = {
                        Button({
                            .child = Text("Item 1", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                            .color = 0xFF3B82F6
                        }),
                        (justify == Justify::Start || justify == Justify::End || justify == Justify::Center) 
                            ? Column({.width = 10.0f}) : nullptr,
                        Button({
                            .child = Text("Item 2", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                            .color = 0xFF10B981
                        }),
                        (justify == Justify::Start || justify == Justify::End || justify == Justify::Center) 
                            ? Column({.width = 10.0f}) : nullptr,
                        Button({
                            .child = Text("Item 3", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                            .color = 0xFFEF4444
                        })
                    }
                })
            }
        });
    }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Row & Flex Alignment Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                createRow(Justify::Start, "MainAxisAlignment: Start"),
                createRow(Justify::Center, "MainAxisAlignment: Center"),
                createRow(Justify::SpaceBetween, "MainAxisAlignment: Space Between"),
                createRow(Justify::SpaceEvenly, "MainAxisAlignment: Space Evenly")
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Row Demo";
    config.width = 800;
    config.height = 800;
    return runApp(std::make_shared<RowDemo>(), config);
}
