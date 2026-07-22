#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ContainerDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .gap(YGGutterAll, 60.0f)
            .backgroundColor(0xFF0F172A) // Slate 900
            .widthPercent(100).heightPercent(100);

        auto title = Text("Container Showcase", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold});
        root->child(title);

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 40.0f);

        // 1. Pill Shape Container
        auto inner_pill = std::make_shared<FlexBox>();
        inner_pill->width(200).height(80)
                  .justifyContent(YGJustifyCenter)
                  .alignItems(YGAlignCenter)
                  .child(Text("Pill Shape", {.font_size = 20.0f, .color = 0xFFFFFFFF}));
                  
        auto pill_container = Container({
            .color = 0xFF3B82F6, // Blue 500
            .border_radius = BorderRadius(40.0f), // Half of height
            .shadow = Shadow{0x603B82F6, 24.0f, {0, 12}},
            .child = inner_pill
        });
        row->child(pill_container);

        // 2. Glassmorphism-style Container
        auto inner_glass = std::make_shared<FlexBox>();
        inner_glass->width(180).height(180)
                   .justifyContent(YGJustifyCenter)
                   .alignItems(YGAlignCenter)
                   .child(Text("Deep Shadow", {.font_size = 20.0f, .color = 0xFFFFFFFF}));
                   
        auto glass_container = Container({
            .color = 0xFF1E293B, // Slate 800
            .border_radius = BorderRadius(24.0f),
            .border = Border(0xFF334155, 2.0f),
            .shadow = Shadow{0x80000000, 32.0f, {0, 16}},
            .child = inner_glass
        });
        row->child(glass_container);

        // 3. Clipped Image Container (Asymmetric radius)
        auto inner_clip = std::make_shared<FlexBox>();
        inner_clip->width(180).height(180)
                  .justifyContent(YGJustifyCenter)
                  .alignItems(YGAlignCenter)
                  .backgroundColor(0xFF8B5CF6) // Violet 500
                  .child(Text("Asymmetric", {.font_size = 20.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
                  
        auto clipped_container = Container({
            .color = 0x00000000,
            .border_radius = BorderRadius{60.0f, 16.0f, 60.0f, 16.0f}, // Asymmetric
            .shadow = Shadow{0x508B5CF6, 20.0f, {0, 10}},
            .clip_content = true,
            .child = inner_clip
        });
        row->child(clipped_container);

        root->child(row);
        return root;
    }
};

class ContainerDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ContainerDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ContainerDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Container Demo";
    config.width = 900;
    config.height = 600;

    return runApp(std::make_shared<ContainerDemo>(), config);
}
