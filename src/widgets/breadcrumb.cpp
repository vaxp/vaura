/// @file breadcrumb.cpp
/// @brief Breadcrumb widget implementation.

#include "vaura/widgets/breadcrumb.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

class BreadcrumbWidget : public StatefulWidget {
public:
    BreadcrumbConfig config;
    explicit BreadcrumbWidget(BreadcrumbConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Breadcrumb"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class BreadcrumbState : public State {
    int hovered_idx = -1;
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto* bw = static_cast<const BreadcrumbWidget*>(widget());
        const auto& cfg = bw->config;

        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
            .alignItems(YGAlignCenter)
            .flexWrap(YGWrapWrap);

        for (int i = 0; i < (int)cfg.items.size(); ++i) {
            const auto& item = cfg.items[i];
            bool is_last    = (i == (int)cfg.items.size() - 1);
            bool is_hovered = (hovered_idx == i);
            bool clickable  = (item.on_tap != nullptr);

            Color text_col = is_last       ? cfg.active_color
                           : is_hovered    ? cfg.hover_color
                           :                 cfg.inactive_color;

            auto lbl = text(item.label, {
                .font_size  = cfg.font_size,
                .color      = text_col,
                .underline  = (clickable && !is_last),
            });

            WidgetPtr crumb_widget = lbl;

            if (clickable) {
                int ci = i;
                auto fn = item.on_tap;
                crumb_widget = gesture_detector({
                    .child          = lbl,
                    .on_tap         = fn,
                    .on_hover_enter = [this, ci]() { setState([this, ci]{ hovered_idx = ci; }); },
                    .on_hover_exit  = [this]()     { setState([this]{ hovered_idx = -1; }); },
                });
            }

            // Leading icon
            if (item.icon) {
                auto icon_row = std::make_shared<FlexBox>();
                icon_row->flexDirection(YGFlexDirectionRow)
                          .alignItems(YGAlignCenter);
                auto iw = std::make_shared<FlexBox>();
                iw->margin(YGEdgeRight, 4).width(16).height(16)
                   .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter)
                   .child(item.icon);
                icon_row->child(iw).child(crumb_widget);
                crumb_widget = icon_row;
            }

            row->child(crumb_widget);

            // Separator (not after last)
            if (!is_last) {
                auto sep = std::make_shared<FlexBox>();
                sep->margin(YGEdgeHorizontal, 6)
                    .child(text(cfg.separator, {
                        .font_size = cfg.font_size,
                        .color     = cfg.separator_color,
                    }));
                row->child(sep);
            }
        }

        return row;
    }
};

std::unique_ptr<State> BreadcrumbWidget::createState() {
    return std::make_unique<BreadcrumbState>();
}

std::shared_ptr<Widget> breadcrumb(BreadcrumbConfig config) {
    return std::make_shared<BreadcrumbWidget>(std::move(config));
}

} // namespace vaura
