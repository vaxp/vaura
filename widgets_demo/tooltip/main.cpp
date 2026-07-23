#include "vaura/app/app.hpp"
#include "vaura/widgets/tooltip.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/rendering/font_manager.hpp"

using namespace vaura;

class TooltipDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "TooltipDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Container({
                    .color = 0xFF1E293B,
                    .padding = EdgeInsets::all(40),
                    .border_radius = BorderRadius::circular(24.0f),
                    .child = Row({
                        .justify = Justify::SpaceAround,
                        .align = Align::Center,
                        .width = 400.0f,
                        .children = {
                            Tooltip({
                                .message = "This is a button tooltip!",
                                .vertical_offset = 8.0f, // Much closer
                                .child = Button({
                                    .child = Text("Hover Me!", {.font_size = 18, .color = 0xFFFFFFFF}),
                                    .on_pressed = [](){}
                                })
                            }),
                            Tooltip({
                                .message = "Profile Settings",
                                .vertical_offset = 8.0f,
                                .background_color = 0xFF3B82F6, // Blue bg
                                .child = Icon(0xf47d, { // Person
                                    .size = 48.0f,
                                    .color = 0xFF94A3B8
                                })
                            }),
                            Tooltip({
                                .message = "Jane Doe (Online)",
                                .vertical_offset = 8.0f,
                                .child = Avatar({
                                    .size = 64.0f,
                                    .background_color = 0xFF10B981,
                                    .initials = "JD",
                                    .font_size = 24.0f
                                })
                            })
                        }
                    })
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tooltip Demo",
        .width = 500,
        .height = 200
    };
    
    // Load icons font
    vaura::FontManager::loadFont("examples/icon_demo/CupertinoIcons.ttf", "CupertinoIcons");
    
    return vaura::runApp(std::make_shared<TooltipDemoApp>(), config);
}
