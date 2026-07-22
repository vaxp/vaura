#include "vaura/app/app.hpp"
#include "vaura/widgets/checkbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class CheckboxDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CheckboxDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class CheckboxDemoState : public State {
private:
    bool is_agreed_ = false;
    bool is_subscribed_ = true;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto build_labeled_checkbox = [this](const std::string& label, bool& value) {
            return GestureDetector({
                .child = Row({
                    .alignItems = YGAlignCenter,
                    .margin = std::pair{YGEdgeBottom, 20.0f},
                    .children = {
                        Column({
                            .margin = std::pair{YGEdgeRight, 15.0f},
                            .children = {
                                Checkbox({
                                    .value = value,
                                    .on_changed = [this, &value](bool new_val) {
                                        setState([&value, new_val] {
                                            value = new_val;
                                        });
                                    },
                                    .size = 24.0f,
                                    .active_color = 0xFF38BDF8,
                                    .inactive_color = 0xFF64748B,
                                    .checkmark_color = 0xFFFFFFFF,
                                    .border_radius = 6.0f,
                                    .stroke_width = 2.5f
                                })
                            }
                        }),
                        Text(label, {.font_size = 18.0f, .color = 0xFFE2E8F0})
                    }
                }),
                .on_tap = [this, &value]() {
                    setState([&value] { value = !value; });
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
                                Text("Checkbox Demo", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                            }
                        }),
                        build_labeled_checkbox("I agree to the Terms of Service", is_agreed_),
                        build_labeled_checkbox("Subscribe to newsletter", is_subscribed_)
                    }
                })
            }
        });
    }
};

std::unique_ptr<State> CheckboxDemoApp::createState() {
    return std::make_unique<CheckboxDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Checkbox Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<CheckboxDemoApp>(), config);
}
