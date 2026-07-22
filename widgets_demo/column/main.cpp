#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ColumnDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionRow)
            .justifyContent(YGJustifySpaceEvenly)
            .alignItems(YGAlignStretch)
            .padding(YGEdgeAll, 40.0f)
            .backgroundColor(0xFF0F172A) // Slate 900
            .widthPercent(100).heightPercent(100);
            
        auto make_box = [](const std::string& text_val, Color color) {
            auto content = std::make_shared<FlexBox>();
            content->justifyContent(YGJustifyCenter)
                   .alignItems(YGAlignCenter)
                   .width(100).height(60)
                   .child(Text(text_val, {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
            return Container({
                .color = color,
                .border_radius = BorderRadius(8.0f),
                .shadow = Shadow{0x40000000, 8.0f, {0, 4}},
                .child = content
            });
        };

        // Column 1: FlexStart (Left/Top aligned)
        auto col1 = std::make_shared<FlexBox>();
        col1->flexDirection(YGFlexDirectionColumn)
            .alignItems(YGAlignFlexStart)
            .gap(YGGutterAll, 16.0f)
            .padding(YGEdgeAll, 20.0f)
            .backgroundColor(0xFF1E293B) // Slate 800
            .borderRadius(16.0f)
            .width(200);
            
        auto title1 = std::make_shared<FlexBox>();
        title1->margin(YGEdgeBottom, 10).child(Text("Flex Start", {.font_size = 20.0f, .color = 0xFF94A3B8}));
        col1->child(title1);
        col1->child(make_box("Box 1", 0xFF3B82F6)); // Blue
        col1->child(make_box("Box 2", 0xFF8B5CF6)); // Violet
        col1->child(make_box("Box 3", 0xFFEC4899)); // Pink

        // Column 2: Center with SpaceBetween
        auto col2 = std::make_shared<FlexBox>();
        col2->flexDirection(YGFlexDirectionColumn)
            .alignItems(YGAlignCenter)
            .justifyContent(YGJustifySpaceBetween)
            .padding(YGEdgeAll, 20.0f)
            .backgroundColor(0xFF1E293B)
            .borderRadius(16.0f)
            .width(200);
            
        auto title2 = std::make_shared<FlexBox>();
        title2->child(Text("Space Between", {.font_size = 20.0f, .color = 0xFF94A3B8}));
        col2->child(title2);
        col2->child(make_box("Box A", 0xFF10B981)); // Emerald
        col2->child(make_box("Box B", 0xFFF59E0B)); // Amber
        col2->child(make_box("Box C", 0xFFEF4444)); // Red

        // Column 3: FlexEnd (Right aligned)
        auto col3 = std::make_shared<FlexBox>();
        col3->flexDirection(YGFlexDirectionColumn)
            .alignItems(YGAlignFlexEnd)
            .gap(YGGutterAll, 16.0f)
            .padding(YGEdgeAll, 20.0f)
            .backgroundColor(0xFF1E293B)
            .borderRadius(16.0f)
            .width(200);
            
        auto title3 = std::make_shared<FlexBox>();
        title3->margin(YGEdgeBottom, 10).child(Text("Flex End", {.font_size = 20.0f, .color = 0xFF94A3B8}));
        col3->child(title3);
        col3->child(make_box("Alpha", 0xFF06B6D4)); // Cyan
        col3->child(make_box("Beta", 0xFF6366F1));  // Indigo
        col3->child(make_box("Gamma", 0xFF14B8A6)); // Teal

        root->child(col1).child(col2).child(col3);
        return root;
    }
};

class ColumnDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ColumnDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ColumnDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Column Demo";
    config.width = 800;
    config.height = 600;

    return runApp(std::make_shared<ColumnDemo>(), config);
}
