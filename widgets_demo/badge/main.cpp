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
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B,
            .children = {
                Card({
                    .color = 0xFF0F172A,
                    .border_radius = 20.0f,
                    .elevation = 15.0f,
                    .padding = {40, 40, 40, 40}
                }, Row({
                    .justifyContent = YGJustifySpaceEvenly,
                    .alignItems = YGAlignCenter,
                    .width = 400.0f,
                    .children = {
                        // Icon 1: Notifications
                        Badge({
                            .text = "3",
                            .background_color = 0xFFEF4444, // Red
                            .child = Icon(0xf448, {
                                .size = 36.0f,
                                .color = 0xFFCBD5E1
                            })
                        }),
                        
                        // Icon 2: Messages
                        Badge({
                            .text = "99+",
                            .background_color = 0xFF3B82F6, // Blue
                            .child = Icon(0xf443, { // heart_solid
                                .size = 36.0f,
                                .color = 0xFFCBD5E1
                            })
                        }),
                        
                        // Icon 3: Shopping Cart
                        Badge({
                            .text = "12",
                            .background_color = 0xFF10B981, // Emerald
                            .offset = {-2.0f, 2.0f}, // slight adjustment
                            .child = Icon(0xf47d, { // person
                                .size = 36.0f,
                                .color = 0xFFCBD5E1
                            })
                        })
                    }
                }))
            }
        });
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
