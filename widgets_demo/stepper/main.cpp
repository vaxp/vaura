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
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100)
            .padding(YGEdgeAll, 32);

        root->child(Text("Stepper Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        // Step 1 Content
        auto step1 = std::make_shared<FlexBox>();
        step1->padding(YGEdgeAll, 16);
        step1->child(Text("Enter your personal details here.", {.color = 0xFF94A3B8}));

        // Step 2 Content
        auto step2 = std::make_shared<FlexBox>();
        step2->padding(YGEdgeAll, 16);
        step2->child(Text("Select your subscription plan.", {.color = 0xFF94A3B8}));

        // Step 3 Content
        auto step3 = std::make_shared<FlexBox>();
        step3->padding(YGEdgeAll, 16);
        step3->child(Text("Review and confirm your choices.", {.color = 0xFF94A3B8}));

        StepperConfig stepper_cfg;
        stepper_cfg.current_step = current_step;
        stepper_cfg.steps = {
            {"Account", "Personal Info", step1, false},
            {"Plan", "Select tier", step2, false},
            {"Confirm", "Final review", step3, false}
        };

        // Customizing button sizes from the outside!
        stepper_cfg.button_padding = EdgeInsets(16, 40, 16, 40);
        stepper_cfg.button_font_size = 18.0f;
        stepper_cfg.button_width = 120.0f;
        stepper_cfg.button_height = 50.0f;

        stepper_cfg.on_step_changed = [this](int new_step) {
            setState([this, new_step]() { current_step = new_step; });
        };

        stepper_cfg.on_complete = [this]() {
            std::cout << "Stepper completed!" << std::endl;
        };

        auto stepper_wrapper = std::make_shared<FlexBox>();
        stepper_wrapper->width(600).height(400)
                       .backgroundColor(0xFF1E293B)
                       .borderRadius(16)
                       .padding(YGEdgeAll, 16)
                       .child(Stepper(stepper_cfg));

        root->child(stepper_wrapper);
        return root;
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
