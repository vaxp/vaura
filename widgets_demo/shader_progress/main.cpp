// widgets_demo/shader_progress/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/shader_progress.hpp"

using namespace vaura;

class ShaderProgressDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ShaderProgressDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Shader Progress Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(60);
        root->child(spacer);

        std::string prog_shader = R"(
            uniform float u_time;
            uniform vec2  u_resolution;
            uniform vec4  u_color1;
            uniform vec4  u_color2;
            
            vec4 main(vec2 fragCoord) {
                vec2 uv = fragCoord.xy / u_resolution.xy;
                // Simple rotating gradient
                vec2 centered = uv - 0.5;
                float angle = atan(centered.y, centered.x);
                float radius = length(centered);
                
                float t = u_time * 3.0;
                float w = sin(angle * 4.0 - t) * 0.5 + 0.5;
                
                vec4 col = mix(u_color1, u_color2, w);
                
                // create a ring
                float ring = smoothstep(0.4, 0.45, radius) - smoothstep(0.45, 0.5, radius);
                
                return col * ring * 2.0;
            }
        )";

        ShaderProgressConfig prog_cfg;
        prog_cfg.sksl_code = prog_shader;
        prog_cfg.size = 150.0f;
        prog_cfg.color1 = 0xFF10B981; // Emerald
        prog_cfg.color2 = 0xFF3B82F6; // Blue
        
        root->child(std::make_shared<ShaderProgressWidget>(prog_cfg));

        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Shader Progress Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ShaderProgressDemo>(), config);
}
