// widgets_demo/stack/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/stack.hpp"

using namespace vaura;

class StackDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "StackDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Stack Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        auto bottom_box = std::make_shared<FlexBox>();
        bottom_box->width(300).height(300)
                  .backgroundColor(0xFF1E293B)
                  .borderRadius(16)
                  .justifyContent(YGJustifyFlexEnd)
                  .alignItems(YGAlignFlexEnd)
                  .padding(YGEdgeAll, 16);
        bottom_box->child(Text("Base", {.font_size = 18.0f, .color = 0xFF94A3B8}));

        auto middle_box = std::make_shared<FlexBox>();
        middle_box->width(200).height(200)
                  .backgroundColor(0xFF3B82F6)
                  .borderRadius(12)
                  .justifyContent(YGJustifyFlexEnd)
                  .alignItems(YGAlignFlexEnd)
                  .padding(YGEdgeAll, 16);
        middle_box->child(Text("Middle", {.font_size = 18.0f, .color = 0xFFFFFFFF}));

        auto top_box = std::make_shared<FlexBox>();
        top_box->width(100).height(100)
                 .backgroundColor(0xFFEF4444)
                 .borderRadius(8)
                 .justifyContent(YGJustifyFlexEnd)
                 .alignItems(YGAlignFlexEnd)
                 .padding(YGEdgeAll, 8);
        top_box->child(Text("Top", {.font_size = 16.0f, .color = 0xFFFFFFFF}));

        StackConfig stack_cfg;
        stack_cfg.alignment = Alignment::Center;
        stack_cfg.children = {bottom_box, middle_box, top_box};

        auto stack_widget = Stack(stack_cfg);
        root->child(stack_widget);

        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Stack Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<StackDemo>(), config);
}
