#include "vaura/app/app.hpp"
#include "vaura/widgets/radio.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class RadioDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "RadioDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class RadioDemoState : public State {
private:
    int selected_option_ = 1;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto build_radio_option = [this](const std::string& label, int value) {
            return GestureDetector({
                .child = Row({
                    .alignItems = YGAlignCenter,
                    .margin = std::pair{YGEdgeBottom, 20.0f},
                    .children = {
                        Column({
                            .margin = std::pair{YGEdgeRight, 15.0f},
                            .children = {
                                Radio({
                                    .value = (selected_option_ == value),
                                    .on_changed = nullptr,
                                    .size = 24.0f,
                                    .active_color = 0xFF38BDF8,
                                    .inactive_color = 0xFF64748B,
                                    .stroke_width = 2.5f,
                                    .inner_padding = 4.0f
                                })
                            }
                        }),
                        Text(label, {.font_size = 18.0f, .color = 0xFFE2E8F0})
                    }
                }),
                .on_tap = [this, value]() {
                    setState([this, value] {
                        selected_option_ = value;
                    });
                }
            });
        };

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B,
            .children = {
                Column({
                    .padding = std::pair{YGEdgeAll, 40.0f},
                    .backgroundColor = 0xFF0F172A,
                    .borderRadius = 16.0f,
                    .children = {
                        Column({
                            .margin = std::pair{YGEdgeBottom, 30.0f},
                            .children = {
                                Text("Radio Button Demo", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                            }
                        }),
                        build_radio_option("Option 1", 1),
                        build_radio_option("Option 2", 2),
                        build_radio_option("Option 3", 3)
                    }
                })
            }
        });
    }
};

std::unique_ptr<State> RadioDemoApp::createState() {
    return std::make_unique<RadioDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Radio Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<RadioDemoApp>(), config);
}
