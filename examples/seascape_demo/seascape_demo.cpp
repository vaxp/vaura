#include "vaura/app/app.hpp"
#include "vaura/widgets/shader_background.hpp"
#include <iostream>
#include "seascape_sksl.h"

using namespace vaura;

class SeascapeDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "SeascapeDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        ShaderBackgroundConfig cfg;
        cfg.sksl_code = SEASCAPE_SKSL;
        return std::make_shared<ShaderBackgroundWidget>(std::move(cfg));
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tech Flex - Seascape",
        .width = 1280,
        .height = 720,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<SeascapeDemoApp>(), config);
}
