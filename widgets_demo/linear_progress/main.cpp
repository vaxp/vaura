#include "vaura/app/app.hpp"
#include "vaura/widgets/linear_progress.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class LinearProgressDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "LinearProgressDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class LinearProgressDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF1E293B,
            .children = {
                Card({
                    .color = 0xFF0F172A,
                    .border_radius = 20.0f,
                    .elevation = 15.0f,
                    .shadow_color = 0x80000000,
                    .padding = {30, 30, 30, 30}
                }, Column({
                        .alignItems = YGAlignStretch,
                        .width = 300.0f,
                        .children = {
                            // Indeterminate Progress
                            Column({
                                .margin = std::pair{YGEdgeBottom, 10.0f},
                                .children = {
                                    Text("Indeterminate Progress", { .font_size = 18, .color = 0xFFFFFFFF, .weight = TextStyle::Bold })
                                }
                            }),
                            Column({
                                .margin = std::pair{YGEdgeBottom, 30.0f},
                                .children = {
                                    LinearProgress({
                                        .value = std::nullopt,
                                        .thickness = 6.0f,
                                        .border_radius = 3.0f,
                                        .active_color = 0xFF3B82F6, // Blue
                                        .inactive_color = 0xFF334155  // Slate 700
                                    })
                                }
                            }),

                            // Determinate Progress
                            Column({
                                .margin = std::pair{YGEdgeBottom, 10.0f},
                                .children = {
                                    Text("Determinate Progress (75%)", { .font_size = 18, .color = 0xFFFFFFFF, .weight = TextStyle::Bold })
                                }
                            }),
                            LinearProgress({
                                .value = 0.75f,
                                .thickness = 6.0f,
                                .border_radius = 3.0f,
                                .active_color = 0xFF10B981, // Emerald
                                .inactive_color = 0xFF334155  // Slate 700
                            })
                        }
                    })
                )
            }
        });
    }
};

std::unique_ptr<State> LinearProgressDemoApp::createState() {
    return std::make_unique<LinearProgressDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA LinearProgress Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<LinearProgressDemoApp>(), config);
}
