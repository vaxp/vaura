// widgets_demo/row/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"

using namespace vaura;

class RowDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "RowDemo"; }

    WidgetPtr createRow(YGJustify justify, const std::string& label) {
        return Column({
            .margin = std::pair{YGEdgeBottom, 32.0f},
            .children = {
                Text(label, {.font_size = 18.0f, .color = 0xFF94A3B8, .weight = TextStyle::Bold}),
                Row({
                    .justifyContent = justify,
                    .alignItems = YGAlignCenter,
                    .width = 600.0f,
                    .padding = std::pair{YGEdgeAll, 16.0f},
                    .margin = std::pair{YGEdgeTop, 12.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 8.0f,
                    .children = {
                        Button({
                            .child = Text("Item 1", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                            .color = 0xFF3B82F6
                        }),
                        (justify == YGJustifyFlexStart || justify == YGJustifyFlexEnd || justify == YGJustifyCenter) 
                            ? Column({.width = 10.0f}) : nullptr,
                        Button({
                            .child = Text("Item 2", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                            .color = 0xFF10B981
                        }),
                        (justify == YGJustifyFlexStart || justify == YGJustifyFlexEnd || justify == YGJustifyCenter) 
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
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Row & Flex Alignment Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                createRow(YGJustifyFlexStart, "MainAxisAlignment: Start"),
                createRow(YGJustifyCenter, "MainAxisAlignment: Center"),
                createRow(YGJustifySpaceBetween, "MainAxisAlignment: Space Between"),
                createRow(YGJustifySpaceEvenly, "MainAxisAlignment: Space Evenly")
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
