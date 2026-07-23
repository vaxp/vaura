/// @file tooltip.cpp
/// @brief Tooltip widget — uses OverlayController for true overlay rendering.
///
/// On hover: inserts a tooltip overlay at the child's global position.
/// On exit:  removes the overlay.
/// The tooltip floats above ALL widgets via the OverlayStack.

#include "vaura/widgets/tooltip.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/ignore_pointer.hpp"
#include "vaura/overlay/overlay.hpp"
#include "vaura/animation/ticker.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/tree/element.hpp"
#include "vaura/tree/render_object.hpp"
#include <layout_engine/Anu.h>

namespace vaura {

class TooltipWidget : public StatefulWidget {
public:
    TooltipConfig config;
    explicit TooltipWidget(TooltipConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Tooltip"; }
    std::unique_ptr<State> createState() override;
};

class TooltipState : public State {
    bool       is_hovered_  = false;
    OverlayId  overlay_id_  = 0;
    bool       has_overlay_ = false;
    size_t     pending_cb_  = 0;
    bool       has_pending_ = false;

public:
    void dispose() override {
        cancelPending();
        removeOverlay();
        State::dispose();
    }

    void cancelPending() {
        if (has_pending_) {
            SchedulerBinding::instance().removeFrameCallback(pending_cb_);
            has_pending_ = false;
            pending_cb_ = 0;
        }
    }

    void removeOverlay() {
        if (has_overlay_) {
            OverlayController::instance().remove(overlay_id_);
            has_overlay_ = false;
            overlay_id_  = 0;
        }
    }

    void showOverlay(const TooltipConfig& cfg) {
        removeOverlay();

        // Read anchor global position from the render object
        float ax = 0.0f, ay = 0.0f, aw = 0.0f, ah = 0.0f;
        if (auto* el = element()) {
            if (auto* ro = el->findRenderObject()) {
                Rect gb = ro->globalBounds();
                ax = gb.x;
                ay = gb.y;
                aw = gb.width;
                ah = gb.height;
            }
        }

        // Build the tooltip bubble
        auto bubble = container({
            .color         = cfg.background_color,
            .padding       = {6, 10, 6, 10},
            .border_radius = BorderRadius::circular(6.0f),
            .shadow        = Shadow{
                .color       = 0x40000000,
                .blur_radius = 8.0f,
                .offset      = {0, 2}
            },
            .child = text(cfg.message, {
                .font_size = cfg.font_size,
                .color     = cfg.text_color,
                .weight    = TextStyle::SemiBold,
            }),
        });

        // Wrap in ignore_pointer so tooltip doesn't steal hover
        auto no_hit = ignore_pointer(bubble);

        // Position absolutely at the anchor's location + offset below
        auto positioned = std::make_shared<FlexBox>();
        positioned->positionType(PositionType::Absolute)
                   .position(Edge::Left, ax)
                   .position(Edge::Top, ay + ah + cfg.vertical_offset);
        positioned->child(no_hit);

        // Insert into OverlayController — renders above everything
        overlay_id_ = OverlayController::instance().insert({
            .content         = positioned,
            .absorbs_pointer = false,  // Don't block clicks/hover below
        });
        has_overlay_ = true;
    }

    /// Schedule overlay show for the NEXT frame, so the current
    /// hover dispatch cycle completes without being invalidated.
    void scheduleShow(const TooltipConfig& cfg) {
        cancelPending();
        TooltipConfig cfg_copy = cfg;
        pending_cb_ = SchedulerBinding::instance().addFrameCallback(
            [this, cfg_copy]() {
                // One-shot: remove ourselves immediately
                SchedulerBinding::instance().removeFrameCallback(pending_cb_);
                has_pending_ = false;
                pending_cb_ = 0;
                if (is_hovered_) {
                    showOverlay(cfg_copy);
                }
            });
        has_pending_ = true;
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* tw = static_cast<const TooltipWidget*>(widget());
        const auto& cfg = tw->config;

        return gesture_detector({
            .child          = cfg.child,
            .on_hover_enter = [this, cfg]() {
                if (!is_hovered_) {
                    is_hovered_ = true;
                    scheduleShow(cfg);
                }
            },
            .on_hover_exit  = [this]() {
                if (is_hovered_) {
                    is_hovered_ = false;
                    cancelPending();
                    removeOverlay();
                }
            },
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
