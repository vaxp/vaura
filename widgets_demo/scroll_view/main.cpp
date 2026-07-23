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
        return ScrollView({
            .child = Column({
                .widthPercent = 100.0f,
                .padding = std::pair{Edge::All, 20.0f},
                .backgroundColor = 0xFF1E293B,
                .children = []() {
                    std::vector<WidgetPtr> items;
                    for (int i = 1; i <= 50; ++i) {
                        items.push_back(
                            Row({
                                .justify = Justify::Start,
                                .align = Align::Center,
                                .widthPercent = 100.0f,
                                .height = 60.0f,
                                .padding = std::pair{Edge::Left, 20.0f},
                                .margin = std::pair{Edge::Bottom, 10.0f},
                                .backgroundColor = 0xFF334155,
                                .borderRadius = 8.0f,
                                .children = {
                                    Text("List Item " + std::to_string(i), {
                                        .font_size = 18,
                                        .color = 0xFFF8FAFC
                                    })
                                }
                            })
                        );
                    }
                    return items;
                }()
            }),
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
