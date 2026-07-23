// widgets_demo/fab/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/fab.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/stack.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class FABDemoState : public State {
    int counter = 0;
public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                // Spacer top
                Column({ .flexGrow = 1.0f }),
                
                // Centered Counter
                Row({
                    .justify = Justify::Center,
                    .widthPercent = 100.0f,
                    .children = {
                        Text("Counter: " + std::to_string(counter), {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                }),

                // Bottom section (takes equal flex to stay centered, pushes FABs to bottom)
                Column({
                    .justify = Justify::End,
                    .widthPercent = 100.0f,
                    .flexGrow = 1.0f,
                    .children = {
                        Row({
                            .justify = Justify::SpaceBetween,
                            .widthPercent = 100.0f,
                            .padding = std::pair{Edge::All, 40.0f},
                            .children = {
                                // Extended FAB
                                FAB({
                                    .icon = Icon(Icons::Pencil, {.color = 0xFFFFFFFF}),
                                    .label = "Reset",
                                    .on_pressed = [this]() { setState([this]() { counter = 0; }); },
                                    .background_color = 0xFFEF4444 // Red 500
                                }),
                                // Regular FAB
                                FAB({
                                    .icon = Icon(Icons::Plus, {.color = 0xFFFFFFFF}),
                                    .on_pressed = [this]() { setState([this]() { counter++; }); },
                                    .background_color = 0xFF6366F1 // Indigo 500
                                })
                            }
                        })
                    }
                })
            }
        });
    }
};

class FABDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "FABDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<FABDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA FAB Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<FABDemo>(), config);
}
