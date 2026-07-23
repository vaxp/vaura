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
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .gap = 40.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                // Title
                Text("Settings", {.font_size = 32.0f, .color = 0xFFFFFFFF}),
                
                // Settings Card Layout
                Column({
                    .gap = 24.0f,
                    .padding = std::pair{Edge::All, 32.0f},
                    .backgroundColor = 0xFF1E293B, // Slate 800
                    .borderRadius = 16.0f,
                    .children = {
                        // 1. Wi-Fi
                        buildSettingRow("Wi-Fi", wifi_enabled, [this](bool val) {
                            setState([this, val]() { wifi_enabled = val; });
                        }, 0xFF3B82F6),
                        
                        // 2. Bluetooth
                        buildSettingRow("Bluetooth", bluetooth_enabled, [this](bool val) {
                            setState([this, val]() { bluetooth_enabled = val; });
                        }, 0xFF8B5CF6),
                        
                        // 3. Airplane Mode
                        buildSettingRow("Airplane Mode", airplane_mode, [this](bool val) {
                            setState([this, val]() { 
                                airplane_mode = val; 
                                if (val) {
                                    wifi_enabled = false;
                                    bluetooth_enabled = false;
                                }
                            });
                        }, 0xFFF59E0B)
                    }
                })
            }
        });
    }

private:
    WidgetPtr buildSettingRow(const std::string& label, bool value, std::function<void(bool)> on_change, Color active_color) {
        return Row({
            .justify = Justify::SpaceBetween,
            .align = Align::Center,
            .width = 300.0f,
            .children = {
                Text(label, {.font_size = 20.0f, .color = 0xFFF8FAFC}),
                Switch({
                    .value = value,
                    .on_changed = on_change,
                    .active_color = active_color,
                    .inactive_color = 0xFF475569 // Slate 600
                })
            }
        });
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
