#include "vaura/app/app.hpp"
#include "vaura/widgets/tooltip.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/rendering/font_manager.hpp"

using namespace vaura;

class TooltipDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "TooltipDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF0F172A)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .alignItems(YGAlignCenter)
           .justifyContent(YGJustifySpaceAround)
           .width(600);

        // Tooltip 1: Button
        auto btn = Button({
            .child = Text("Hover Me!", {.font_size = 18, .color = 0xFFFFFFFF}),
            .on_pressed = [](){}
        });
        
        auto tt1 = Tooltip({
            .message = "This is a button tooltip!",
            .vertical_offset = 50.0f, // Button is about 40px high
            .child = btn
        });

        // Tooltip 2: Icon
        auto icn = Icon(0xf47d, { // Person
            .size = 48.0f,
            .color = 0xFF94A3B8
        });
        
        auto tt2 = Tooltip({
            .message = "Profile Settings",
            .vertical_offset = 60.0f,
            .background_color = 0xFF3B82F6, // Blue bg
            .child = icn
        });

        // Tooltip 3: Avatar
        auto av = Avatar({
            .size = 64.0f,
            .background_color = 0xFF10B981,
            .initials = "JD",
            .font_size = 24.0f
        });
        
        auto tt3 = Tooltip({
            .message = "Jane Doe (Online)",
            .vertical_offset = 75.0f,
            .child = av
        });

        row->child(tt1)
           .child(tt2)
           .child(tt3);

        auto card_container = Container({
            .color = 0xFF1E293B,
            .padding = {60, 40, 60, 40},
            .border_radius = BorderRadius::circular(24.0f),
            .child = row
        });

        root->child(card_container);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tooltip Demo",
        .width = 800,
        .height = 400
    };
    
    // Load icons font
    vaura::FontManager::loadFont("examples/icon_demo/CupertinoIcons.ttf", "CupertinoIcons");
    
    return vaura::runApp(std::make_shared<TooltipDemoApp>(), config);
}
