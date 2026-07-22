// widgets_demo/stepper/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/stepper.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class StepperDemoState : public State {
    int current_step = 0;
public:
    WidgetPtr build(BuildContext& ctx) override {
        // Step 1 Content
        auto step1 = Column({
            .padding = std::pair{YGEdgeAll, 16.0f},
            .children = { Text("Enter your personal details here.", {.color = 0xFF94A3B8}) }
        });

        // Step 2 Content
        auto step2 = Column({
            .padding = std::pair{YGEdgeAll, 16.0f},
            .children = { Text("Select your subscription plan.", {.color = 0xFF94A3B8}) }
        });

        // Step 3 Content
        auto step3 = Column({
            .padding = std::pair{YGEdgeAll, 16.0f},
            .children = { Text("Review and confirm your choices.", {.color = 0xFF94A3B8}) }
        });

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .padding = std::pair{YGEdgeAll, 32.0f},
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Stepper Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                Column({
                    .width = 600.0f,
                    .height = 400.0f,
                    .padding = std::pair{YGEdgeAll, 16.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 16.0f,
                    .children = {
                        Stepper({
                            .steps = {
                                {"Account", "Personal Info", step1, false},
                                {"Plan", "Select tier", step2, false},
                                {"Confirm", "Final review", step3, false}
                            },
                            .current_step = current_step,
                            .on_step_changed = [this](int new_step) {
                                setState([this, new_step]() { current_step = new_step; });
                            },
                            .on_complete = [this]() {
                                std::cout << "Stepper completed!" << std::endl;
                            },
                            .button_padding = EdgeInsets(16, 40, 16, 40),
                            .button_font_size = 18.0f,
                            .button_width = 120.0f,
                            .button_height = 50.0f
                        })
                    }
                })
            }
        });
    }
};

class StepperDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "StepperDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<StepperDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Stepper Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<StepperDemo>(), config);
}
