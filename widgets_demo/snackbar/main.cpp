// widgets_demo/snackbar/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/snackbar.hpp"
#include "vaura/widgets/stack.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class SnackbarDemoState : public State {
    bool show_snackbar = false;
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionColumn)
               .justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter)
               .backgroundColor(0xFF0F172A)
               .widthPercent(100).heightPercent(100);

        content->child(text("Snackbar Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        content->child(spacer);

        ButtonConfig btn_cfg;
        btn_cfg.child = text("Show Snackbar", {.font_size = 16.0f, .color = 0xFFFFFFFF, .weight = TextStyle::SemiBold});
        btn_cfg.color = 0xFF3B82F6;
        btn_cfg.border_radius = 8.0f;
        btn_cfg.on_pressed = [this]() {
            setState([this]() { show_snackbar = true; });
        };

        content->child(button(btn_cfg));

        StackConfig stack_cfg;
        stack_cfg.alignment = Alignment::Center;
        stack_cfg.children.push_back(content);

        if (show_snackbar) {
            SnackbarConfig snack_cfg;
            snack_cfg.message = "Item successfully deleted.";
            snack_cfg.action_label = "UNDO";
            snack_cfg.on_action = [this]() {
                std::cout << "Undo clicked!" << std::endl;
                setState([this]() { show_snackbar = false; });
            };
            snack_cfg.on_dismissed = [this]() {
                setState([this]() { show_snackbar = false; });
            };
            
            auto snackbar_wrapper = std::make_shared<FlexBox>();
            snackbar_wrapper->widthPercent(100).heightPercent(100)
                            .justifyContent(YGJustifyFlexEnd)
                            .alignItems(YGAlignCenter)
                            .padding(YGEdgeBottom, 32);
            snackbar_wrapper->child(snackbar(snack_cfg));
            
            stack_cfg.children.push_back(snackbar_wrapper);
        }

        return stack(stack_cfg);
    }
};

class SnackbarDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "SnackbarDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<SnackbarDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Snackbar Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<SnackbarDemo>(), config);
}
