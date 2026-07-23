#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class AvatarDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AvatarDemo"; }

    WidgetPtr build(BuildContext& context) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .gap = 40.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Row({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .gap = 32.0f,
                    .children = {
                        Avatar({
                            .size = 48.0f, 
                            .image_path = "assets/vaxp.png", 
                            .background_color = 0xFFEC4899, 
                            .initials = "SM", 
                            .font_size = 18.0f
                        }),
                        Avatar({
                            .size = 72.0f, 
                            .image_path = "assets/1.png", 
                            .background_color = 0xFF3B82F6, 
                            .initials = "MD", 
                            .font_size = 28.0f
                        }),
                        Avatar({
                            .size = 100.0f, 
                            .background_color = 0xFF10B981, 
                            .initials = "LG", 
                            .font_size = 40.0f
                        })
                    }
                }),
                Text("Avatar Showcase (Initials Fallback)", {
                    .font_size = 20.0f,
                    .color = 0xFF94A3B8
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Avatar Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<AvatarDemo>(), config);
}
