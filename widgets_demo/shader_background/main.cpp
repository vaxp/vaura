// widgets_demo/shader_background/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/shader_background.hpp"
#include "vaura/widgets/stack.hpp"

using namespace vaura;

class ShaderBackgroundDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ShaderBackgroundDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Shader Background Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        ShaderBackgroundConfig shader_cfg;
        shader_cfg.sksl_code = R"(
            uniform float u_time;
            uniform vec2  u_resolution;
            
            vec4 main(vec2 fragCoord) {
                vec2 uv = fragCoord.xy / u_resolution.xy;
                vec3 col = 0.5 + 0.5*cos(u_time + uv.xyx + vec3(0,2,4));
                return vec4(col, 1.0);
            }
        )";

        auto bg = std::make_shared<ShaderBackgroundWidget>(shader_cfg);
        auto bg_container = std::make_shared<FlexBox>();
        bg_container->widthPercent(100).heightPercent(100).child(bg);

        auto content = std::make_shared<FlexBox>();
        content->justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter)
               .widthPercent(100).heightPercent(100);
        content->child(Text("Hello GLSL!", {.font_size = 48.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        StackConfig stack_cfg;
        stack_cfg.alignment = Alignment::Center;
        stack_cfg.children = {bg_container, content};

        auto stack_widget = Stack(stack_cfg);
        
        auto stack_container = std::make_shared<FlexBox>();
        stack_container->widthPercent(100).heightPercent(100).child(stack_widget);

        auto wrapper = std::make_shared<FlexBox>();
        wrapper->width(600).height(400)
               .borderRadius(16)
               .overflow(YGOverflowHidden)
               .child(stack_container);

        root->child(wrapper);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Shader Background Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ShaderBackgroundDemo>(), config);
}
