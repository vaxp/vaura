// widgets_demo/list_view/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/widgets/list_tile.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/icon.hpp"

using namespace vaura;

class ListViewDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ListViewDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("List View Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 20.0f}),
                Column({
                    .width = 600.0f,
                    .height = 500.0f,
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 16.0f,
                    .overflow = YGOverflowHidden,
                    .children = {
                        ScrollView({
                            .child = Column({
                                .children = [this]() {
                                    std::vector<WidgetPtr> items;
                                    // Add 50 items
                                    for (int i = 1; i <= 50; ++i) {
                                        WidgetPtr leading_icon;
                                        if (i % 3 == 0) {
                                            leading_icon = Icon(Icons::HeartFill, {.size=24, .color=0xFFEF4444});
                                        } else if (i % 3 == 1) {
                                            leading_icon = Icon(Icons::Settings, {.size=24, .color=0xFF3B82F6});
                                        } else {
                                            leading_icon = Icon(Icons::PersonFill, {.size=24, .color=0xFF10B981});
                                        }

                                        items.push_back(
                                            Column({
                                                .margin = std::pair{YGEdgeAll, 10.0f},
                                                .children = {
                                                    ListTile({
                                                        .leading = leading_icon,
                                                        .title = Text("List Item " + std::to_string(i), {.font_size = 16.0f, .color = 0xFFFFFFFF}),
                                                        .subtitle = Text("This is a detailed description for item " + std::to_string(i), {.font_size = 14.0f, .color = 0xFF94A3B8}),
                                                        .trailing = Icon(Icons::ChevronRight, {.size=24, .color=0xFF94A3B8}),
                                                        .on_tap = [i]() { /* Handle tap */ }
                                                    })
                                                }
                                            })
                                        );

                                        // Add a separator except for the last item
                                        if (i < 50) {
                                            items.push_back(
                                                Column({
                                                    .height = 1.0f,
                                                    .margin = std::pair{YGEdgeHorizontal, 16.0f},
                                                    .backgroundColor = 0xFF1E293B
                                                })
                                            );
                                        }
                                    }
                                    return items;
                                }()
                            }),
                            .show_scrollbar = true
                        })
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA List View Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<ListViewDemo>(), config);
}
