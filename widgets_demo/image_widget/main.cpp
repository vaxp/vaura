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
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        auto title = std::make_shared<FlexBox>();
        title->margin(YGEdgeBottom, 40)
             .child(text("Image Widget Gallery", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        root->child(title);

        auto grid = std::make_shared<FlexBox>();
        grid->flexDirection(YGFlexDirectionRow)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .widthPercent(100);

        auto make_image_card = [](const std::string& label, BoxFit fit) {
            auto card_col = std::make_shared<FlexBox>();
            card_col->flexDirection(YGFlexDirectionColumn)
                     .alignItems(YGAlignCenter)
                     .margin(YGEdgeAll, 20);

            auto img_wrapper = std::make_shared<FlexBox>();
            img_wrapper->width(200).height(200)
                       .backgroundColor(0xFF1E293B)
                       .borderRadius(20.0f) // rounded corners
                       .justifyContent(YGJustifyCenter)
                       .alignItems(YGAlignCenter);
                       
            ImageConfig img_cfg;
            img_cfg.path = "assets/0.png";
            img_cfg.width = 200.0f;
            img_cfg.height = 200.0f;
            img_cfg.fit = fit;
            img_cfg.border_radius = 20.0f;
            
            img_wrapper->child(image(img_cfg));

            auto txt_wrap = std::make_shared<FlexBox>();
            txt_wrap->margin(YGEdgeTop, 16)
                    .child(text(label, {.font_size = 16.0f, .color = 0xFF94A3B8}));

            card_col->child(img_wrapper);
            card_col->child(txt_wrap);
            return card_col;
        };

        grid->child(make_image_card("BoxFit::Contain", BoxFit::Contain));
        grid->child(make_image_card("BoxFit::Cover", BoxFit::Cover));

        root->child(grid);
        return root;
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
