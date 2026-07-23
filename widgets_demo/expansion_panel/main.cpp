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
        auto make_content = [](const std::string& text_val) {
            return Column({
                .padding = std::pair{Edge::All, 16.0f},
                .children = { Text(text_val, {.font_size = 14.0f, .color = 0xFF94A3B8}) }
            });
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

        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .margin = std::pair{Edge::Bottom, 40.0f},
                    .children = {
                        Text("Settings FAQ", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                }),
                Column({
                    .width = 600.0f,
                    .children = {
                        ExpansionPanel(cfg)
                    }
                })
            }
        });
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
