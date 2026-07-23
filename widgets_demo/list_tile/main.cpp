#include "vaura/app/app.hpp"
#include "vaura/widgets/list_tile.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/rendering/font_manager.hpp"
#include <iostream>

using namespace vaura;

class ListTileDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ListTileDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B,
            .children = {
                Card({
                    .color = 0xFF0F172A,
                    .border_radius = 20.0f,
                    .elevation = 15.0f,
                    .padding = {0, 10, 0, 10} // Vertial padding, 0 horizontal to allow tiles to stretch
                }, Column({
                    .align = Align::Stretch,
                    .width = 450.0f,
                    .children = {
                        Column({
                            .margin = std::pair{Edge::Bottom, 20.0f},
                            .children = {
                                Text("Settings", {
                                    .font_size = 28,
                                    .color = 0xFFFFFFFF,
                                    .weight = TextStyle::Bold
                                })
                            }
                        }),
                        ListTile({
                            .leading = Icon(0xf47d, {.size = 28, .color = 0xFF38BDF8}), // Person
                            .title = Text("Account", {.font_size = 18, .color = 0xFFF8FAFC}),
                            .subtitle = Text("Manage your profile", {.font_size = 14, .color = 0xFF94A3B8}),
                            .trailing = Icon(0xf448, {.size = 24, .color = 0xFF64748B}), // Chevron
                            .on_tap = []() { std::cout << "Account tapped!" << std::endl; }
                        }),
                        Divider({.thickness = 1, .color = 0xFF334155, .indent = 60, .end_indent = 16}),
                        ListTile({
                            .leading = Avatar({.size = 40.0f, .image_path = "", .background_color = 0xFF10B981, .initials = "JD"}),
                            .title = Text("VAURA Cloud", {.font_size = 18, .color = 0xFFF8FAFC}),
                            .subtitle = Text("Sync & Backup", {.font_size = 14, .color = 0xFF94A3B8}),
                            .on_tap = []() { std::cout << "Cloud tapped!" << std::endl; }
                        }),
                        Divider({.thickness = 1, .color = 0xFF334155, .indent = 60, .end_indent = 16}),
                        ListTile({
                            .leading = Icon(0xf443, {.size = 28, .color = 0xFFEF4444}), // Heart
                            .title = Text("Favorites", {.font_size = 18, .color = 0xFFF8FAFC}),
                            .trailing = Text("12", {.font_size = 16, .color = 0xFF94A3B8}),
                            .on_tap = []() { std::cout << "Favorites tapped!" << std::endl; }
                        })
                    }
                }))
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA ListTile Demo",
        .width = 600,
        .height = 500,
        .vsync = true
    };
    
    bool font_loaded = FontManager::loadFont("examples/icon_demo/CupertinoIcons.ttf", "CupertinoIcons");
    if (!font_loaded) {
        std::cerr << "Failed to load CupertinoIcons.ttf" << std::endl;
    }
    
    return vaura::runApp(std::make_shared<ListTileDemoApp>(), config);
}
