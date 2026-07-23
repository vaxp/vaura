/// @file fab.cpp
/// @brief FloatingActionButton widget implementation.

#include "vaura/widgets/fab.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <layout_engine/Anu.h>

namespace vaura {

class FABWidget : public StatefulWidget {
public:
    FABConfig config;
    explicit FABWidget(FABConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "FAB"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class FABState : public State {
    bool is_hovered = false;
    bool is_pressed = false;
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto* fw = static_cast<const FABWidget*>(widget());
        const auto& cfg = fw->config;

        bool is_extended = !cfg.label.empty();
        bool is_mini     = (cfg.size == FABConfig::Size::Mini);

        float btn_size = is_mini ? 40.0f : 56.0f;
        float radius   = is_extended ? 16.0f : btn_size / 2.0f;

        Color bg = !cfg.enabled     ? 0xFF475569
                 : is_pressed       ? cfg.pressed_color
                 : is_hovered       ? cfg.hover_color
                 :                    cfg.background_color;

        // Content row
        auto content_row = std::make_shared<FlexBox>();
        content_row->direction(FlexDirection::Row)
                    .align(Align::Center)
                    .justify(Justify::Center);

        if (cfg.icon) {
            auto iw = std::make_shared<FlexBox>();
            iw->width(24).height(24)
               .justify(Justify::Center)
               .align(Align::Center)
               .child(cfg.icon);
            content_row->child(iw);
        }

        if (is_extended && !cfg.label.empty()) {
            auto lbl_wrap = std::make_shared<FlexBox>();
            lbl_wrap->margin(Edge::Left, cfg.icon ? 8.0f : 0.0f)
                     .child(text(cfg.label, {
                         .font_size = cfg.label_font_size,
                         .color     = cfg.label_color,
                         .weight    = TextStyle::SemiBold,
                     }));
            content_row->child(lbl_wrap);
        }

        // Button box
        auto btn_box = container({
            .color         = bg,
            .width         = is_extended ? ANUUndefined : btn_size,
            .height        = btn_size,
            .min_width     = btn_size,
            .padding       = is_extended
                             ? EdgeInsets{0, 20, 0, cfg.icon ? 16.0f : 20.0f}
                             : EdgeInsets{0, 0, 0, 0},
            .border_radius = BorderRadius::circular(radius),
            .shadow        = Shadow{0x50000000, cfg.elevation * 2.0f, {0, cfg.elevation * 0.5f}},
            .child         = content_row,
        });

        if (!cfg.enabled) return btn_box;

        return gesture_detector({
            .child          = btn_box,
            .on_tap         = cfg.on_pressed,
            .on_tap_down    = [this]() { setState([this]{ is_pressed = true; }); },
            .on_hover_enter = [this]() { setState([this]{ is_hovered = true; }); },
            .on_hover_exit  = [this]() { setState([this]{ is_hovered = false; is_pressed = false; }); },
        });
    }
};

std::unique_ptr<State> FABWidget::createState() {
    return std::make_unique<FABState>();
}

std::shared_ptr<Widget> fab(FABConfig config) {
    return std::make_shared<FABWidget>(std::move(config));
}

} // namespace vaura
