#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/shader_button.hpp"

using namespace vaura;

const char* GLOW_BUTTON_SKSL = R"(
    uniform float u_time;
    uniform float2 u_resolution;
    uniform float u_hover;
    uniform float u_pressed;
    uniform half4 u_color1;
    uniform half4 u_color2;

    half4 main(float2 fragCoord) {
        float2 uv = fragCoord / u_resolution;
        float dist = length(uv - 0.5);
        
        // Dynamic plasma/liquid background
        float t = u_time * 1.5;
        float2 p = uv * 3.0;
        
        float wave = sin(p.x + t) + cos(p.y + t) + 
                     sin(p.x * 2.0 - t * 0.5) + cos(p.y * 1.5 + t * 0.8);
                     
        float mix_val = wave * 0.25 + 0.5;
        half4 color = mix(u_color1, u_color2, mix_val);
        
        // Add a glow when hovered
        if (u_hover > 0.0) {
            float glow = (0.5 - dist) * 2.0 * u_hover;
            color = color + half4(glow, glow, glow, 0.0);
        }
        
        // Darken when pressed
        if (u_pressed > 0.0) {
            color = color * (1.0 - u_pressed * 0.3);
        }
        
        return color;
    }
)";

int main(int argc, char** argv) {
    auto root = std::make_shared<FlexBox>();
    root->widthPercent(100).heightPercent(100)
        .justifyContent(YGJustifyCenter)
        .alignItems(YGAlignCenter)
        .backgroundColor(0xFF1E293B);

    auto center_wrap = std::make_shared<FlexBox>();
    center_wrap->widthPercent(100).heightPercent(100)
               .justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter)
               .child(Text("GLOW BUTTON", {
                   .font_size = 20, 
                   .color = 0xFFFFFFFF,
                   .weight = TextStyle::Bold
               }));
               
    auto btn = ShaderButton({
        .sksl_code = GLOW_BUTTON_SKSL,
        .color1 = 0xFF4A90E2,
        .color2 = 0xFF50E3C2,
        .border_radius = 24.0f,
        .width = 200.0f,
        .height = 60.0f,
        .on_tap = []() { printf("Shader button tapped!\n"); },
        .child = center_wrap
    });

    root->child(btn);

    runApp(root, {.title = "Shader Button Demo", .width = 800, .height = 600});
    return 0;
}
