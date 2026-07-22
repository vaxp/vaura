// widgets_demo/text/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class TextDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "TextDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .width = 600.0f,
                    .padding = std::pair{YGEdgeAll, 40.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 16.0f,
                    .children = {
                        // Heading
                        Text("Typography in VAURA", {
                            .font_size = 36.0f, 
                            .color = 0xFFFFFFFF, 
                            .weight = TextStyle::Bold,
                            .align = TextStyle::Center
                        }),
                        Column({.height = 24.0f}),

                        // Subheading
                        Text("Elegant & Responsive Text Rendering", {
                            .font_size = 20.0f, 
                            .color = 0xFF3B82F6, 
                            .weight = TextStyle::SemiBold,
                            .align = TextStyle::Center
                        }),
                        Column({.height = 32.0f}),

                        // Normal text
                        Text("This is a standard body text. It supports different colors and font sizes. The rendering engine uses Skia's Paragraph to provide high quality text layout, including proper kerning and word wrapping.", {
                            .font_size = 16.0f, 
                            .color = 0xFF94A3B8,
                            .align = TextStyle::Left
                        }),
                        Column({.height = 24.0f}),

                        // Max lines and ellipsis
                        Text("This is a very long text that demonstrates how the maximum lines and overflow ellipsis work in the text widget. When the text exceeds the allowed number of lines, it will be automatically truncated and an ellipsis will be appended to the end of the visible text. This is extremely useful for displaying previews of articles, descriptions, or any content that needs to be clamped to fit within a specific UI layout without breaking the design.", {
                            .font_size = 14.0f, 
                            .color = 0xFFCBD5E1,
                            .max_lines = 2,
                            .overflow = TextStyle::Ellipsis
                        }),
                        Column({.height = 24.0f}),

                        // Right aligned
                        Text("Right Aligned Text", {
                            .font_size = 16.0f, 
                            .color = 0xFF10B981,
                            .weight = TextStyle::Bold,
                            .align = TextStyle::Right
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Text Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<TextDemo>(), config);
}
