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
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionColumn)
               .alignItems(YGAlignStretch)
               .width(450);

        auto title = text("Settings", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(YGEdgeBottom, 20).child(title);
        content->child(title_container);

        // Tile 1
        auto tile1 = list_tile({
            .leading = icon(0xf47d, {.size = 28, .color = 0xFF38BDF8}), // Person
            .title = text("Account", {.font_size = 18, .color = 0xFFF8FAFC}),
            .subtitle = text("Manage your profile", {.font_size = 14, .color = 0xFF94A3B8}),
            .trailing = icon(0xf448, {.size = 24, .color = 0xFF64748B}), // Chevron
            .on_tap = []() { std::cout << "Account tapped!" << std::endl; }
        });

        // Tile 2
        auto tile2 = list_tile({
            .leading = avatar({.size = 40.0f, .background_color = 0xFF8B5CF6, .image_path = "", .initials = "VA"}),
            .title = text("VAURA Cloud", {.font_size = 18, .color = 0xFFF8FAFC}),
            .subtitle = text("Sync & Backup", {.font_size = 14, .color = 0xFF94A3B8}),
            .on_tap = []() { std::cout << "Cloud tapped!" << std::endl; }
        });

        // Tile 3
        auto tile3 = list_tile({
            .leading = icon(0xf443, {.size = 28, .color = 0xFFEF4444}), // Heart
            .title = text("Favorites", {.font_size = 18, .color = 0xFFF8FAFC}),
            .trailing = text("12", {.font_size = 16, .color = 0xFF94A3B8}),
            .on_tap = []() { std::cout << "Favorites tapped!" << std::endl; }
        });

        auto div1 = divider({.thickness = 1, .color = 0xFF334155, .indent = 60, .end_indent = 16});
        auto div2 = divider({.thickness = 1, .color = 0xFF334155, .indent = 60, .end_indent = 16});

        content->child(tile1).child(div1)
               .child(tile2).child(div2)
               .child(tile3);

        auto c = card({
            .color = 0xFF0F172A,
            .border_radius = 20.0f,
            .elevation = 15.0f,
            .padding = {0, 10, 0, 10} // Vertial padding, 0 horizontal to allow tiles to stretch
        }, content);

        root->child(c);
        return root;
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
