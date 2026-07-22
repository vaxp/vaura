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
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto content_col = std::make_shared<FlexBox>();
        content_col->flexDirection(YGFlexDirectionColumn)
                   .width(300);

        auto item1 = text("Item 1", {.font_size = 18, .color = 0xFFFFFFFF});
        auto item2 = text("Item 2", {.font_size = 18, .color = 0xFFFFFFFF});
        auto item3 = text("Item 3", {.font_size = 18, .color = 0xFFFFFFFF});

        auto div1 = divider({.thickness = 1.0f, .color = 0xFF475569, .indent = 10.0f, .end_indent = 10.0f});
        auto div2 = divider({.thickness = 1.0f, .color = 0xFF475569, .indent = 10.0f, .end_indent = 10.0f});

        auto wrap1 = std::make_shared<FlexBox>(); wrap1->padding(YGEdgeAll, 15).child(item1);
        auto wrap2 = std::make_shared<FlexBox>(); wrap2->padding(YGEdgeAll, 15).child(item2);
        auto wrap3 = std::make_shared<FlexBox>(); wrap3->padding(YGEdgeAll, 15).child(item3);

        content_col->child(wrap1);
        content_col->child(div1);
        content_col->child(wrap2);
        content_col->child(div2);
        content_col->child(wrap3);

        auto c = card({
            .color = 0xFF0F172A,
            .border_radius = 16.0f,
            .elevation = 10.0f,
            .shadow_color = 0x80000000,
            .padding = {0,0,0,0} // padding handles spacing
        }, content_col);

        root->child(c);
        return root;
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
