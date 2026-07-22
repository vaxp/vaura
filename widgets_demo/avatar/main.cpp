#include "vaura/app/app.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class AvatarDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AvatarDemo"; }

    WidgetPtr build(BuildContext& context) override {
        // Main layout (Centered Column)
        auto col = std::make_shared<FlexBox>();
        col->flexDirection(YGFlexDirectionColumn)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 40.0f)
           .widthPercent(100.0f)
           .heightPercent(100.0f)
           .backgroundColor(0xFF0F172A);

        // Row for Avatars
        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 32.0f);

        // 1. Small Avatar
        AvatarConfig small_cfg;
        small_cfg.size = 48.0f;
        small_cfg.image_path = "assets/vaxp.png";
        small_cfg.background_color = 0xFFEC4899; // Pink 500
        small_cfg.initials = "SM";
        small_cfg.font_size = 18.0f;
        row->child(avatar(small_cfg));

        // 2. Medium Avatar
        AvatarConfig med_cfg;
        med_cfg.size = 72.0f;
        med_cfg.image_path = "assets/1.png";
        med_cfg.background_color = 0xFF3B82F6; // Blue 500
        med_cfg.initials = "MD";
        med_cfg.font_size = 28.0f;
        row->child(avatar(med_cfg));

        // 3. Large Avatar
        AvatarConfig large_cfg;
        large_cfg.size = 100.0f;
        large_cfg.background_color = 0xFF10B981; // Emerald 500
        large_cfg.initials = "LG";
        large_cfg.font_size = 40.0f;
        row->child(avatar(large_cfg));

        col->child(row);

        // Subtitle
        TextConfig sub_cfg;
        sub_cfg.font_size = 20.0f;
        sub_cfg.color = 0xFF94A3B8; // Slate 400
        col->child(std::make_shared<TextWidget>("Avatar Showcase (Initials Fallback)", sub_cfg));

        return col;
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
