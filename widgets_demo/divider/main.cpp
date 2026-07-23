#include "vaura/app/app.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/card.hpp"

using namespace vaura;

class DividerDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "DividerDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B,
            .children = {
                Card({
                    .color = 0xFF0F172A,
                    .border_radius = 16.0f,
                    .elevation = 10.0f,
                    .shadow_color = 0x80000000,
                    .padding = {0,0,0,0} // padding handles spacing
                }, Column({
                    .width = 300.0f,
                    .children = {
                        Column({
                            .padding = std::pair{Edge::All, 15.0f},
                            .children = { Text("Item 1", {.font_size = 18, .color = 0xFFFFFFFF}) }
                        }),
                        Divider({.thickness = 1.0f, .color = 0xFF475569, .indent = 10.0f, .end_indent = 10.0f}),
                        Column({
                            .padding = std::pair{Edge::All, 15.0f},
                            .children = { Text("Item 2", {.font_size = 18, .color = 0xFFFFFFFF}) }
                        }),
                        Divider({.thickness = 1.0f, .color = 0xFF475569, .indent = 10.0f, .end_indent = 10.0f}),
                        Column({
                            .padding = std::pair{Edge::All, 15.0f},
                            .children = { Text("Item 3", {.font_size = 18, .color = 0xFFFFFFFF}) }
                        })
                    }
                }))
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Divider Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<DividerDemoApp>(), config);
}
