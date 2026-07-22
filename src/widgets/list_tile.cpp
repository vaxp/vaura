#include "vaura/widgets/list_tile.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/state/state.hpp"

namespace vaura {

class ListTileState : public State {
    bool is_hovered = false;
    bool is_pressed = false;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto w = static_cast<const StatefulWidget*>(widget());
        // Since we don't have access to the original config directly from Widget,
        // we should actually pass config via a custom StatefulWidget subclass.
        return nullptr; // We'll implement a custom stateful widget below
    }
};

class ListTileWidget : public StatefulWidget {
public:
    ListTileConfig config;

    explicit ListTileWidget(ListTileConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "ListTile"; }

    std::unique_ptr<State> createState() override;
};

class ListTileStateImpl : public State {
    bool is_hovered = false;
    bool is_pressed = false;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto tile = static_cast<const ListTileWidget*>(widget());
        const auto& config = tile->config;

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .alignItems(YGAlignCenter)
           .widthPercent(100);

        if (config.leading) {
            auto leading_box = std::make_shared<FlexBox>();
            leading_box->margin(YGEdgeRight, 16).child(config.leading);
            row->child(leading_box);
        }

        auto middle_col = std::make_shared<FlexBox>();
        middle_col->flexDirection(YGFlexDirectionColumn)
                  .justifyContent(YGJustifyCenter)
                  .flexGrow(1.0f);

        if (config.title) {
            middle_col->child(config.title);
        }
        if (config.subtitle) {
            auto sub_box = std::make_shared<FlexBox>();
            sub_box->margin(YGEdgeTop, 4).child(config.subtitle);
            middle_col->child(sub_box);
        }
        row->child(middle_col);

        if (config.trailing) {
            auto trailing_box = std::make_shared<FlexBox>();
            trailing_box->margin(YGEdgeLeft, 16).child(config.trailing);
            row->child(trailing_box);
        }

        Color bg_color = 0x00000000;
        if (config.on_tap) {
            if (is_pressed) bg_color = config.pressed_color;
            else if (is_hovered) bg_color = config.hover_color;
        }

        auto inner_container = container({
            .color = bg_color,
            .padding = config.content_padding,
            .child = row
        });

        if (config.on_tap) {
            GestureDetectorConfig gd_config;
            gd_config.on_tap = config.on_tap;
            gd_config.on_hover_enter = [this]() {
                setState([this] { is_hovered = true; });
            };
            gd_config.on_hover_exit = [this]() {
                setState([this] { is_hovered = false; is_pressed = false; });
            };
            gd_config.on_tap_down = [this]() {
                setState([this] { is_pressed = true; });
            };
            gd_config.on_tap_up = [this]() {
                setState([this] { is_pressed = false; });
            };
            gd_config.on_tap = config.on_tap;
            gd_config.child = inner_container;

            return gesture_detector(std::move(gd_config));
        }

        return inner_container;
    }
};

std::unique_ptr<State> ListTileWidget::createState() {
    return std::make_unique<ListTileStateImpl>();
}

std::shared_ptr<Widget> list_tile(ListTileConfig config) {
    return std::make_shared<ListTileWidget>(std::move(config));
}

std::shared_ptr<Widget> list_tile(std::string title_text, std::string subtitle_text, WidgetPtr leading, WidgetPtr trailing, std::function<void()> on_tap) {
    ListTileConfig config;
    config.title = text(title_text, {.font_size = 16, .color = 0xFFE2E8F0});
    if (!subtitle_text.empty()) {
        config.subtitle = text(subtitle_text, {.font_size = 14, .color = 0xFF94A3B8});
    }
    config.leading = std::move(leading);
    config.trailing = std::move(trailing);
    config.on_tap = std::move(on_tap);
    return list_tile(std::move(config));
}

} // namespace vaura
