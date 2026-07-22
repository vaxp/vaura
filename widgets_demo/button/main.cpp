#include "vaura/app/app.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class ButtonDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ButtonDemo"; }

    WidgetPtr build(BuildContext& context) override {
        auto col = std::make_shared<FlexBox>();
        col->flexDirection(YGFlexDirectionColumn)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 24.0f)
           .widthPercent(100.0f)
           .heightPercent(100.0f)
           .backgroundColor(0xFF0F172A);

        // Title
        TextConfig title_cfg;
        title_cfg.font_size = 32.0f;
        title_cfg.color = 0xFFF8FAFC; 
        col->child(std::make_shared<TextWidget>("Button Showcase", title_cfg));

        // 1. Primary Button
        ButtonConfig primary_cfg;
        primary_cfg.color = 0xFF3B82F6; 
        primary_cfg.hover_color = 0xFF2563EB; 
        primary_cfg.pressed_color = 0xFF1D4ED8; 
        primary_cfg.border_radius = 12.0f;
        primary_cfg.padding = EdgeInsets(16, 32, 16, 32);
        primary_cfg.elevation = 8.0f;
        
        TextConfig primary_txt;
        primary_txt.font_size = 18.0f;
        primary_txt.color = 0xFFFFFFFF;
        primary_cfg.child = std::make_shared<TextWidget>("Primary Button", primary_txt);
        col->child(std::make_shared<ButtonWidget>(primary_cfg));

        // 2. Success Button
        ButtonConfig success_cfg;
        success_cfg.color = 0xFF10B981; 
        success_cfg.hover_color = 0xFF059669; 
        success_cfg.pressed_color = 0xFF047857; 
        success_cfg.border_radius = 24.0f; 
        success_cfg.padding = EdgeInsets(16, 40, 16, 40);
        success_cfg.elevation = 4.0f;
        
        TextConfig success_txt;
        success_txt.font_size = 18.0f;
        success_txt.color = 0xFFFFFFFF;
        success_cfg.child = std::make_shared<TextWidget>("Success Pill", success_txt);
        col->child(std::make_shared<ButtonWidget>(success_cfg));

        // 3. Outlined Button 
        ButtonConfig outlined_cfg;
        outlined_cfg.color = 0x00000000;
        outlined_cfg.hover_color = 0x20F43F5E; 
        outlined_cfg.border = Border{0xFFF43F5E, 2.0f}; 
        outlined_cfg.border_radius = 12.0f;
        outlined_cfg.padding = EdgeInsets(16, 32, 16, 32);
        outlined_cfg.elevation = 0.0f;
        
        TextConfig outlined_txt;
        outlined_txt.font_size = 18.0f;
        outlined_txt.color = 0xFFF43F5E;
        outlined_cfg.child = std::make_shared<TextWidget>("Outlined Button", outlined_txt);
        col->child(std::make_shared<ButtonWidget>(outlined_cfg));

        // 4. Disabled Button
        ButtonConfig disabled_cfg;
        disabled_cfg.color = 0xFF334155; 
        disabled_cfg.border_radius = 12.0f;
        disabled_cfg.padding = EdgeInsets(16, 32, 16, 32);
        disabled_cfg.enabled = false;
        disabled_cfg.elevation = 0.0f;
        
        TextConfig disabled_txt;
        disabled_txt.font_size = 18.0f;
        disabled_txt.color = 0xFF94A3B8; 
        disabled_cfg.child = std::make_shared<TextWidget>("Disabled State", disabled_txt);
        col->child(std::make_shared<ButtonWidget>(disabled_cfg));

        return col;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Button Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<ButtonDemo>(), config);
}
