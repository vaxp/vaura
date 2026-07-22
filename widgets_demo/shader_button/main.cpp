// widgets_demo/shader_button/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/shader_button.hpp"
#include <iostream>

using namespace vaura;

class ShaderButtonDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ShaderButtonDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(text("Shader Button Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        // A basic SKSL shader for the button background
        std::string btn_shader = R"(
            uniform float u_time;
            uniform vec2  u_resolution;
            uniform float u_hover;
            uniform float u_pressed;
            uniform vec4  u_color1;
            uniform vec4  u_color2;
            
            vec4 main(vec2 fragCoord) {
                vec2 uv = fragCoord.xy / u_resolution.xy;
                float t = u_time * 2.0;
                float blend = (sin(uv.x * 10.0 + t) + cos(uv.y * 10.0 + t)) * 0.25 + 0.5;
                vec4 baseCol = mix(u_color1, u_color2, blend);
                vec4 highlight = vec4(u_hover * 0.2 + u_pressed * 0.3);
                return baseCol + highlight;
            }
        )";

        ShaderButtonConfig btn_cfg;
        btn_cfg.sksl_code = btn_shader;
        btn_cfg.color1 = 0xFF3B82F6; // Blue
        btn_cfg.color2 = 0xFF8B5CF6; // Purple
        btn_cfg.width = 200.0f;
        btn_cfg.height = 60.0f;
        btn_cfg.border_radius = 30.0f;
        btn_cfg.on_tap = []() {
            std::cout << "Shader button tapped!" << std::endl;
        };
        
        btn_cfg.child = text("Animated Button", {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold});

        root->child(shader_button(btn_cfg));

        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Shader Button Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ShaderButtonDemo>(), config);
}
