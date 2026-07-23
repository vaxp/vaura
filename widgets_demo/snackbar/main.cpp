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
        std::vector<WidgetPtr> children = {
            Column({
                .justify = Justify::Center,
                .align = Align::Center,
                .widthPercent = 100.0f,
                .heightPercent = 100.0f,
                .backgroundColor = 0xFF0F172A,
                .children = {
                    Text("Snackbar Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                    Column({.height = 40.0f}),
                    Button({
                        .child = Text("Show Snackbar", {.font_size = 16.0f, .color = 0xFFFFFFFF, .weight = TextStyle::SemiBold}),
                        .on_pressed = [this]() {
                            setState([this]() { show_snackbar = true; });
                        },
                        .color = 0xFF3B82F6,
                        .border_radius = 8.0f
                    })
                }
            })
        };

        if (show_snackbar) {
            children.push_back(
                Column({
                    .justify = Justify::End,
                    .align = Align::Center,
                    .widthPercent = 100.0f,
                    .heightPercent = 100.0f,
                    .padding = std::pair{Edge::Bottom, 32.0f},
                    .children = {
                        Snackbar({
                            .message = "Item successfully deleted.",
                            .action_label = "UNDO",
                            .on_action = [this]() {
                                std::cout << "Undo clicked!" << std::endl;
                                setState([this]() { show_snackbar = false; });
                            },
                            .on_dismissed = [this]() {
                                setState([this]() { show_snackbar = false; });
                            }
                        })
                    }
                })
            );
        }

        return Stack({
            .alignment = Alignment::Center,
            .children = children
        });
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
