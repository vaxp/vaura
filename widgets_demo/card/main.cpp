#include "vaura/app/app.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class CardDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CardDemo"; }

    WidgetPtr build(BuildContext& context) override {
        // Main layout (Centered)
        auto center = std::make_shared<FlexBox>();
        center->flexDirection(YGFlexDirectionColumn)
              .justifyContent(YGJustifyCenter)
              .alignItems(YGAlignCenter)
              .widthPercent(100.0f)
              .heightPercent(100.0f)
              .backgroundColor(0xFF0F172A);

        // Profile Card Layout
        auto card_layout = std::make_shared<FlexBox>();
        card_layout->flexDirection(YGFlexDirectionColumn)
                   .justifyContent(YGJustifyFlexStart)
                   .alignItems(YGAlignCenter)
                   .gap(YGGutterAll, 16.0f);

        // Header Text
        TextConfig header_cfg;
        header_cfg.font_size = 24.0f;
        header_cfg.color = 0xFFFFFFFF;
        card_layout->child(std::make_shared<TextWidget>("VAURA Developer", header_cfg));

        // Subtitle Text
        TextConfig sub_cfg;
        sub_cfg.font_size = 16.0f;
        sub_cfg.color = 0xFF94A3B8; // Slate 400
        card_layout->child(std::make_shared<TextWidget>("C++ UI Framework", sub_cfg));

        // Action Button
        ButtonConfig action_btn;
        action_btn.color = 0xFF8B5CF6; // Violet 500
        action_btn.hover_color = 0xFF7C3AED; // Violet 600
        action_btn.border_radius = 20.0f;
        action_btn.padding = EdgeInsets(12, 32, 12, 32);
        
        TextConfig action_txt;
        action_txt.font_size = 16.0f;
        action_txt.color = 0xFFFFFFFF;
        action_btn.child = std::make_shared<TextWidget>("Follow", action_txt);
        
        card_layout->child(std::make_shared<ButtonWidget>(action_btn));

        // Create the Luxurious Card
        CardConfig card_cfg;
        card_cfg.color = 0xFF1E293B; // Slate 800
        card_cfg.border_radius = 24.0f;
        card_cfg.elevation = 20.0f; // Deep shadow
        card_cfg.shadow_color = 0x80000000;
        card_cfg.shadow_dy = 10.0f;
        card_cfg.padding = EdgeInsets(40, 60, 40, 60);

        center->child(card(card_cfg, card_layout));
        return center;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Card Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<CardDemo>(), config);
}
