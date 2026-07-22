#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/text_field.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/switch.hpp"
#include <memory>
#include <iostream>

using namespace vaura;

class LoginFormApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "LoginFormApp"; }
    std::unique_ptr<State> createState() override;
};

class LoginFormAppState : public State {
private:
    std::string username_;
    std::string password_;
    bool remember_me_ = false;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A); // Dark slate background

        auto card = std::make_shared<FlexBox>();
        card->width(350)
            .padding(YGEdgeAll, 32)
            .backgroundColor(0xFF1E293B) // Lighter slate
            .borderRadius(16)
            .flexDirection(YGFlexDirectionColumn);

        auto title = text("Welcome Back", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });

        auto subtitle = text("Sign in to your account", {
            .font_size = 14,
            .color = 0xFF94A3B8,
            .weight = TextStyle::Regular
        });

        auto gap_small = std::make_shared<FlexBox>(); gap_small->height(8).width(1);
        auto gap_medium = std::make_shared<FlexBox>(); gap_medium->height(16).width(1);
        auto gap_large = std::make_shared<FlexBox>(); gap_large->height(32).width(1);

        auto username_field = text_field({
            .placeholder = "Username or Email",
            .on_changed = [this](std::string_view text) { username_ = text; },
            .bg_color = 0x00000000, // Transparent, wrapper handles background
            .text_color = 0xFFF8FAFC,
            .placeholder_color = 0xFF64748B,
            .cursor_color = 0xFF38BDF8, // Sky blue
            .font_size = 15,
            .border_radius = 8,
            .width_percent = 100
        });

        auto user_container = std::make_shared<FlexBox>();
        user_container->widthPercent(100)
                      .backgroundColor(0xFF0F172A) // Darker background for the field
                      .borderRadius(8)
                      .child(username_field);

        auto password_field = text_field({
            .placeholder = "Password",
            .on_changed = [this](std::string_view text) { password_ = text; },
            .bg_color = 0x00000000, // Transparent
            .text_color = 0xFFF8FAFC,
            .placeholder_color = 0xFF64748B,
            .cursor_color = 0xFF38BDF8, // Sky blue
            .font_size = 15,
            .border_radius = 8,
            .width_percent = 100
        });

        auto pass_container = std::make_shared<FlexBox>();
        pass_container->widthPercent(100)
                      .backgroundColor(0xFF0F172A)
                      .borderRadius(8)
                      .child(password_field);

        auto remember_row = std::make_shared<FlexBox>();
        remember_row->widthPercent(100)
                    .flexDirection(YGFlexDirectionRow)
                    .alignItems(YGAlignCenter);

        auto remember_switch = toggle_switch({
            .value = remember_me_,
            .on_changed = [this](bool val) {
                setState([this, val] {
                    remember_me_ = val;
                });
            }
        });

        auto remember_text = text("Remember me", {
            .font_size = 14,
            .color = 0xFF94A3B8
        });

        auto remember_text_container = std::make_shared<FlexBox>();
        remember_text_container->margin(YGEdgeRight, 16)
                               .child(remember_text);

        remember_row->child(remember_text_container)
                    .child(remember_switch);

        auto login_btn = button({
            .child = text("Sign In", {
                .font_size = 16,
                .color = 0xFF0F172A,
                .weight = TextStyle::Bold
            }),
            .on_pressed = [this]() {
                std::cout << "Attempting login with user: " << username_ << "\n";
            },
            .color = 0xFF38BDF8, // Sky blue
            .hover_color = 0xFF0EA5E9,
            .pressed_color = 0xFF0284C7,
            .border_radius = 8,
            .width_percent = 100
        });

        card->child(title);
        card->child(gap_small);
        card->child(subtitle);
        card->child(gap_large);
        card->child(user_container);
        card->child(gap_medium);
        card->child(pass_container);
        card->child(gap_medium);
        card->child(remember_row);
        card->child(gap_large);
        card->child(login_btn);

        root->child(card);

        return root;
    }
};

std::unique_ptr<State> LoginFormApp::createState() {
    return std::make_unique<LoginFormAppState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Login Form - VAURA",
        .width = 1000,
        .height = 700,
        .resizable = true,
        .vsync = true,
        .msaa = true,
        .clear_color = 0xFF0F172A
    };

    return vaura::runApp(std::make_shared<LoginFormApp>(), config);
}
