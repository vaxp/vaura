#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/drawer.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class DrawerDemoState : public State {
    bool is_open = false;
public:
    WidgetPtr build(BuildContext& ctx) override {
        // Navigation Links (Simulated)
        auto make_nav_link = [](const std::string& label, Color txt_color) {
            return Column({
                .margin = std::pair{Edge::Top, 12.0f},
                .children = {
                    Button({
                        .child = Text(label, {.font_size = 16.0f, .color = txt_color}),
                        .on_pressed = [](){},
                        .color = 0x00000000,
                        .width_percent = 100.0f
                    })
                }
            });
        };

        // --- Drawer Content ---
        auto drawer_content = Column({
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .padding = std::pair{Edge::All, 24.0f},
            .children = {
                // Profile Section
                Row({
                    .align = Align::Center,
                    .margin = std::pair{Edge::Bottom, 24.0f},
                    .children = {
                        Avatar({
                            .size = 64.0f,
                            .background_color = 0xFF6366F1,
                            .initials = "JD"
                        }),
                        Column({
                            .justify = Justify::Center,
                            .margin = std::pair{Edge::Left, 16.0f},
                            .children = {
                                Text("John Doe", {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                                Text("john@vaura.dev", {.font_size = 14.0f, .color = 0xFF94A3B8})
                            }
                        })
                    }
                }),
                Divider({.color = 0xFF334155}),
                make_nav_link("Dashboard", 0xFFF8FAFC),
                make_nav_link("Projects", 0xFF94A3B8),
                make_nav_link("Team", 0xFF94A3B8),
                make_nav_link("Settings", 0xFF94A3B8)
            }
        });

        // --- Main Body ---
        auto body = Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Main Screen", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Text("Click the button below to open the drawer.", {.font_size = 16.0f, .color = 0xFF94A3B8}),
                Column({
                    .margin = std::pair{Edge::Top, 32.0f},
                    .children = {
                        Button({
                            .child = Text("Open Drawer", {.font_size = 16.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                            .on_pressed = [this]() {
                                setState([this]() { is_open = true; });
                            },
                            .color = 0xFF3B82F6
                        })
                    }
                })
            }
        });

        // --- Drawer Wrapper ---
        return Drawer({
            .body = body,
            .child = drawer_content,
            .is_open = is_open,
            .on_close = [this]() {
                setState([this]() { is_open = false; });
            },
            .side = DrawerConfig::Side::Left
        });
    }
};

class DrawerDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "DrawerDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<DrawerDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Drawer Demo";
    config.width = 800;
    config.height = 600;

    return runApp(std::make_shared<DrawerDemo>(), config);
}
