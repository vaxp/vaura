// widgets_demo/toast/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/toast.hpp"
#include "vaura/widgets/stack.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ToastDemoState : public State {
    bool show_toast = false;
    ToastConfig::Position toast_pos = ToastConfig::Position::Bottom;
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
                    Text("Toast Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                    Column({.height = 40.0f}),
                    Row({
                        .justify = Justify::Center,
                        .children = {
                            Column({
                                .margin = std::pair{Edge::Right, 10.0f},
                                .children = {
                                    Button({
                                        .child = Text("Top Toast", {.color = 0xFFFFFFFF}),
                                        .on_pressed = [this]() {
                                            setState([this]() { show_toast = true; toast_pos = ToastConfig::Position::Top; });
                                        },
                                        .color = 0xFF3B82F6
                                    })
                                }
                            }),
                            Button({
                                .child = Text("Bottom Toast", {.color = 0xFFFFFFFF}),
                                .on_pressed = [this]() {
                                    setState([this]() { show_toast = true; toast_pos = ToastConfig::Position::Bottom; });
                                },
                                .color = 0xFF10B981
                            })
                        }
                    })
                }
            })
        };

        if (show_toast) {
            children.push_back(
                Column({
                    .widthPercent = 100.0f,
                    .heightPercent = 100.0f,
                    .children = {
                        Toast({
                            .message = "This is a toast notification!",
                            .on_dismissed = [this]() {
                                setState([this]() { show_toast = false; });
                            },
                            .position = toast_pos
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

class ToastDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ToastDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ToastDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Toast Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ToastDemo>(), config);
}
