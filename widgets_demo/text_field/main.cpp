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
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E1E1E,
            .children = {
                Column({
                    .alignItems = YGAlignCenter,
                    .width = 400.0f,
                    .padding = std::pair{YGEdgeAll, 40.0f},
                    .backgroundColor = 0xFF2D2D30,
                    .borderRadius = 20.0f,
                    .children = {
                        // Username Field
                        Column({
                            .widthPercent = 100.0f,
                            .margin = std::pair{YGEdgeBottom, 20.0f},
                            .children = {
                                TextField({
                                    .placeholder = "Enter your username",
                                    .on_changed = [this](std::string_view v) { username = std::string(v); },
                                    .bg_color = 0xFF1E1E1E,
                                    .text_color = 0xFFFFFFFF,
                                    .cursor_color = 0xFF007ACC,
                                    .border_radius = 10.0f,
                                    .padding = {16.0f, 16.0f, 0.0f, 0.0f},
                                    .height = 50.0f,
                                    .width_percent = 100.0f
                                })
                            }
                        }),

                        // Password Field
                        Column({
                            .widthPercent = 100.0f,
                            .margin = std::pair{YGEdgeBottom, 40.0f},
                            .children = {
                                TextField({
                                    .placeholder = "Enter your password",
                                    .on_changed = [this](std::string_view v) { password = std::string(v); },
                                    .on_submitted = [this](std::string_view v) {
                                        std::cout << "Form submitted: " << username << " / " << password << std::endl;
                                    },
                                    .bg_color = 0xFF1E1E1E,
                                    .text_color = 0xFFFFFFFF,
                                    .cursor_color = 0xFF007ACC,
                                    .border_radius = 10.0f,
                                    .padding = {16.0f, 16.0f, 0.0f, 0.0f},
                                    .height = 50.0f,
                                    .width_percent = 100.0f
                                })
                            }
                        }),

                        // Login Button
                        Column({
                            .widthPercent = 100.0f,
                            .height = 50.0f,
                            .children = {
                                Button({
                                    .child = Text("Login", {.font_size = 16.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                                    .on_pressed = [this]() {
                                        std::cout << "Login Button Tapped: " << username << " / " << password << std::endl;
                                    },
                                    .color = 0xFF007ACC,
                                    .hover_color = 0xFF005A9E,
                                    .border_radius = 10.0f
                                })
                            }
                        })
                    }
                })
            }
        });
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
