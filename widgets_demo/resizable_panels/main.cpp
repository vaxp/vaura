// widgets_demo/resizable_panels/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/resizable_panels.hpp"

using namespace vaura;

class ResizablePanelsDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ResizablePanelsDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Resizable Panels Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(20);
        root->child(spacer);

        auto pane1 = std::make_shared<FlexBox>();
        pane1->backgroundColor(0xFF1E293B)
             .justifyContent(YGJustifyCenter)
             .alignItems(YGAlignCenter)
             .widthPercent(100).heightPercent(100);
        pane1->child(Text("Sidebar", {.font_size = 20.0f, .color = 0xFF3B82F6}));

        auto pane2 = std::make_shared<FlexBox>();
        pane2->backgroundColor(0xFF334155)
             .justifyContent(YGJustifyCenter)
             .alignItems(YGAlignCenter)
             .widthPercent(100).heightPercent(100);
        pane2->child(Text("Main Editor", {.font_size = 20.0f, .color = 0xFF10B981}));

        auto pane3 = std::make_shared<FlexBox>();
        pane3->backgroundColor(0xFF475569)
             .justifyContent(YGJustifyCenter)
             .alignItems(YGAlignCenter)
             .widthPercent(100).heightPercent(100);
        pane3->child(Text("Terminal", {.font_size = 20.0f, .color = 0xFFF59E0B}));

        ResizablePanelsConfig panels_cfg;
        panels_cfg.axis = ResizablePanelsConfig::Axis::Horizontal;
        panels_cfg.total_size = 800.0f; // Ensure this matches the wrapper's width below
        panels_cfg.panels = {
            {pane1, 200.0f, 100.0f, 400.0f}, // Initial 200px, min 100, max 400
            {pane2, 400.0f, 200.0f, 0.0f},   // Initial 400px, min 200, no max
            {pane3, 200.0f, 100.0f, 0.0f}    // Initial 200px, min 100, no max
        };

        auto panels_widget = ResizablePanels(panels_cfg);

        auto wrapper = std::make_shared<FlexBox>();
        wrapper->width(800).height(400)
               .borderRadius(16)
               .overflow(YGOverflowHidden)
               .child(panels_widget);

        root->child(wrapper);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Resizable Panels Demo";
    config.width = 1000;
    config.height = 700;
    return runApp(std::make_shared<ResizablePanelsDemo>(), config);
}
