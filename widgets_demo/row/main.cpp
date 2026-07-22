// widgets_demo/row/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"

using namespace vaura;

class RowDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "RowDemo"; }

    WidgetPtr createRow(YGJustify justify, const std::string& label) {
        auto row_container = std::make_shared<FlexBox>();
        row_container->flexDirection(YGFlexDirectionColumn)
                     .margin(YGEdgeBottom, 32);

        row_container->child(Text(label, {.font_size = 18.0f, .color = 0xFF94A3B8, .weight = TextStyle::Bold}));

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .justifyContent(justify)
           .alignItems(YGAlignCenter)
           .backgroundColor(0xFF1E293B)
           .padding(YGEdgeAll, 16)
           .margin(YGEdgeTop, 12)
           .borderRadius(8)
           .width(600);

        // Add 3 buttons to each row
        ButtonConfig b1, b2, b3;
        b1.child = Text("Item 1", {.font_size = 14.0f, .color = 0xFFFFFFFF}); b1.color = 0xFF3B82F6;
        b2.child = Text("Item 2", {.font_size = 14.0f, .color = 0xFFFFFFFF}); b2.color = 0xFF10B981;
        b3.child = Text("Item 3", {.font_size = 14.0f, .color = 0xFFFFFFFF}); b3.color = 0xFFEF4444;

        row->child(Button(b1));
        
        auto spacer1 = std::make_shared<FlexBox>();
        spacer1->width(10);
        auto spacer2 = std::make_shared<FlexBox>();
        spacer2->width(10);
        
        // Only add manual spacers if we're not using a space-distributing justify
        if (justify == YGJustifyFlexStart || justify == YGJustifyFlexEnd || justify == YGJustifyCenter) {
            row->child(spacer1);
            row->child(Button(b2));
            row->child(spacer2);
            row->child(Button(b3));
        } else {
            row->child(Button(b2));
            row->child(Button(b3));
        }

        row_container->child(row);
        return row_container;
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Row & Flex Alignment Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        
        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        root->child(createRow(YGJustifyFlexStart, "MainAxisAlignment: Start"));
        root->child(createRow(YGJustifyCenter, "MainAxisAlignment: Center"));
        root->child(createRow(YGJustifySpaceBetween, "MainAxisAlignment: Space Between"));
        root->child(createRow(YGJustifySpaceEvenly, "MainAxisAlignment: Space Evenly"));

        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Row Demo";
    config.width = 800;
    config.height = 800;
    return runApp(std::make_shared<RowDemo>(), config);
}
