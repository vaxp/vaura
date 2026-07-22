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
        std::string sksl_code = R"(
            uniform float u_time;
            uniform vec2  u_resolution;
            
            vec4 main(vec2 fragCoord) {
                vec2 uv = fragCoord.xy / u_resolution.xy;
                vec3 col = 0.5 + 0.5*cos(u_time + uv.xyx + vec3(0,2,4));
                return vec4(col, 1.0);
            }
        )";

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Shader Background Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                Column({
                    .width = 600.0f,
                    .height = 400.0f,
                    .borderRadius = 16.0f,
                    .overflow = YGOverflowHidden,
                    .children = {
                        Stack({
                            .alignment = Alignment::Center,
                            .children = {
                                Column({
                                    .widthPercent = 100.0f,
                                    .heightPercent = 100.0f,
                                    .children = {
                                        std::make_shared<ShaderBackgroundWidget>(ShaderBackgroundConfig{
                                            .sksl_code = sksl_code
                                        })
                                    }
                                }),
                                Column({
                                    .justifyContent = YGJustifyCenter,
                                    .alignItems = YGAlignCenter,
                                    .widthPercent = 100.0f,
                                    .heightPercent = 100.0f,
                                    .children = {
                                        Text("Hello GLSL!", {.font_size = 48.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                                    }
                                })
                            }
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Shader Background Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ShaderBackgroundDemo>(), config);
}
