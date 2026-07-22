#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include <memory>
#include <string>

using namespace vaura;

class ScrollTestApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ScrollTestApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto content = std::make_shared<FlexBox>();
        content->widthPercent(100).backgroundColor(0xFF1E293B).padding(YGEdgeAll, 20);

        for (int i = 1; i <= 50; ++i) {
            auto item = std::make_shared<FlexBox>();
            item->widthPercent(100)
                .height(60)
                .margin(YGEdgeBottom, 10)
                .backgroundColor(0xFF334155)
                .borderRadius(8)
                .justifyContent(YGJustifyCenter)
                .padding(YGEdgeLeft, 20);

            item->child(Text("List Item " + std::to_string(i), {
                .font_size = 18,
                .color = 0xFFF8FAFC
            }));

            content->child(item);
        }

        // Wrap the long content in a scroll_view
        return ScrollView({
            .child = content,
            .scroll_speed = 40.0f
        });
    }
};

int main() {
    AppConfig config{
        .title = "VAURA - ScrollView Test",
        .width = 400,
        .height = 600,
        .clear_color = 0xFF0F172A
    };
    return runApp(std::make_shared<ScrollTestApp>(), config);
}
