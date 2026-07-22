// widgets_demo/text/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class TextDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "TextDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        auto container = std::make_shared<FlexBox>();
        container->flexDirection(YGFlexDirectionColumn)
                 .backgroundColor(0xFF1E293B)
                 .padding(YGEdgeAll, 40)
                 .borderRadius(16)
                 .width(600);

        // Heading
        container->child(Text("Typography in VAURA", {
            .font_size = 36.0f, 
            .color = 0xFFFFFFFF, 
            .weight = TextStyle::Bold,
            .align = TextStyle::Center
        }));

        auto spacer1 = std::make_shared<FlexBox>(); spacer1->height(24);
        container->child(spacer1);

        // Subheading
        container->child(Text("Elegant & Responsive Text Rendering", {
            .font_size = 20.0f, 
            .color = 0xFF3B82F6, 
            .weight = TextStyle::SemiBold,
            .align = TextStyle::Center
        }));

        auto spacer2 = std::make_shared<FlexBox>(); spacer2->height(32);
        container->child(spacer2);

        // Normal text
        container->child(Text("This is a standard body text. It supports different colors and font sizes. The rendering engine uses Skia's Paragraph to provide high quality text layout, including proper kerning and word wrapping.", {
            .font_size = 16.0f, 
            .color = 0xFF94A3B8,
            .align = TextStyle::Left
        }));

        auto spacer3 = std::make_shared<FlexBox>(); spacer3->height(24);
        container->child(spacer3);

        // Max lines and ellipsis
        container->child(Text("This is a very long text that demonstrates how the maximum lines and overflow ellipsis work in the text widget. When the text exceeds the allowed number of lines, it will be automatically truncated and an ellipsis will be appended to the end of the visible text. This is extremely useful for displaying previews of articles, descriptions, or any content that needs to be clamped to fit within a specific UI layout without breaking the design.", {
            .font_size = 14.0f, 
            .color = 0xFFCBD5E1,
            .max_lines = 2,
            .overflow = TextStyle::Ellipsis
        }));

        auto spacer4 = std::make_shared<FlexBox>(); spacer4->height(24);
        container->child(spacer4);

        // Right aligned
        container->child(Text("Right Aligned Text", {
            .font_size = 16.0f, 
            .color = 0xFF10B981,
            .weight = TextStyle::Bold,
            .align = TextStyle::Right
        }));

        root->child(container);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Text Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<TextDemo>(), config);
}
