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

class YogaSettingsApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "YogaSettingsApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class YogaSettingsState : public State {
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
        // Settings Content Container
        auto content = std::make_shared<FlexBox>();
        content->widthPercent(100)
               .backgroundColor(0xFF121212)
               .flexDirection(YGFlexDirectionColumn)
               .alignItems(YGAlignCenter);

        // 1. App Bar
        auto app_bar = std::make_shared<FlexBox>();
        app_bar->widthPercent(100)
               .height(40)
               .backgroundColor(0xFF1E1E1E)
               .flexDirection(YGFlexDirectionRow)
               .alignItems(YGAlignCenter)
               .justifyContent(YGJustifyCenter);

        app_bar->child(text("Settings", {
            .font_size = 20,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        }));

        auto app_bar_divider = std::make_shared<FlexBox>();
        app_bar_divider->widthPercent(100).height(1).backgroundColor(0xFF333333);

        content->child(app_bar).child(app_bar_divider);

        auto center_container = std::make_shared<FlexBox>();
        center_container->widthPercent(90)
                        .padding(YGEdgeVertical, 30)
                        .flexDirection(YGFlexDirectionColumn);

        // Helper to create a setting group
        auto create_group = [](const std::string& title, const std::vector<WidgetPtr>& items) {
            auto group = std::make_shared<FlexBox>();
            group->flexDirection(YGFlexDirectionColumn)
                 .margin(YGEdgeBottom, 30);

            // Group Title
            group->child(container({
                .padding = {16, 8, 16, 8},
                .child = text(title, {.font_size = 14, .color = 0xFFAAAAAA, .weight = TextStyle::Bold})
            }));

            // Group Card
            auto card = std::make_shared<FlexBox>();
            card->flexDirection(YGFlexDirectionColumn)
                .backgroundColor(0xFF1E1E1E)
                .borderRadius(12);

            for (size_t i = 0; i < items.size(); ++i) {
                card->child(items[i]);
                if (i < items.size() - 1) {
                    // Divider
                    auto divider = std::make_shared<FlexBox>();
                    divider->widthPercent(100).height(0).backgroundColor(0xFF333333);
                    card->child(divider);
                }
            }

            group->child(card);
            return group;
        };

        // Helper to create a single setting tile
        auto create_setting = [this](IconData icon_data, uint32_t icon_bg, const std::string& title, bool current_val, std::function<void(bool)> on_toggle) {
            auto leading_icon = container({
                .color = icon_bg,
                .width = 32.0f,
                .height = 32.0f,
                .border_radius = {8.0f, 8.0f, 8.0f, 8.0f},
                .child = icon(icon_data, {.size = 18, .color = 0xFFFFFFFF})
            });

            auto trailing_switch = toggle_switch({
                .value = current_val,
                .on_changed = [this, on_toggle, title](bool val) {
                    std::cout << title << " switch clicked, changed to " << val << "\n";
                    setState([=]() { on_toggle(val); });
                },
                .active_color = 0xFF34C759 // iOS Green
            });

            return list_tile(ListTileConfig{
                .leading = leading_icon,
                .title = text(title, {.font_size = 16, .color = 0xFFFFFFFF}),
                .subtitle = nullptr,
                .trailing = trailing_switch,
                .on_tap = nullptr, // Disable row tap to let the switch receive the events properly!
                .content_padding = {16, 12, 16, 12},
                .hover_color = 0xFF2A2A2A,
                .pressed_color = 0xFF333333
            });
        };

        // Group 1: Connectivity
        std::vector<WidgetPtr> group1 = {
            create_setting(Icons::Wifi, 0xFF007AFF, "Wi-Fi", wifi_enabled, [this](bool v) { wifi_enabled = v; }),
            create_setting(Icons::Bluetooth, 0xFF007AFF, "Bluetooth", bluetooth_enabled, [this](bool v) { bluetooth_enabled = v; }),
            create_setting(Icons::AntennaRadiowavesLeftRight, 0xFF34C759, "Cellular", cellular_enabled, [this](bool v) { cellular_enabled = v; }),
            create_setting(Icons::Personalhotspot, 0xFF34C759, "Personal Hotspot", hotspot_enabled, [this](bool v) { hotspot_enabled = v; })
        };
        center_container->child(create_group("CONNECTIVITY", group1));

        // Group 2: Notifications & Sounds
        std::vector<WidgetPtr> group2 = {
            create_setting(Icons::Bell, 0xFFFF3B30, "Notifications", notifications_enabled, [this](bool v) { notifications_enabled = v; }),
            create_setting(Icons::Speaker2, 0xFFFF2D55, "Sounds & Haptics", sounds_enabled, [this](bool v) { sounds_enabled = v; }),
            create_setting(Icons::Moon, 0xFF5856D6, "Focus", focus_enabled, [this](bool v) { focus_enabled = v; })
        };
        center_container->child(create_group("NOTIFICATIONS", group2));

        // Group 3: General
        std::vector<WidgetPtr> group3 = {
            create_setting(Icons::Settings, 0xFF8E8E93, "General", general_enabled, [this](bool v) { general_enabled = v; }),
            create_setting(Icons::DeviceDesktop, 0xFF007AFF, "Display & Brightness", display_enabled, [this](bool v) { display_enabled = v; }),
            create_setting(Icons::Battery100, 0xFF34C759, "Battery", battery_enabled, [this](bool v) { battery_enabled = v; }),
            create_setting(Icons::Lock, 0xFF8E8E93, "Privacy & Security", lock_enabled, [this](bool v) { lock_enabled = v; })
        };
        center_container->child(create_group("GENERAL", group3));

        content->child(center_container);
        
        // Use scroll_view as the ROOT widget! This allows it to automatically take up the full window size.
        return scroll_view({
            .child = content,
            .scroll_speed = 40.0f
        });
    }
};

std::unique_ptr<State> YogaSettingsApp::createState() {
    return std::make_unique<YogaSettingsState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Settings App",
        .width = 450,
        .height = 800,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<YogaSettingsApp>(), config);
}
