#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include <memory>

using namespace vaura;

class HelloWorldApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "HelloWorldApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A); // Dark slate background

        auto title = Text("Hello, VAURA!", {
            .font_size = 64,
            .color = 0xFF5EEAD4, // Turquoise/Cyan
            .weight = TextStyle::Bold
        });

        auto subtitle = Text("Built with C++ + Skia", {
            .font_size = 24,
            .color = 0xFF94A3B8, // Slate gray
            .weight = TextStyle::Regular
        });

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(16); // Gap between title and subtitle

        root->child(title);
        root->child(spacer);
        root->child(subtitle);

        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Hello World - VAURA",
        .width = 800,
        .height = 600,
        .resizable = true,
        .vsync = true,
        .msaa = true,
        .clear_color = 0xFF0F172A // Match root background
    };

    return vaura::runApp(std::make_shared<HelloWorldApp>(), config);
}
