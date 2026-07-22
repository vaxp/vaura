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
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Split View Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 20.0f}),
                Column({
                    .width = 600.0f,
                    .height = 400.0f,
                    .borderRadius = 16.0f,
                    .overflow = YGOverflowHidden,
                    .children = {
                        SplitView({
                            .first = Column({
                                .justifyContent = YGJustifyCenter,
                                .alignItems = YGAlignCenter,
                                .widthPercent = 100.0f,
                                .heightPercent = 100.0f,
                                .backgroundColor = 0xFF1E293B,
                                .children = { Text("Left Pane", {.font_size = 24.0f, .color = 0xFF3B82F6}) }
                            }),
                            .second = Column({
                                .justifyContent = YGJustifyCenter,
                                .alignItems = YGAlignCenter,
                                .widthPercent = 100.0f,
                                .heightPercent = 100.0f,
                                .backgroundColor = 0xFF334155,
                                .children = { Text("Right Pane", {.font_size = 24.0f, .color = 0xFF10B981}) }
                            }),
                            .axis = SplitViewConfig::Axis::Horizontal,
                            .initial_ratio = 0.4f
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Split View Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<SplitViewDemo>(), config);
}
