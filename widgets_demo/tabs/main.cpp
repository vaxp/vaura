#include "vaura/app/app.hpp"
#include "vaura/widgets/tabs.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class TabsDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "TabsDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B,
            .children = {
                Column({
                    .alignItems = YGAlignStretch,
                    .width = 500.0f,
                    .height = 400.0f, // Fixed height to test tab content flexibility
                    .children = {
                        Column({
                            .margin = std::pair{YGEdgeBottom, 20.0f},
                            .children = {
                                Text("Dashboard", {
                                    .font_size = 28,
                                    .color = 0xFFFFFFFF,
                                    .weight = TextStyle::Bold
                                })
                            }
                        }),
                        Column({
                            .flexGrow = 1.0f,
                            .children = {
                                Tabs({
                                    .tabs = {
                                        {"Overview", Column({
                                            .justifyContent = YGJustifyCenter,
                                            .alignItems = YGAlignCenter,
                                            .flexGrow = 1.0f,
                                            .backgroundColor = 0xFF0F172A,
                                            .borderRadius = 12.0f,
                                            .children = { Text("Overview Dashboard Content", {.font_size = 18, .color = 0xFFF8FAFC}) }
                                        })},
                                        {"Analytics", Column({
                                            .justifyContent = YGJustifyCenter,
                                            .alignItems = YGAlignCenter,
                                            .flexGrow = 1.0f,
                                            .backgroundColor = 0xFF0F172A,
                                            .borderRadius = 12.0f,
                                            .children = { Text("Analytics Charts Here", {.font_size = 18, .color = 0xFFF8FAFC}) }
                                        })},
                                        {"Settings", Column({
                                            .justifyContent = YGJustifyCenter,
                                            .alignItems = YGAlignCenter,
                                            .flexGrow = 1.0f,
                                            .backgroundColor = 0xFF0F172A,
                                            .borderRadius = 12.0f,
                                            .children = { Text("System Settings", {.font_size = 18, .color = 0xFFF8FAFC}) }
                                        })}
                                    },
                                    .initial_index = 0
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
        .title = "VAURA Tabs Demo",
        .width = 700,
        .height = 600,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<TabsDemoApp>(), config);
}
