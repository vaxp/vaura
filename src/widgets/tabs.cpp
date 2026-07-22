#include "vaura/widgets/tabs.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/state/state.hpp"

namespace vaura {

class TabsWidget : public StatefulWidget {
public:
    TabsConfig config;

    explicit TabsWidget(TabsConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "Tabs"; }
    std::unique_ptr<State> createState() override;
};

class TabsState : public State {
    int active_index = 0;

public:
    void initState() override {
        State::initState();
        auto w = static_cast<const TabsWidget*>(widget());
        active_index = w->config.initial_index;
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto w = static_cast<const TabsWidget*>(widget());
        const auto& config = w->config;

        // Make sure index is valid
        if (active_index < 0 || active_index >= (int)config.tabs.size()) {
            active_index = 0;
        }

        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .widthPercent(100)
            .heightPercent(100);

        // --- Tab Bar ---
        auto tab_bar_row = std::make_shared<FlexBox>();
        tab_bar_row->flexDirection(YGFlexDirectionRow)
                   .alignItems(YGAlignCenter)
                   .widthPercent(100);

        for (int i = 0; i < (int)config.tabs.size(); ++i) {
            bool is_active = (i == active_index);
            
            auto tab_text = text(config.tabs[i].first, {
                .font_size = 16,
                .color = is_active ? config.active_text_color : config.inactive_text_color,
                .weight = is_active ? TextStyle::Bold : TextStyle::Regular,
                .max_lines = 1
            });

            auto tab_container = container({
                .color = is_active ? config.active_tab_color : config.inactive_tab_color,
                .padding = {16, 10, 16, 10},
                .border_radius = BorderRadius::circular(8.0f),
                .child = tab_text
            });

            GestureDetectorConfig gd_config;
            gd_config.child = tab_container;
            gd_config.on_tap = [this, i]() {
                setState([this, i] { active_index = i; });
            };

            auto clickable_tab = gesture_detector(std::move(gd_config));
            
            auto tab_wrapper = std::make_shared<FlexBox>();
            tab_wrapper->margin(YGEdgeRight, 8)
                       .flexShrink(0.0f)
                       .child(clickable_tab);

            tab_bar_row->child(tab_wrapper);
        }

        auto tab_bar_bg = container({
            .color = config.tab_bar_color,
            .padding = {8, 8, 8, 8},
            .border_radius = BorderRadius::circular(12.0f),
            .child = tab_bar_row
        });

        auto header_wrapper = std::make_shared<FlexBox>();
        header_wrapper->margin(YGEdgeBottom, 20).child(tab_bar_bg);
        
        root->child(header_wrapper);

        // --- Tab Content ---
        if (!config.tabs.empty()) {
            auto content_wrapper = std::make_shared<FlexBox>();
            content_wrapper->flexGrow(1.0f).widthPercent(100);
            content_wrapper->child(config.tabs[active_index].second);
            root->child(content_wrapper);
        }

        return root;
    }
};

std::unique_ptr<State> TabsWidget::createState() {
    return std::make_unique<TabsState>();
}

std::shared_ptr<Widget> tabs(TabsConfig config) {
    return std::make_shared<TabsWidget>(std::move(config));
}

} // namespace vaura
