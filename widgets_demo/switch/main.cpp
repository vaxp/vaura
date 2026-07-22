#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/switch.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class SwitchDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "SwitchDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SwitchDemoState : public State {
    bool wifi_enabled = true;
    bool bluetooth_enabled = false;
    bool airplane_mode = false;

public:
    WidgetPtr build(BuildContext& context) override {
        // Main layout (Centered Column)
        auto col = std::make_shared<FlexBox>();
        col->flexDirection(YGFlexDirectionColumn)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 40.0f)
           .widthPercent(100.0f)
           .heightPercent(100.0f)
           .backgroundColor(0xFF0F172A); // Slate 900

        // Title
        TextConfig title_cfg;
        title_cfg.font_size = 32.0f;
        title_cfg.color = 0xFFFFFFFF; 
        col->child(std::make_shared<TextWidget>("Settings", title_cfg));

        // Settings Card Layout
        auto list = std::make_shared<FlexBox>();
        list->flexDirection(YGFlexDirectionColumn)
            .gap(YGGutterAll, 24.0f)
            .backgroundColor(0xFF1E293B) // Slate 800
            .borderRadius(16.0f)
            .padding(YGEdgeTop, 24.0f)
            .padding(YGEdgeBottom, 24.0f)
            .padding(YGEdgeLeft, 32.0f)
            .padding(YGEdgeRight, 32.0f);

        // 1. Wi-Fi
        list->child(buildSettingRow("Wi-Fi", wifi_enabled, [this](bool val) {
            setState([this, val]() { wifi_enabled = val; });
        }, 0xFF3B82F6)); // Blue

        // 2. Bluetooth
        list->child(buildSettingRow("Bluetooth", bluetooth_enabled, [this](bool val) {
            setState([this, val]() { bluetooth_enabled = val; });
        }, 0xFF8B5CF6)); // Violet

        // 3. Airplane Mode
        list->child(buildSettingRow("Airplane Mode", airplane_mode, [this](bool val) {
            setState([this, val]() { 
                airplane_mode = val; 
                if (val) {
                    wifi_enabled = false;
                    bluetooth_enabled = false;
                }
            });
        }, 0xFFF59E0B)); // Amber

        col->child(list);
        return col;
    }

private:
    WidgetPtr buildSettingRow(const std::string& label, bool value, std::function<void(bool)> on_change, Color active_color) {
        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .justifyContent(YGJustifySpaceBetween)
           .alignItems(YGAlignCenter)
           .width(300.0f);

        TextConfig text_cfg;
        text_cfg.font_size = 20.0f;
        text_cfg.color = 0xFFF8FAFC;
        row->child(std::make_shared<TextWidget>(label, text_cfg));

        SwitchConfig sw_cfg;
        sw_cfg.value = value;
        sw_cfg.active_color = active_color;
        sw_cfg.inactive_color = 0xFF475569; // Slate 600
        sw_cfg.on_changed = on_change;
        row->child(toggle_switch(sw_cfg));

        return row;
    }
};

std::unique_ptr<State> SwitchDemo::createState() {
    return std::make_unique<SwitchDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Switch Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<SwitchDemo>(), config);
}
