#include "vaura/app/app.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"

using namespace vaura;

class CardDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CardDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFFF1F5F9) // Light background to see shadow clearly
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto title = Text("Card Widget Demo", {
            .font_size = 28,
            .color = 0xFF0F172A,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(YGEdgeBottom, 40).child(title);

        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionColumn)
               .alignItems(YGAlignCenter);

        content->child(title_container);

        auto build_profile_card = []() {
            auto content_col = std::make_shared<FlexBox>();
            content_col->flexDirection(YGFlexDirectionColumn);

            auto name = Text("Aether Desktop", {
                .font_size = 22,
                .color = 0xFF1E293B,
                .weight = TextStyle::Bold
            });
            
            auto desc = Text("Next Generation Operating System Environment built with VAURA.", {
                .font_size = 16,
                .color = 0xFF64748B
            });

            auto name_c = std::make_shared<FlexBox>();
            name_c->margin(YGEdgeBottom, 10).child(name);

            auto desc_c = std::make_shared<FlexBox>();
            desc_c->child(desc);

            content_col->child(name_c).child(desc_c);

            return Card({
                .color = 0xFFFFFFFF,
                .border_radius = 24.0f,
                .elevation = 20.0f, // Big luxurious shadow
                .shadow_color = 0x20000000,
                .shadow_dy = 10.0f,
                .padding = {30, 30, 30, 30}
            }, content_col);
        };

        content->child(build_profile_card());
        root->child(content);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Card Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<CardDemoApp>(), config);
}
