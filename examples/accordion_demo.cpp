#include "vaura/app/app.hpp"
#include "vaura/widgets/accordion.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"

using namespace vaura;

class AccordionDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AccordionDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF0F172A)
            .justify(Justify::Center)
            .align(Align::Center);

        auto content = std::make_shared<FlexBox>();
        content->direction(FlexDirection::Column)
               .align(Align::Stretch)
               .width(400);

        auto title = Text("FAQ", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(Edge::Bottom, 20).child(title);
        content->child(title_container);

        auto answer1 = Text("VAURA is a next-generation retained-mode C++ GUI framework designed for building luxurious and highly animated user interfaces.", {
            .font_size = 15,
            .color = 0xFF94A3B8
        });

        auto answer2 = Text("Yes! It utilizes Anu for flexbox layouts, Skia for high-performance GPU-accelerated rendering, and an architecture inspired by modern declarative frameworks.", {
            .font_size = 15,
            .color = 0xFF94A3B8
        });

        auto acc1 = Accordion({
            .title = "What is VAURA?",
            .content = answer1,
            .initially_expanded = true,
            .background_color = 0xFF1E293B
        });

        auto wrapper1 = std::make_shared<FlexBox>();
        wrapper1->margin(Edge::Bottom, 10).child(acc1);

        auto acc2 = Accordion({
            .title = "Is it hardware accelerated?",
            .content = answer2,
            .initially_expanded = false,
            .background_color = 0xFF1E293B
        });

        auto wrapper2 = std::make_shared<FlexBox>();
        wrapper2->child(acc2);

        content->child(wrapper1).child(wrapper2);

        root->child(content);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Accordion Demo",
        .width = 600,
        .height = 500,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<AccordionDemoApp>(), config);
}
