#include "vaura/widgets/tooltip.hpp"
#include "vaura/widgets/stack.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/ignore_pointer.hpp"
#include "vaura/state/state.hpp"

namespace vaura {

class TooltipWidget : public StatefulWidget {
public:
    TooltipConfig config;
    explicit TooltipWidget(TooltipConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Tooltip"; }
    std::unique_ptr<State> createState() override;
};

class TooltipState : public State {
    bool is_hovered = false;
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto w = static_cast<const TooltipWidget*>(widget());
        
        auto child = gesture_detector({
            .child = w->config.child,
            .on_hover_enter = [this]() {
                if (!is_hovered) {
                    setState([&](){ is_hovered = true; });
                }
            },
            .on_hover_exit = [this]() {
                if (is_hovered) {
                    setState([&](){ is_hovered = false; });
                }
            }
        });

        if (!is_hovered) {
            return child;
        }

        auto tooltip_box = container({
            .color = w->config.background_color,
            .padding = {8, 4, 8, 4},
            // Push tooltip down by vertical_offset
            .margin = {w->config.vertical_offset, 0, 0, 0}, 
            .border_radius = BorderRadius::circular(6.0f),
            .shadow = Shadow{
                .color = 0x40000000,
                .blur_radius = 8.0f,
                .offset = {0, 4}
            },
            .child = text(w->config.message, {
                .font_size = w->config.font_size,
                .color = w->config.text_color,
                .weight = TextStyle::SemiBold
            })
        });

        std::vector<WidgetPtr> stack_children;
        stack_children.push_back(child);
        stack_children.push_back(ignore_pointer(tooltip_box));

        return stack({
            .alignment = Alignment::TopLeft,
            .children = stack_children
        });
    }
};

std::unique_ptr<State> TooltipWidget::createState() {
    return std::make_unique<TooltipState>();
}

std::shared_ptr<Widget> tooltip(TooltipConfig config) {
    return std::make_shared<TooltipWidget>(std::move(config));
}

} // namespace vaura
