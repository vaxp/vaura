/// @file split_view.cpp
/// @brief SplitView widget implementation.
///
/// Strategy:
///   - Tracks a `ratio` in [0,1] that is the proportion of the first pane.
///   - The divider is a thin GestureDetector strip.
///   - On pointer_move while dragging, we use the global pointer position
///     delivered via App to compute a delta and update the ratio.
///   - Uses Anu widths/heights to layout both panes.

#include "vaura/widgets/split_view.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/platform/platform.hpp"
#include <layout_engine/Anu.h>
#include <algorithm>

namespace vaura {

class SplitViewWidget : public StatefulWidget {
public:
    SplitViewConfig config;
    explicit SplitViewWidget(SplitViewConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "SplitView"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SplitViewState : public State {
public:
    float ratio_       = 0.5f;
    bool  dragging_    = false;
    bool  div_hovered_ = false;

    // Connected signal slots for global pointer tracking during drag
    SlotId mouse_move_slot_ = InvalidSlotId;
    SlotId mouse_up_slot_   = InvalidSlotId;
    Point  last_pos_        = {0, 0};

    void initState() override {
        State::initState();
        auto* sw = static_cast<const SplitViewWidget*>(widget());
        ratio_ = std::clamp(sw->config.initial_ratio, 0.0f, 1.0f);
    }

    void dispose() override {
        stopDragTracking();
        State::dispose();
    }

    // ── Global drag tracking ────────────────────────────────────

    void startDragTracking(const SplitViewConfig& cfg) {
        if (dragging_) return;
        dragging_ = true;
        setState([]{});

        auto* p = Platform::instance();
        if (!p) return;

        bool is_horiz = (cfg.axis == SplitViewConfig::Axis::Horizontal);

        mouse_move_slot_ = p->onMouseMove().connect([this, cfg, is_horiz](float x, float y) {
            if (!dragging_) return;
            Point cur{x, y};
            float delta = is_horiz ? (cur.x - last_pos_.x) : (cur.y - last_pos_.y);
            last_pos_ = cur;
            if (std::abs(delta) > 0.0f) {
                updateRatioDelta(delta, cfg);
            }
        });

        // We track mouse move by storing last position and computing delta
        mouse_up_slot_ = p->onMouseUp().connect([this](float, float, int btn) {
            if (btn == 1) stopDragTracking();
        });
    }

    void stopDragTracking() {
        if (!dragging_) return;
        dragging_ = false;
        if (auto* p = Platform::instance()) {
            p->onMouseMove().disconnect(mouse_move_slot_);
            p->onMouseUp().disconnect(mouse_up_slot_);
        }
        mouse_move_slot_ = InvalidSlotId;
        mouse_up_slot_   = InvalidSlotId;
        setState([]{});
    }

    void updateRatioDelta(float delta_px, const SplitViewConfig& cfg) {
        float total     = cfg.total_size - cfg.divider_thickness;
        float first_px  = ratio_ * total + delta_px;
        float second_px = total - first_px;

        // Clamp to minimums
        first_px  = std::max(first_px,  cfg.min_first_size);
        second_px = std::max(second_px, cfg.min_second_size);
        first_px  = total - second_px;  // re-derive after clamping second

        float clamped = first_px / total;
        clamped = std::clamp(clamped, 0.0f, 1.0f);
        setState([this, clamped]{ ratio_ = clamped; });
        if (cfg.on_resize) cfg.on_resize(clamped);
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sw = static_cast<const SplitViewWidget*>(widget());
        const auto& cfg = sw->config;

        bool is_horiz = (cfg.axis == SplitViewConfig::Axis::Horizontal);
        float total   = cfg.total_size;
        float div_t   = cfg.divider_thickness;
        float first_sz  = ratio_ * (total - div_t);
        float second_sz = (1.0f - ratio_) * (total - div_t);

        Color div_col = (div_hovered_ || dragging_) ? cfg.divider_hover : cfg.divider_color;

        // First pane
        auto first_wrap = std::make_shared<FlexBox>();
        if (is_horiz) first_wrap->width(first_sz).heightPercent(100);
        else          first_wrap->widthPercent(100).height(first_sz);
        if (cfg.first) first_wrap->child(cfg.first);

        // Divider — uses GLOBAL pointer delta for resize
        auto divider_box = std::make_shared<FlexBox>();
        if (is_horiz) divider_box->width(div_t).heightPercent(100).backgroundColor(div_col);
        else          divider_box->widthPercent(100).height(div_t).backgroundColor(div_col);

        auto divider_gd = gesture_detector({
            .child  = divider_box,
            .on_tap_down = [this, cfg]() {
                // We'll initialize last_pos_ properly on the first move, or we could grab it here if we had access to the current mouse pos.
                // However, the first delta might be large if we don't init it. The best way is to rely on on_pointer_move to seed it.
                startDragTracking(cfg);
            },
            .on_tap_up = [this]() {
                stopDragTracking();
            },
            .on_hover_enter = [this]() {
                setState([this]{ div_hovered_ = true; });
            },
            .on_hover_exit  = [this]() {
                // NOTE: Do NOT stop dragging here — mouse can leave divider while dragging
                setState([this]{ div_hovered_ = false; });
            },
            .on_pointer_move = [this, cfg, is_horiz](PointerEvent e) {
                if (!dragging_) {
                    // Seed last_pos_ when hovering (pre-drag)
                    last_pos_ = e.position;
                    return;
                }
                // Fallback for moves that arrive while still inside the divider hit zone
                last_pos_ = e.position;
            },
        });

        // Second pane
        auto second_wrap = std::make_shared<FlexBox>();
        if (is_horiz) second_wrap->width(second_sz).heightPercent(100);
        else          second_wrap->widthPercent(100).height(second_sz);
        if (cfg.second) second_wrap->child(cfg.second);

        // Root row / column
        auto root = std::make_shared<FlexBox>();
        root->direction(is_horiz ? FlexDirection::Row : FlexDirection::Column);
        root->widthPercent(100).heightPercent(100);
        root->child(first_wrap).child(divider_gd).child(second_wrap);
        return root;
    }
};

std::unique_ptr<State> SplitViewWidget::createState() {
    return std::make_unique<SplitViewState>();
}

std::shared_ptr<Widget> split_view(SplitViewConfig config) {
    return std::make_shared<SplitViewWidget>(std::move(config));
}

} // namespace vaura
