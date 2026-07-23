// widgets_demo/icon/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class IconDemoState : public State {
    int hovered_idx = -1;

public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .margin = std::pair{Edge::Bottom, 40.0f},
                    .children = {
                        Text("Icon Gallery", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                }),
                Row({
                    .justify = Justify::Center,
                    .align = Align::Center,
                    .wrap = FlexWrap::Wrap,
                    .width = 500.0f,
                    .children = [this]() {
                        std::vector<WidgetPtr> items;
                        std::vector<std::pair<IconData, std::string>> icons = {
                            {Icons::House, "Home"},
                            {Icons::Gear, "Settings"},
                            {Icons::Person, "User"},
                            {Icons::Heart, "Favorite"},
                            {Icons::Star, "Rate"}
                        };

                        for (size_t i = 0; i < icons.size(); ++i) {
                            bool is_hovered = (static_cast<int>(i) == hovered_idx);
                            items.push_back(
                                GestureDetector({
                                    .child = Column({
                                        .justify = Justify::Center,
                                        .align = Align::Center,
                                        .width = 120.0f,
                                        .height = 120.0f,
                                        .margin = std::pair{Edge::All, 10.0f},
                                        .backgroundColor = is_hovered ? 0xFF334155 : 0xFF1E293B,
                                        .borderRadius = 16.0f,
                                        .children = {
                                            Icon(icons[i].first, {
                                                .size = is_hovered ? 56.0f : 48.0f,
                                                .color = is_hovered ? 0xFF0EA5E9 : 0xFF94A3B8
                                            }),
                                            Column({
                                                .margin = std::pair{Edge::Top, 12.0f},
                                                .children = {
                                                    Text(icons[i].second, {.font_size = 14.0f, .color = is_hovered ? 0xFFF8FAFC : 0xFF94A3B8})
                                                }
                                            })
                                        }
                                    }),
                                    .on_hover_enter = [this, i]() { setState([this, i]() { hovered_idx = i; }); },
                                    .on_hover_exit = [this, i]() { setState([this, i]() { if (hovered_idx == static_cast<int>(i)) hovered_idx = -1; }); }
                                })
                            );
                        }
                        return items;
                    }()
                })
            }
        });
    }
};

class IconDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "IconDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<IconDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Icon Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<IconDemo>(), config);
}
