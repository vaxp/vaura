#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/switch.hpp"
#include "vaura/widgets/list_tile.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class AnuSettingsApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AnuSettingsApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class AnuSettingsState : public State {
public:
    // Switch states
    bool wifi_enabled = true;
    bool bluetooth_enabled = true;
    bool cellular_enabled = false;
    bool hotspot_enabled = false;

    bool notifications_enabled = true;
    bool sounds_enabled = true;
    bool focus_enabled = false;

    bool general_enabled = false;
    bool display_enabled = true;
    bool battery_enabled = true;
    bool lock_enabled = false;

    WidgetPtr build(BuildContext& ctx) override {
        // Helper to create a setting group
        auto create_group = [](const std::string& title, const std::vector<WidgetPtr>& items) {
            std::vector<WidgetPtr> card_children;
            for (size_t i = 0; i < items.size(); ++i) {
                card_children.push_back(items[i]);
                if (i < items.size() - 1) {
                    card_children.push_back(Container({
                        .color = 0xFF333333,
                        .height = 0.0f,
                        .width_percent = 100.0f
                    }));
                }
            }

            return Column({
                .margin = std::pair{Edge::Bottom, 30.0f},
                .children = {
                    // Group Title
                    Container({

                        .padding = {16, 8, 16, 8},
                        .child = Text(title, {.font_size = 14, .color = 0xFFAAAAAA, .weight = TextStyle::Bold})
                    }),
                    // Group Card
                    Column({
                        .padding = std::pair{Edge::All, 10.0f},
                        .backgroundColor = 0xFF1E1E1E,
                        .borderRadius = 12.0f,
                        .children = card_children
                        
                    })
                }
            });
        };

        // Helper to create a single setting tile
        auto create_setting = [this](IconData icon_data, uint32_t icon_bg, const std::string& title, bool current_val, std::function<void(bool)> on_toggle) {
            return ListTile(ListTileConfig{
                .leading = Container({
                    .color = icon_bg,
                    .width = 32.0f,
                    .height = 32.0f,
                    .border_radius = {8.0f, 8.0f, 8.0f, 8.0f},
                    .child = Icon(icon_data, {.size = 18, .color = 0xFFFFFFFF}),
                }),
                .title = Text(title, {.font_size = 16, .color = 0xFFFFFFFF}),
                .subtitle = nullptr,
                .trailing = Switch({
                    .value = current_val,
                    .on_changed = [this, on_toggle, title](bool val) {
                        std::cout << title << " switch clicked, changed to " << val << "\n";
                        setState([=]() { on_toggle(val); });
                    },
                    .active_color = 0xFF34C759 // iOS Green
                }),
                .on_tap = nullptr, // Disable row tap to let the switch receive the events properly!
                .hover_color = 0xFF2A2A2A,
                .pressed_color = 0xFF333333
            });
        };

        auto content = Column({
            .align = Align::Center,
            .widthPercent = 100.0f,
            .backgroundColor = 0xFF121212,
            .children = {
                // 1. App Bar
                Row({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .widthPercent = 100.0f,
                    .height = 40.0f,
                    .backgroundColor = 0xFF1E1E1E,
                    .children = {
                        Text("Settings", {
                            .font_size = 20,
                            .color = 0xFFFFFFFF,
                            .weight = TextStyle::Bold
                        })
                    }
                }),
                // App Bar Divider
                Container({
                    .color = 0xFF333333,
                    .height = 1.0f,
                    .width_percent = 100.0f
                }),
                // Center Container
                Column({
                    .widthPercent = 90.0f,
                    .padding = std::pair{Edge::Vertical, 30.0f},
                    .children = {

                        create_group("CONNECTIVITY", {
                            create_setting(Icons::Wifi, 0xFF007AFF, "Wi-Fi", wifi_enabled, [this](bool v) { wifi_enabled = v; }),
                            create_setting(Icons::Bluetooth, 0xFF007AFF, "Bluetooth", bluetooth_enabled, [this](bool v) { bluetooth_enabled = v; }),
                            create_setting(Icons::AntennaRadiowavesLeftRight, 0xFF34C759, "Cellular", cellular_enabled, [this](bool v) { cellular_enabled = v; }),
                            create_setting(Icons::Personalhotspot, 0xFF34C759, "Personal Hotspot", hotspot_enabled, [this](bool v) { hotspot_enabled = v; })
                        }),
                        create_group("NOTIFICATIONS", {
                            create_setting(Icons::Bell, 0xFFFF3B30, "Notifications", notifications_enabled, [this](bool v) { notifications_enabled = v; }),
                            create_setting(Icons::Speaker2, 0xFFFF2D55, "Sounds & Haptics", sounds_enabled, [this](bool v) { sounds_enabled = v; }),
                            create_setting(Icons::Moon, 0xFF5856D6, "Focus", focus_enabled, [this](bool v) { focus_enabled = v; })
                        }),
                        create_group("GENERAL", {
                            create_setting(Icons::Settings, 0xFF8E8E93, "General", general_enabled, [this](bool v) { general_enabled = v; }),
                            create_setting(Icons::DeviceDesktop, 0xFF007AFF, "Display & Brightness", display_enabled, [this](bool v) { display_enabled = v; }),
                            create_setting(Icons::Battery100, 0xFF34C759, "Battery", battery_enabled, [this](bool v) { battery_enabled = v; }),
                            create_setting(Icons::Lock, 0xFF8E8E93, "Privacy & Security", lock_enabled, [this](bool v) { lock_enabled = v; })
                        })
                    }
                })
            }
        });

        // Use scroll_view as the ROOT widget! This allows it to automatically take up the full window size.
        return ScrollView({
            .child = content,
            .show_scrollbar = true,
            .scroll_speed = 40.0f,
        });
    }
};

std::unique_ptr<State> AnuSettingsApp::createState() {
    return std::make_unique<AnuSettingsState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Settings App",
        .width = 450,
        .height = 800,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<AnuSettingsApp>(), config);
}
