// widgets_demo/expansion_panel/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/expansion_panel.hpp"
#include "vaura/state/state.hpp"
#include <string>

using namespace vaura;

class ExpansionPanelDemoState : public State {
    std::vector<bool> expanded = {true, false, false};
    
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        auto title = std::make_shared<FlexBox>();
        title->margin(YGEdgeBottom, 40)
             .child(text("Settings FAQ", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        root->child(title);

        auto make_content = [](const std::string& text_val) {
            auto txt = text(text_val, {.font_size = 14.0f, .color = 0xFF94A3B8});
            auto content_box = std::make_shared<FlexBox>();
            content_box->padding(YGEdgeAll, 16.0f).child(txt);
            return content_box;
        };

        ExpansionPanelConfig cfg;
        cfg.allow_multiple_expanded = false;
        
        cfg.items.push_back({
            "Account Settings",
            "Manage your personal info",
            make_content("Here you can change your email, password, and update your profile picture."),
            expanded[0]
        });
        cfg.items.push_back({
            "Privacy & Security",
            "Control who sees your data",
            make_content("Adjust your privacy settings to ensure your data is safe and shared only with people you trust."),
            expanded[1]
        });
        cfg.items.push_back({
            "Notifications",
            "Mute or enable alerts",
            make_content("Configure email, SMS, and push notifications to stay updated without being overwhelmed."),
            expanded[2]
        });
        
        cfg.on_expansion_changed = [this](int idx, bool is_exp) {
            setState([this, idx, is_exp]() {
                for (size_t i = 0; i < expanded.size(); ++i) {
                    expanded[i] = false;
                }
                expanded[idx] = is_exp;
            });
        };
        
        auto panel_wrap = std::make_shared<FlexBox>();
        panel_wrap->width(600).child(expansion_panel(cfg));

        root->child(panel_wrap);
        return root;
    }
};

class ExpansionPanelDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ExpansionPanelDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ExpansionPanelDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Expansion Panel Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ExpansionPanelDemo>(), config);
}
