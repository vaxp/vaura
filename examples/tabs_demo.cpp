#include "vaura/app/app.hpp"
#include "vaura/widgets/tabs.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class TabsDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "TabsDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justify(Justify::Center)
            .align(Align::Center);

        auto content = std::make_shared<FlexBox>();
        content->direction(FlexDirection::Column)
               .align(Align::Stretch)
               .width(500)
               .height(400); // Fixed height to test tab content flexibility

        auto title = Text("Dashboard", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(Edge::Bottom, 20).child(title);
        content->child(title_container);

        auto view1 = std::make_shared<FlexBox>();
        view1->justify(Justify::Center)
             .align(Align::Center)
             .flexGrow(1.0f)
             .backgroundColor(0xFF0F172A)
             .borderRadius(12.0f);
        view1->child(Text("Overview Dashboard Content", {.font_size = 18, .color = 0xFFF8FAFC}));

        auto view2 = std::make_shared<FlexBox>();
        view2->justify(Justify::Center)
             .align(Align::Center)
             .flexGrow(1.0f)
             .backgroundColor(0xFF0F172A)
             .borderRadius(12.0f);
        view2->child(Text("Analytics Charts Here", {.font_size = 18, .color = 0xFFF8FAFC}));

        auto view3 = std::make_shared<FlexBox>();
        view3->justify(Justify::Center)
             .align(Align::Center)
             .flexGrow(1.0f)
             .backgroundColor(0xFF0F172A)
             .borderRadius(12.0f);
        view3->child(Text("System Settings", {.font_size = 18, .color = 0xFFF8FAFC}));

        auto t = Tabs({
            .tabs = {
                {"Overview", view1},
                {"Analytics", view2},
                {"Settings", view3}
            },
            .initial_index = 0
        });

        auto wrapper = std::make_shared<FlexBox>();
        wrapper->flexGrow(1.0f).child(t);

        content->child(wrapper);

        root->child(content);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Tabs Demo",
        .width = 700,
        .height = 600,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<TabsDemoApp>(), config);
}
