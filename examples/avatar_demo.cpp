#include "vaura/app/app.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/badge.hpp"

using namespace vaura;

class AvatarDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AvatarDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .alignItems(YGAlignCenter)
           .justifyContent(YGJustifySpaceEvenly)
           .width(500);

        // Avatar 1: With Image
        auto av1 = Avatar({
            .size = 80.0f,
            .image_path = "examples/direct_yoga_test/logo.png"
        });

        // Wrap av1 in a Badge to show online status!
        auto status_badge = Badge({
            .text = "", // empty text, just a dot
            .background_color = 0xFF10B981, // Green
            .padding = 6.0f,
            .offset = {-6.0f, 6.0f}, // bottom right! Wait, badge goes top-right by default.
            // But we'll just use default top-right for now.
            .child = av1
        });

        // Avatar 2: Initials
        auto av2 = Avatar({
            .size = 80.0f,
            .background_color = 0xFF8B5CF6, // Purple
            .initials = "MS",
            .font_size = 32.0f
        });

        // Avatar 3: Initials
        auto av3 = Avatar({
            .size = 80.0f,
            .background_color = 0xFFF59E0B, // Amber
            .initials = "JD",
            .text_color = 0xFF1E293B,
            .font_size = 32.0f
        });

        // Create vertical containers with names below
        auto create_profile = [](WidgetPtr av, std::string name) {
            auto c = std::make_shared<FlexBox>();
            c->alignItems(YGAlignCenter);
            
            c->child(av);
            
            auto t = Text(name, {
                .font_size = 18.0f,
                .color = 0xFFCBD5E1,
                .weight = TextStyle::SemiBold
            });
            auto tc = std::make_shared<FlexBox>();
            tc->margin(YGEdgeTop, 15.0f).child(t);
            c->child(tc);
            return c;
        };

        row->child(create_profile(status_badge, "System"))
           .child(create_profile(av2, "Michael Scott"))
           .child(create_profile(av3, "Jane Doe"));

        auto card_container = Container({
            .color = 0xFF0F172A,
            .padding = {40, 40, 40, 40},
            .border_radius = BorderRadius::circular(24.0f),
            .child = row
        });

        root->child(card_container);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Avatar Demo",
        .width = 700,
        .height = 400
    };
    
    return vaura::runApp(std::make_shared<AvatarDemoApp>(), config);
}
