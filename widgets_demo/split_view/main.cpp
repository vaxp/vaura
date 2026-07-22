// widgets_demo/split_view/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/split_view.hpp"

using namespace vaura;

class SplitViewDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "SplitViewDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(text("Split View Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(20);
        root->child(spacer);

        auto first_pane = std::make_shared<FlexBox>();
        first_pane->backgroundColor(0xFF1E293B)
                  .justifyContent(YGJustifyCenter)
                  .alignItems(YGAlignCenter)
                  .widthPercent(100).heightPercent(100);
        first_pane->child(text("Left Pane", {.font_size = 24.0f, .color = 0xFF3B82F6}));

        auto second_pane = std::make_shared<FlexBox>();
        second_pane->backgroundColor(0xFF334155)
                   .justifyContent(YGJustifyCenter)
                   .alignItems(YGAlignCenter)
                   .widthPercent(100).heightPercent(100);
        second_pane->child(text("Right Pane", {.font_size = 24.0f, .color = 0xFF10B981}));

        SplitViewConfig split_cfg;
        split_cfg.first = first_pane;
        split_cfg.second = second_pane;
        split_cfg.axis = SplitViewConfig::Axis::Horizontal;
        split_cfg.initial_ratio = 0.4f;

        auto split = split_view(split_cfg);

        auto split_wrapper = std::make_shared<FlexBox>();
        split_wrapper->width(600).height(400)
                     .borderRadius(16)
                     .overflow(YGOverflowHidden)
                     .child(split);

        root->child(split_wrapper);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Split View Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<SplitViewDemo>(), config);
}
