// widgets_demo/popup_menu/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/popup_menu.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class PopupMenuDemoState : public State {
    std::string action_text = "None";

public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Last Action: " + action_text, {.font_size = 24.0f, .color = 0xFF94A3B8}),
                Column({.height = 40.0f}),
                PopupMenu({
                    .anchor = Button({
                        .child = Text("Show Options", {.font_size = 14.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                        .color = 0xFF3B82F6 // Blue 500
                    }),
                    .items = {
                        {"Edit Profile", [this]() { setState([this]() { action_text = "Edit"; }); }},
                        {"Share", [this]() { setState([this]() { action_text = "Share"; }); }},
                        {.is_separator = true},
                        // Label, Callback, Submenu, Disabled, Danger
                        {"Delete Account", [this]() { setState([this]() { action_text = "Delete"; }); }, nullptr, false, true}
                    },
                    .placement = PopupMenuConfig::Placement::BelowLeft
                })
            }
        });
    }
};

class PopupMenuDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "PopupMenuDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<PopupMenuDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Popup Menu Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<PopupMenuDemo>(), config);
}
