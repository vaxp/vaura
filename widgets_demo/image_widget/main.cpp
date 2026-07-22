// widgets_demo/image_widget/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/image_widget.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ImageWidgetDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto make_image_card = [](const std::string& label, BoxFit fit) {
            return Column({
                .alignItems = YGAlignCenter,
                .margin = std::pair{YGEdgeAll, 20.0f},
                .children = {
                    Column({
                        .justifyContent = YGJustifyCenter,
                        .alignItems = YGAlignCenter,
                        .width = 200.0f,
                        .height = 200.0f,
                        .backgroundColor = 0xFF1E293B,
                        .borderRadius = 20.0f, // rounded corners
                        .children = {
                            ImageView({
                                .path = "assets/0.png",
                                .width = 200.0f,
                                .height = 200.0f,
                                .fit = fit,
                                .border_radius = 20.0f
                            })
                        }
                    }),
                    Column({
                        .margin = std::pair{YGEdgeTop, 16.0f},
                        .children = {
                            Text(label, {.font_size = 16.0f, .color = 0xFF94A3B8})
                        }
                    })
                }
            });
        };

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .margin = std::pair{YGEdgeBottom, 40.0f},
                    .children = {
                        Text("Image Widget Gallery", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                }),
                Row({
                    .justifyContent = YGJustifyCenter,
                    .alignItems = YGAlignCenter,
                    .widthPercent = 100.0f,
                    .children = {
                        make_image_card("BoxFit::Contain", BoxFit::Contain),
                        make_image_card("BoxFit::Cover", BoxFit::Cover)
                    }
                })
            }
        });
    }
};

class ImageWidgetDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ImageWidgetDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ImageWidgetDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Image Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ImageWidgetDemo>(), config);
}
