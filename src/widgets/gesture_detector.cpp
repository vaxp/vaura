/// @file gesture_detector.cpp
/// @brief GestureDetectorWidget implementation.

#include "vaura/widgets/gesture_detector.hpp"

namespace vaura {

extern RenderGestureDetector* g_app_last_hovered;

RenderGestureDetector::~RenderGestureDetector() {
    if (g_app_last_hovered == this) {
        g_app_last_hovered = nullptr;
    }
}

std::unique_ptr<RenderObject> GestureDetectorWidget::createRenderObject(BuildContext& ctx) {
    auto obj = std::make_unique<RenderGestureDetector>();
    updateRenderObject(ctx, *obj);
    return obj;
}

void GestureDetectorWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& gd = static_cast<RenderGestureDetector&>(renderObject);
    gd.on_tap         = config.on_tap;
    gd.on_tap_down    = config.on_tap_down;
    gd.on_tap_up      = config.on_tap_up;
    gd.on_hover_enter = config.on_hover_enter;
    gd.on_hover_exit  = config.on_hover_exit;
    gd.on_pointer_move = config.on_pointer_move;
    gd.on_scroll       = config.on_scroll;
    gd.on_right_click  = config.on_right_click;
    gd.markNeedsLayout();
}

} // namespace vaura
