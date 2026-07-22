// widgets_demo/text_field/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/text_field.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class TextFieldDemoState : public State {
    std::string username = "";
    std::string password = "";
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF1E1E1E)
            .widthPercent(100).heightPercent(100);

        auto card = std::make_shared<FlexBox>();
        card->flexDirection(YGFlexDirectionColumn)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF2D2D30)
            .padding(YGEdgeAll, 40)
            .borderRadius(20)
            .width(400);

        // Username Field
        TextFieldConfig user_cfg;
        user_cfg.placeholder = "Enter your username";
        user_cfg.bg_color = 0xFF1E1E1E;
        user_cfg.text_color = 0xFFFFFFFF;
        user_cfg.cursor_color = 0xFF007ACC;
        user_cfg.border_radius = 10.0f;
        user_cfg.width_percent = 100.0f;
        user_cfg.height = 50.0f;
        user_cfg.padding = {16.0f, 16.0f, 0.0f, 0.0f}; // Optional: left padding
        user_cfg.on_changed = [this](std::string_view v) { username = std::string(v); };

        auto user_wrapper = std::make_shared<FlexBox>();
        user_wrapper->widthPercent(100).margin(YGEdgeBottom, 20).child(TextField(user_cfg));
        card->child(user_wrapper);

        // Password Field
        TextFieldConfig pass_cfg;
        pass_cfg.placeholder = "Enter your password";
        pass_cfg.bg_color = 0xFF1E1E1E;
        pass_cfg.text_color = 0xFFFFFFFF;
        pass_cfg.cursor_color = 0xFF007ACC;
        pass_cfg.border_radius = 10.0f;
        pass_cfg.width_percent = 100.0f;
        pass_cfg.height = 50.0f;
        pass_cfg.padding = {16.0f, 16.0f, 0.0f, 0.0f};
        pass_cfg.on_changed = [this](std::string_view v) { password = std::string(v); };
        pass_cfg.on_submitted = [this](std::string_view v) { 
            std::cout << "Form submitted: " << username << " / " << password << std::endl;
        };

        auto pass_wrapper = std::make_shared<FlexBox>();
        pass_wrapper->widthPercent(100).margin(YGEdgeBottom, 40).child(TextField(pass_cfg));
        card->child(pass_wrapper);

        // Login Button
        ButtonConfig btn_cfg;
        btn_cfg.child = Text("Login", {.font_size = 16.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold});
        btn_cfg.color = 0xFF007ACC;
        btn_cfg.hover_color = 0xFF005A9E; // Slightly darker on hover
        btn_cfg.border_radius = 10.0f;
        btn_cfg.on_pressed = [this]() {
            std::cout << "Login Button Tapped: " << username << " / " << password << std::endl;
        };

        auto btn_wrapper = std::make_shared<FlexBox>();
        btn_wrapper->widthPercent(100).height(50).child(Button(btn_cfg));
        card->child(btn_wrapper);

        root->child(card);
        return root;
    }
};

class TextFieldDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "TextFieldDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<TextFieldDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA TextField Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<TextFieldDemo>(), config);
}
