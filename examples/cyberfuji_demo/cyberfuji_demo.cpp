#include "vaura/app/app.hpp"
#include "vaura/widgets/shader_background.hpp"
#include <iostream>
#include "cyber_fuji_sksl.h"

using namespace vaura;

class CyberFujiDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CyberFujiDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        ShaderBackgroundConfig cfg;
        cfg.sksl_code = CYBER_FUJI_SKSL;
        return std::make_shared<ShaderBackgroundWidget>(std::move(cfg));
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tech Flex - Cyber Fuji 2020",
        .width = 1280,
        .height = 720,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<CyberFujiDemoApp>(), config);
}
