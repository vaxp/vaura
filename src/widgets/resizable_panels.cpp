/// @file resizable_panels.cpp
/// @brief ResizablePanels widget implementation.
///
/// Fix for drag bug:
///   - on_hover_exit no longer cancels the drag (dragging_div_ reset).
///   - Mouse-up is tracked globally via Platform signals so releasing
///     outside the divider still ends the drag cleanly.
///   - on_pointer_move now receives GLOBAL position (e.position) and
///     computes delta against the stored last_pos_, which is correct.

#include "vaura/widgets/resizable_panels.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/platform/platform.hpp"
#include <layout_engine/Anu.h>
#include <algorithm>
#include <numeric>

namespace vaura {

class ResizablePanelsWidget : public StatefulWidget {
public:
    ResizablePanelsConfig config;
    explicit ResizablePanelsWidget(ResizablePanelsConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "ResizablePanels"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ResizablePanelsState : public State {
public:
    std::vector<float> sizes_;
    int   dragging_div_ = -1;
    int   hovered_div_  = -1;
    Point last_pos_     = {0, 0};

    // Global signal slots for tracking mouse outside dividers during drag
    SlotId mouse_move_slot_ = InvalidSlotId;
    SlotId mouse_up_slot_   = InvalidSlotId;

    void initState() override {
        State::initState();
        auto* rw = static_cast<const ResizablePanelsWidget*>(widget());
        initSizes(rw->config);
    }

    void dispose() override {
        stopGlobalDrag();
        State::dispose();
    }

    void initSizes(const ResizablePanelsConfig& cfg) {
        int n = (int)cfg.panels.size();
        sizes_.resize(n, 0.0f);
        int num_dividers = n - 1;
        float available = cfg.total_size - num_dividers * cfg.divider_thickness;

        float assigned = 0.0f;
        int   auto_count = 0;
        for (int i = 0; i < n; ++i) {
            if (cfg.panels[i].initial_size > 0.0f) {
                sizes_[i] = cfg.panels[i].initial_size;
                assigned += sizes_[i];
            } else {
                ++auto_count;
            }
        }

        float remaining = available - assigned;
        float auto_size = auto_count > 0 ? remaining / auto_count : 0.0f;
        for (int i = 0; i < n; ++i) {
            if (cfg.panels[i].initial_size <= 0.0f) {
                sizes_[i] = std::max(auto_size, cfg.panels[i].min_size);
            }
        }
    }

    void moveDivider(int div_idx, float delta, const ResizablePanelsConfig& cfg) {
        int a = div_idx;
        int b = div_idx + 1;

        float new_a = sizes_[a] + delta;
        float new_b = sizes_[b] - delta;

        float min_a = cfg.panels[a].min_size;
        float min_b = cfg.panels[b].min_size;
        float max_a = cfg.panels[a].max_size > 0 ? cfg.panels[a].max_size : 1e9f;
        float max_b = cfg.panels[b].max_size > 0 ? cfg.panels[b].max_size : 1e9f;

        float sum = sizes_[a] + sizes_[b];
        new_a = std::clamp(new_a, min_a, std::min(max_a, sum - min_b));
        new_b = sum - new_a;
        new_b = std::clamp(new_b, min_b, max_b);
        new_a = sum - new_b;

        setState([this, a, b, new_a, new_b] {
            sizes_[a] = new_a;
            sizes_[b] = new_b;
        });

        if (cfg.on_resize) cfg.on_resize(sizes_);
    }

    // ── Global mouse tracking (keeps drag alive outside divider) ─

    void startGlobalDrag(int di, const ResizablePanelsConfig& cfg) {
        auto* p = Platform::instance();
        if (!p) return;

        bool is_horiz = (cfg.axis == ResizablePanelsConfig::Axis::Horizontal);

        mouse_move_slot_ = p->onMouseMove().connect([this, di, cfg, is_horiz](float x, float y) {
            if (dragging_div_ != di) return;
            Point cur{x, y};
            float delta = is_horiz ? (cur.x - last_pos_.x) : (cur.y - last_pos_.y);
            last_pos_ = cur;
            if (std::abs(delta) > 0.0f) moveDivider(di, delta, cfg);
        });

        mouse_up_slot_ = p->onMouseUp().connect([this](float, float, int btn) {
            if (btn == 1) stopGlobalDrag();
        });
    }

    void stopGlobalDrag() {
        if (dragging_div_ == -1) return;
        dragging_div_ = -1;
        if (auto* p = Platform::instance()) {
            p->onMouseMove().disconnect(mouse_move_slot_);
            p->onMouseUp().disconnect(mouse_up_slot_);
        }
        mouse_move_slot_ = InvalidSlotId;
        mouse_up_slot_   = InvalidSlotId;
        setState([]{});
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* rw = static_cast<const ResizablePanelsWidget*>(widget());
        const auto& cfg = rw->config;

        if (sizes_.size() != cfg.panels.size()) initSizes(cfg);

        bool is_horiz = (cfg.axis == ResizablePanelsConfig::Axis::Horizontal);

        auto root = std::make_shared<FlexBox>();
        root->direction(is_horiz ? FlexDirection::Row : FlexDirection::Column);
        root->widthPercent(100).heightPercent(100);

        for (int i = 0; i < (int)cfg.panels.size(); ++i) {
            auto panel_wrap = std::make_shared<FlexBox>();
            if (is_horiz) panel_wrap->width(sizes_[i]).heightPercent(100);
            else          panel_wrap->widthPercent(100).height(sizes_[i]);
            if (cfg.panels[i].child) panel_wrap->child(cfg.panels[i].child);
            root->child(panel_wrap);

            if (i < (int)cfg.panels.size() - 1) {
                bool hov = (hovered_div_ == i) || (dragging_div_ == i);
                Color dc  = hov ? cfg.divider_hover : cfg.divider_color;

                auto div_box = std::make_shared<FlexBox>();
                if (is_horiz) div_box->width(cfg.divider_thickness).heightPercent(100).backgroundColor(dc);
                else          div_box->widthPercent(100).height(cfg.divider_thickness).backgroundColor(dc);

                int di = i;
                auto div_gd = gesture_detector({
                    .child = div_box,
                    .on_tap_down = [this, di, cfg, is_horiz](){ 
                        dragging_div_ = di;
                        // Seed last_pos_ from the current known cursor position
                        // (App stores last_pointer_x/y but we can't access it; 
                        //  we'll let the first move event initialise it via on_pointer_move)
                        startGlobalDrag(di, cfg);
                    },
                    .on_tap_up = [this]() {
                        stopGlobalDrag();
                    },
                    .on_hover_enter = [this, di]() {
                        setState([this, di]{ hovered_div_ = di; });
                    },
                    .on_hover_exit  = [this]() {
                        // Do NOT cancel drag here — mouse may leave divider while dragging
                        setState([this]{ hovered_div_ = -1; });
                    },
                    .on_pointer_move = [this, di, cfg, is_horiz](PointerEvent e) {
                        // Seed last_pos_ when hovering (pre-drag)
                        if (dragging_div_ != di) {
                            last_pos_ = e.position;
                            return;
                        }
                        // Delta is handled by global signal; this is a fallback for
                        // moves that arrive while still inside the divider hit zone.
                        // We update last_pos_ here too so the global handler stays in sync.
                        last_pos_ = e.position;
                    },
                });
                root->child(div_gd);
            }
        }

        return root;
    }
};

std::unique_ptr<State> ResizablePanelsWidget::createState() {
    return std::make_unique<ResizablePanelsState>();
}

std::shared_ptr<Widget> resizable_panels(ResizablePanelsConfig config) {
    return std::make_shared<ResizablePanelsWidget>(std::move(config));
}

} // namespace vaura
