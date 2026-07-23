// widgets_demo/resizable_panels/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/resizable_panels.hpp"

using namespace vaura;

class ResizablePanelsDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ResizablePanelsDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Resizable Panels Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 20.0f}),
                Column({
                    .width = 800.0f,
                    .height = 400.0f,
                    .borderRadius = 16.0f,
                    .overflow = Overflow::Hidden,
                    .children = {
                        ResizablePanels({
                            .panels = {
                                {
                                    Column({
                                        .justify = Justify::Center,
                                        .align = Align::Center,
                                        .widthPercent = 100.0f,
                                        .heightPercent = 100.0f,
                                        .backgroundColor = 0xFF1E293B,
                                        .children = { Text("Sidebar", {.font_size = 20.0f, .color = 0xFF3B82F6}) }
                                    }), 200.0f, 100.0f, 400.0f
                                },
                                {
                                    Column({
                                        .justify = Justify::Center,
                                        .align = Align::Center,
                                        .widthPercent = 100.0f,
                                        .heightPercent = 100.0f,
                                        .backgroundColor = 0xFF334155,
                                        .children = { Text("Main Editor", {.font_size = 20.0f, .color = 0xFF10B981}) }
                                    }), 400.0f, 200.0f, 0.0f
                                },
                                {
                                    Column({
                                        .justify = Justify::Center,
                                        .align = Align::Center,
                                        .widthPercent = 100.0f,
                                        .heightPercent = 100.0f,
                                        .backgroundColor = 0xFF475569,
                                        .children = { Text("Terminal", {.font_size = 20.0f, .color = 0xFFF59E0B}) }
                                    }), 200.0f, 100.0f, 0.0f
                                }
                            },
                            .axis = ResizablePanelsConfig::Axis::Horizontal,
                            .total_size = 800.0f
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Resizable Panels Demo";
    config.width = 1000;
    config.height = 700;
    return runApp(std::make_shared<ResizablePanelsDemo>(), config);
}
