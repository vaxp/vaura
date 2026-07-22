#include "vaura/app/app.hpp"
#include "vaura/widgets/shader_background.hpp"
#include <iostream>
#include "primitives_sksl.h"

using namespace vaura;

class PrimitivesDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "PrimitivesDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        ShaderBackgroundConfig cfg;
        cfg.sksl_code = PRIMITIVES_SKSL;
        return std::make_shared<ShaderBackgroundWidget>(std::move(cfg));
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tech Flex - Raymarching Primitives",
        .width = 1280,
        .height = 720,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<PrimitivesDemoApp>(), config);
}
