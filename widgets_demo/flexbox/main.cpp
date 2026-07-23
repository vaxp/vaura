// widgets_demo/flexbox/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class FlexBoxDemoState : public State {
    FlexDirection current_dir = FlexDirection::Row;
    Justify current_justify = Justify::SpaceEvenly;
    Align current_align = Align::Center;
    
public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B, // Slate 800
            .children = {
                // Control Panel
                Row({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .widthPercent = 100.0f,
                    .padding = std::pair{Edge::All, 20.0f},
                    .backgroundColor = 0xFF0F172A,
                    .children = {
                        Button({
                            .child = Text("Toggle Direction", {.font_size = 14.0f, .color = 0xFFFFFFFF}),
                            .on_pressed = [this]() {
                                setState([this]() {
                                    current_dir = (current_dir == FlexDirection::Row) ? FlexDirection::Column : FlexDirection::Row;
                                });
                            },
                            .color = 0xFF3B82F6
                        })
                    }
                }),
                
                // Demo Area
                createFlexBox({
                    .direction = current_dir,
                    .justify = current_justify,
                    .align = current_align,
                    .widthPercent = 100.0f,
                    .flexGrow = 1.0f,
                    .children = []() {
                        std::vector<WidgetPtr> boxes;
                        std::vector<Color> colors = {0xFFEF4444, 0xFFF59E0B, 0xFF10B981, 0xFF3B82F6, 0xFF8B5CF6};
                        for (int i = 0; i < 5; ++i) {
                            boxes.push_back(
                                Column({
                                    .justify = Justify::Center,
                                    .align = Align::Center,
                                    .width = 80.0f,
                                    .height = 80.0f,
                                    .margin = std::pair{Edge::All, 10.0f},
                                    .backgroundColor = colors[i],
                                    .borderRadius = 12.0f,
                                    .children = {
                                        Text(std::to_string(i + 1), {.font_size = 24.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                                    }
                                })
                            );
                        }
                        return boxes;
                    }()
                })
            }
        });
    }
};

class FlexBoxDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "FlexBoxDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<FlexBoxDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA FlexBox Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<FlexBoxDemo>(), config);
}
