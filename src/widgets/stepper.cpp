/// @file stepper.cpp
/// @brief Stepper widget implementation.

#include "vaura/widgets/stepper.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <layout_engine/Yoga.h>
#include <string>
#include <algorithm>

namespace vaura {

// ── Step circle built from widget primitives ──────────────────
// (Avoids custom RenderBox / canvas API dependency)

static WidgetPtr makeStepCircle(float sz, Color fill, Color text_col,
                                 int number, bool is_check) {
    std::string lbl = is_check ? "✓" : std::to_string(number);
    auto circle = std::make_shared<FlexBox>();
    circle->width(sz).height(sz)
           .borderRadius(sz / 2.0f)
           .backgroundColor(fill)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .child(text(lbl, {
               .font_size = sz * 0.40f,
               .color     = text_col,
               .weight    = TextStyle::Bold,
               .align     = TextStyle::Center,
           }));
    return circle;
}

// ── Stepper (StatelessWidget — driven by external state) ───────

class StepperWidgetImpl : public StatelessWidget {
public:
    StepperConfig config;
    explicit StepperWidgetImpl(StepperConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Stepper"; }

    WidgetPtr build(BuildContext& ctx) override {
        const auto& cfg = config;
        int total = (int)cfg.steps.size();
        int cur   = std::clamp(cfg.current_step, 0, total - 1);

        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn).widthPercent(100);

        // ── Step header row ───────────────────────────────────
        auto hdr = std::make_shared<FlexBox>();
        hdr->flexDirection(YGFlexDirectionRow)
            .alignItems(YGAlignCenter)
            .widthPercent(100)
            .padding(YGEdgeHorizontal, 16)
            .padding(YGEdgeVertical, 12);

        for (int i = 0; i < total; ++i) {
            bool is_active    = (i == cur);
            bool is_completed = (i <  cur);

            Color circle_color = is_completed  ? cfg.completed_color
                               : is_active     ? cfg.active_color
                               :                 cfg.inactive_color;

            auto circle = makeStepCircle(cfg.circle_size, circle_color,
                                          0xFFFFFFFF, i + 1, is_completed);

            // Label column
            auto lbl_col = std::make_shared<FlexBox>();
            lbl_col->flexDirection(YGFlexDirectionColumn).margin(YGEdgeLeft, 8);
            lbl_col->child(text(cfg.steps[i].title, {
                .font_size = cfg.font_size,
                .color     = is_active ? cfg.text_color : cfg.subtitle_color,
                .weight    = is_active ? TextStyle::SemiBold : TextStyle::Regular,
            }));
            if (!cfg.steps[i].subtitle.empty()) {
                lbl_col->child(text(cfg.steps[i].subtitle, {
                    .font_size = cfg.font_size - 2.0f,
                    .color     = cfg.subtitle_color,
                }));
            }

            auto step_item = std::make_shared<FlexBox>();
            step_item->flexDirection(YGFlexDirectionRow).alignItems(YGAlignCenter);
            step_item->child(circle).child(lbl_col);

            if (!cfg.linear && cfg.on_step_changed) {
                int ci = i;
                auto fn = cfg.on_step_changed;
                hdr->child(gesture_detector({
                    .child  = step_item,
                    .on_tap = [fn, ci]() { fn(ci); },
                }));
            } else {
                hdr->child(step_item);
            }

            // Connector line
            if (i < total - 1) {
                auto line = std::make_shared<FlexBox>();
                line->flexGrow(1).height(2).margin(YGEdgeHorizontal, 8)
                     .backgroundColor(i < cur ? cfg.active_color : cfg.connector_color);
                hdr->child(line);
            }
        }
        root->child(hdr);

        // ── Active step content ───────────────────────────────
        if (cur < total && cfg.steps[cur].content) {
            auto content_box = container({
                .color         = cfg.background_color,
                .width_percent = 100.0f,
                .padding       = EdgeInsets::all(16),
                .border_radius = BorderRadius::circular(12),
                .child         = cfg.steps[cur].content,
            });
            auto cw = std::make_shared<FlexBox>();
            cw->widthPercent(100).margin(YGEdgeTop, 8).child(content_box);
            root->child(cw);
        }

        // ── Navigation buttons ────────────────────────────────
        auto btn_row = std::make_shared<FlexBox>();
        btn_row->flexDirection(YGFlexDirectionRow)
                .justifyContent(YGJustifySpaceBetween)
                .alignItems(YGAlignCenter)
                .widthPercent(100)
                .margin(YGEdgeTop, 16)
                .padding(YGEdgeHorizontal, 4);

        // Back
        if (cur > 0 && cfg.on_step_changed) {
            auto fn = cfg.on_step_changed;
            int prev = cur - 1;
            btn_row->child(gesture_detector({
                .child = container({
                    .color         = 0xFF334155,
                    .width         = cfg.button_width,
                    .height        = cfg.button_height,
                    .padding       = cfg.button_padding,
                    .border_radius = BorderRadius::circular(8),
                    .child         = text("Back", {.font_size = cfg.button_font_size, .color = 0xFFFFFFFF, .weight = TextStyle::SemiBold}),
                }),
                .on_tap = [fn, prev]() { fn(prev); },
            }));
        } else {
            auto sp = std::make_shared<FlexBox>(); sp->width(1);
            btn_row->child(sp);
        }

        // Next / Finish
        bool is_last = (cur == total - 1);
        if (is_last && cfg.on_complete) {
            btn_row->child(gesture_detector({
                .child = container({
                    .color         = cfg.completed_color,
                    .width         = cfg.button_width,
                    .height        = cfg.button_height,
                    .padding       = cfg.button_padding,
                    .border_radius = BorderRadius::circular(8),
                    .child         = text("Finish", {.font_size = cfg.button_font_size, .color = 0xFFFFFFFF, .weight = TextStyle::SemiBold}),
                }),
                .on_tap = cfg.on_complete,
            }));
        } else if (!is_last && cfg.on_step_changed) {
            auto fn = cfg.on_step_changed;
            int next = cur + 1;
            btn_row->child(gesture_detector({
                .child = container({
                    .color         = cfg.active_color,
                    .width         = cfg.button_width,
                    .height        = cfg.button_height,
                    .padding       = cfg.button_padding,
                    .border_radius = BorderRadius::circular(8),
                    .child         = text("Next", {.font_size = cfg.button_font_size, .color = 0xFFFFFFFF, .weight = TextStyle::SemiBold}),
                }),
                .on_tap = [fn, next]() { fn(next); },
            }));
        }

        root->child(btn_row);
        return root;
    }
};

std::shared_ptr<Widget> stepper(StepperConfig config) {
    return std::make_shared<StepperWidgetImpl>(std::move(config));
}

} // namespace vaura
