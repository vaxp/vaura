// widgets_demo/transform/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class TransformDemoState : public State {
    float rot_val = 0.0f;
    float scale_val = 1.0f;
    float trans_x = 0.0f;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Transform Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        // Control Panel
        auto controls = std::make_shared<FlexBox>();
        controls->flexDirection(YGFlexDirectionColumn)
                .width(400)
                .padding(YGEdgeAll, 20)
                .backgroundColor(0xFF1E293B)
                .borderRadius(12);

        auto spacer_y = [](float h) {
            auto s = std::make_shared<FlexBox>();
            s->height(h);
            return s;
        };

        controls->child(Text("Rotation", {.color = 0xFFAAAAAA}));
        controls->child(spacer_y(10));
        
        SliderConfig rot_cfg;
        rot_cfg.width = 360.0f;
        rot_cfg.min_value = 0.0f; rot_cfg.max_value = 360.0f; rot_cfg.value = rot_val;
        rot_cfg.on_changed = [this](float v) { setState([this, v]{ rot_val = v; }); };
        controls->child(Slider(rot_cfg));
        controls->child(spacer_y(20));

        controls->child(Text("Scale", {.color = 0xFFAAAAAA}));
        controls->child(spacer_y(10));
        
        SliderConfig sc_cfg;
        sc_cfg.width = 360.0f;
        sc_cfg.min_value = 0.5f; sc_cfg.max_value = 2.0f; sc_cfg.value = scale_val;
        sc_cfg.on_changed = [this](float v) { setState([this, v]{ scale_val = v; }); };
        controls->child(Slider(sc_cfg));
        controls->child(spacer_y(20));

        controls->child(Text("Translate X", {.color = 0xFFAAAAAA}));
        controls->child(spacer_y(10));
        
        SliderConfig tx_cfg;
        tx_cfg.width = 360.0f;
        tx_cfg.min_value = -100.0f; tx_cfg.max_value = 100.0f; tx_cfg.value = trans_x;
        tx_cfg.on_changed = [this](float v) { setState([this, v]{ trans_x = v; }); };
        controls->child(Slider(tx_cfg));
        controls->child(spacer_y(20));

        root->child(controls);

        auto spacer2 = std::make_shared<FlexBox>();
        spacer2->height(60);
        root->child(spacer2);

        // The target to transform
        auto box = std::make_shared<FlexBox>();
        box->width(100).height(100)
           .backgroundColor(0xFF3B82F6) // Blue
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .borderRadius(16);
        box->child(Text("Box", {.font_size = 24.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        TransformConfig tr_cfg;
        tr_cfg.rotation = rot_val;
        tr_cfg.scale_x = scale_val;
        tr_cfg.scale_y = scale_val;
        tr_cfg.translate_x = trans_x;
        tr_cfg.translate_y = 0.0f;
        tr_cfg.child = box;

        root->child(Transform(tr_cfg));

        return root;
    }
};

class TransformDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "TransformDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<TransformDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Transform Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<TransformDemo>(), config);
}
