#include "vaura/app/app.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/shader_progress.hpp"
#include <iostream>

using namespace vaura;

class ShadersDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ShadersDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF0F172A)
            .alignItems(YGAlignCenter)
            .justifyContent(YGJustifyCenter);

        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionRow)
               .alignItems(YGAlignCenter)
               .justifyContent(YGJustifyCenter);

        // --- Galaxy Widget ---
        auto galaxy_container = std::make_shared<FlexBox>();
        galaxy_container->alignItems(YGAlignCenter)
                        .margin(YGEdgeRight, 50);
                        
        auto galaxy_title = Text("Galaxy Spinner", {
            .font_size = 18,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto galaxy_text_container = std::make_shared<FlexBox>();
        galaxy_text_container->margin(YGEdgeBottom, 20).child(galaxy_title);
        
        galaxy_container->child(galaxy_text_container);
        galaxy_container->child(GalaxyProgress(150.0f));

        // --- Liquid Wave Widget ---
        auto liquid_container = std::make_shared<FlexBox>();
        liquid_container->alignItems(YGAlignCenter)
                        .margin(YGEdgeLeft, 50);
                        
        auto liquid_title = Text("Liquid Wave Spinner", {
            .font_size = 18,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto liquid_text_container = std::make_shared<FlexBox>();
        liquid_text_container->margin(YGEdgeBottom, 20).child(liquid_title);
        
        liquid_container->child(liquid_text_container);
        liquid_container->child(LiquidProgress(150.0f));

        content->child(galaxy_container);
        content->child(liquid_container);

        root->child(content);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Boss Level Shaders",
        .width = 900,
        .height = 600,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<ShadersDemoApp>(), config);
}
