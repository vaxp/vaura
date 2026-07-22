#include "vaura/app/app.hpp"
#include "vaura/widgets/shader_background.hpp"
#include <iostream>
#include "cube_lovers_sksl.h"

using namespace vaura;

class CubeLoversDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CubeLoversDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        ShaderBackgroundConfig cfg;
        cfg.sksl_code = CUBE_LOVERS_SKSL;
        return std::make_shared<ShaderBackgroundWidget>(std::move(cfg));
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tech Flex - Cube Lovers (ShaderToy Port)",
        .width = 1280,
        .height = 720,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<CubeLoversDemoApp>(), config);
}
