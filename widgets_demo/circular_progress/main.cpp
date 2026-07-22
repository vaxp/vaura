#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/circular_progress.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class CircularProgressDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CircularProgressDemo"; }

    WidgetPtr build(BuildContext& context) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .gap = 60.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                Text("Loading States", {.font_size = 32.0f, .color = 0xFFFFFFFF}),

                // Row for indicators
                Row({
                    .justifyContent = YGJustifyCenter,
                    .alignItems = YGAlignCenter,
                    .gap = 80.0f,
                    .children = {
                        // 1. Indeterminate Progress
                        Column({
                            .alignItems = YGAlignCenter,
                            .gap = 16.0f,
                            .children = {
                                CircularProgress({
                                    .color = 0xFF8B5CF6, // Violet 500
                                    .stroke_width = 8.0f,
                                    .size = 80.0f
                                }),
                                Text("Indeterminate", {.font_size = 18.0f, .color = 0xFF94A3B8})
                            }
                        }),

                        // 2. Determinate Progress
                        Column({
                            .alignItems = YGAlignCenter,
                            .gap = 16.0f,
                            .children = {
                                CircularProgress({
                                    .value = 0.75f, // 75%
                                    .color = 0xFF10B981, // Emerald 500
                                    .background_color = 0xFF1E293B, // Slate 800
                                    .stroke_width = 8.0f,
                                    .size = 80.0f
                                }),
                                Text("75%", {.font_size = 18.0f, .color = 0xFF94A3B8})
                            }
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Circular Progress Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<CircularProgressDemo>(), config);
}
