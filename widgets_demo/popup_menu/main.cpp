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
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(text("Last Action: " + action_text, {.font_size = 24.0f, .color = 0xFF94A3B8}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        ButtonConfig btn_cfg;
        btn_cfg.child = text("Show Options", {.font_size = 14.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold});
        btn_cfg.color = 0xFF3B82F6; // Blue 500
        
        auto anchor_btn = button(btn_cfg);

        PopupMenuConfig pm_cfg;
        pm_cfg.anchor = anchor_btn;
        pm_cfg.placement = PopupMenuConfig::Placement::BelowLeft;
        
        pm_cfg.items.push_back({"Edit Profile", [this]() { setState([this]() { action_text = "Edit"; }); }});
        pm_cfg.items.push_back({"Share", [this]() { setState([this]() { action_text = "Share"; }); }});
        pm_cfg.items.push_back({.is_separator = true});
        // Label, Callback, Submenu, Disabled, Danger
        pm_cfg.items.push_back({"Delete Account", [this]() { setState([this]() { action_text = "Delete"; }); }, nullptr, false, true});

        root->child(popup_menu(pm_cfg));
        return root;
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
