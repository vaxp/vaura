#include "vaura/widgets/button.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/tree/build_context.hpp"

namespace vaura {

class ButtonState : public State {
public:
    bool is_hovered = false;
    bool is_pressed = false;

    WidgetPtr build(BuildContext& ctx) override {
        auto* btn = static_cast<const ButtonWidget*>(widget());
        
        Color bg_color = btn->config.color;
        if (!btn->config.enabled) {
            bg_color = btn->config.disabled_color != 0 ? btn->config.disabled_color : 0xFFCCCCCC;
        } else if (is_pressed) {
            bg_color = btn->config.pressed_color != 0 ? btn->config.pressed_color : 0xFF1976D2;
        } else if (is_hovered) {
            bg_color = btn->config.hover_color != 0 ? btn->config.hover_color : 0xFF42A5F5;
        }

        auto content = std::make_shared<FlexBox>();
        if (btn->config.width > 0) content->width(btn->config.width);
        if (btn->config.width_percent.has_value()) content->widthPercent(btn->config.width_percent.value());
        content->height(btn->config.min_height)
               .backgroundColor(bg_color)
               .borderRadius(btn->config.border_radius)
               .padding(YGEdgeLeft, btn->config.padding.left)
               .padding(YGEdgeRight, btn->config.padding.right)
               .padding(YGEdgeTop, btn->config.padding.top)
               .padding(YGEdgeBottom, btn->config.padding.bottom)
               .justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter);
               
        if (btn->config.child) {
            content->child(btn->config.child);
        }

        if (!btn->config.enabled) return content;

        return gesture_detector({
            .child = std::move(content),
            .on_tap = [this, btn]() {
                if (btn->config.on_pressed) btn->config.on_pressed();
            },
            .on_tap_down = [this]() {
                setState([this] { is_pressed = true; });
            },
            .on_hover_enter = [this]() {
                setState([this] { is_hovered = true; });
            },
            .on_hover_exit = [this]() {
                setState([this] {
                    is_hovered = false;
                    is_pressed = false;
                });
            }
        });
    }
};

std::unique_ptr<State> ButtonWidget::createState() {
    return std::make_unique<ButtonState>();
}

} // namespace vaura
