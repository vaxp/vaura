#include "vaura/app/app.hpp"
#include "vaura/widgets/badge.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/rendering/font_manager.hpp"
#include <iostream>

using namespace vaura;

class BadgeDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "BadgeDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .alignItems(YGAlignCenter)
           .justifyContent(YGJustifySpaceEvenly)
           .width(400);

        // Icon 1: Notifications
        auto icon1 = icon(0xf448, {
            .size = 36.0f,
            .color = 0xFFCBD5E1
        });
        auto b1 = badge({
            .text = "3",
            .background_color = 0xFFEF4444, // Red
            .child = icon1
        });

        // Icon 2: Messages
        auto icon2 = icon(0xf443, { // heart_solid
            .size = 36.0f,
            .color = 0xFFCBD5E1
        });
        auto b2 = badge({
            .text = "99+",
            .background_color = 0xFF3B82F6, // Blue
            .child = icon2
        });

        // Icon 3: Shopping Cart
        auto icon3 = icon(0xf47d, { // person
            .size = 36.0f,
            .color = 0xFFCBD5E1
        });
        auto b3 = badge({
            .text = "12",
            .background_color = 0xFF10B981, // Emerald
            .offset = {-2.0f, 2.0f}, // slight adjustment
            .child = icon3
        });

        row->child(b1).child(b2).child(b3);

        auto c = card({
            .color = 0xFF0F172A,
            .border_radius = 20.0f,
            .elevation = 15.0f,
            .padding = {40, 40, 40, 40}
        }, row);

        root->child(c);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Badge Demo",
        .width = 600,
        .height = 400
    };
    
    bool font_loaded = FontManager::loadFont("examples/icon_demo/CupertinoIcons.ttf", "CupertinoIcons");
    if (!font_loaded) {
        std::cerr << "Failed to load CupertinoIcons.ttf" << std::endl;
    }
    
    return vaura::runApp(std::make_shared<BadgeDemoApp>(), config);
}
