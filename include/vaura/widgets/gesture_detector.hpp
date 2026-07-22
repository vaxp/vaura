#pragma once
/// @file gesture_detector.hpp
/// @brief GestureDetector widget — wraps a child and fires tap/hover callbacks.
///
/// This is the bridge between pointer events (from the platform) and
/// widget logic. It creates a RenderGestureDetector that:
///   - Participates normally in hit testing
///   - Stores callbacks (onTap, onHoverEnter, onHoverExit)
///   - Is invoked by the App event dispatcher when a hit is found
///
/// Usage:
///   gesture_detector({
///       .on_tap        = []() { /* clicked */ },
///       .on_hover_enter = []() { /* mouse entered */ },
///       .on_hover_exit  = []() { /* mouse left */ },
///       .child         = text("Click me"),
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/platform/input.hpp"
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// PointerEvent — passed to event handlers
// ════════════════════════════════════════════════════════════════


// ════════════════════════════════════════════════════════════════
// RenderGestureDetector — render object with pointer event hooks
// ════════════════════════════════════════════════════════════════

/// @brief A render object that participates in pointer event dispatch.
///
/// The App event dispatcher finds this render object via hitTest and
/// calls handlePointerDown / handlePointerUp / handlePointerMove directly.
class RenderGestureDetector : public RenderBox {
public:
    std::function<void()>                  on_tap;
    std::function<void()>                  on_tap_down;
    std::function<void()>                  on_tap_up;
    std::function<void()>                  on_hover_enter;
    std::function<void()>                  on_hover_exit;
    std::function<void(PointerEvent)>      on_pointer_move;
    std::function<void(float, float)>      on_scroll;
    std::function<void(Point)>             on_right_click;  ///< Right-click / context menu trigger.

    bool is_hovered = false;
    bool is_pressed = false;

    // ── Hit Testing ────────────────────────────────────────────

    bool hitTestSelf(Point localPoint) const override {
        // Defer to children: only trigger if a child was hit.
        return false;
    }

    // ── Pointer Events ─────────────────────────────────────────

    bool handlesScroll() const override { return on_scroll != nullptr; }

    void handlePointerScroll(float dx, float dy) override {
        if (on_scroll) {
            on_scroll(dx, dy);
        }
    }

    // ── Pointer event handlers (called by App dispatcher) ──────

    void handlePointerDown(const PointerEvent& e) {
        if (e.button == MouseButton::Right) {
            if (on_right_click) on_right_click(e.position);
            return;
        }
        is_pressed = true;
        if (on_tap_down) on_tap_down();
        markNeedsPaint();
    }

    void handlePointerUp(const PointerEvent& e) {
        if (is_pressed) {
            is_pressed = false;
            if (on_tap_up) on_tap_up();
            if (on_tap) on_tap();
        }
        markNeedsPaint();
    }

    void handlePointerEnter(const PointerEvent& e) {
        if (!is_hovered) {
            is_hovered = true;
            if (on_hover_enter) on_hover_enter();
            markNeedsPaint();
        }
    }

    void handlePointerExit(const PointerEvent& e) {
        if (is_hovered) {
            is_hovered = false;
            is_pressed = false;
            if (on_hover_exit) on_hover_exit();
            markNeedsPaint();
        }
    }

    void handlePointerMove(const PointerEvent& e) {
        if (on_pointer_move) on_pointer_move(e);
    }

    // ── Painting (delegates to child) ─────────────────────────

    void paint(PaintContext& context) override {
        for (auto* child : children_) {
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }
    }
};

// ════════════════════════════════════════════════════════════════
// GestureDetectorConfig
// ════════════════════════════════════════════════════════════════

struct GestureDetectorConfig {
    WidgetPtr child;

    std::function<void()>              on_tap;
    std::function<void()>              on_tap_down;
    std::function<void()>              on_tap_up;
    std::function<void()>              on_hover_enter;
    std::function<void()>              on_hover_exit;
    std::function<void(PointerEvent)>  on_pointer_move;
    std::function<void(float, float)>  on_scroll;
    std::function<void(Point)>         on_right_click;  ///< Right-click / context menu trigger.
};

// ════════════════════════════════════════════════════════════════
// GestureDetectorWidget
// ════════════════════════════════════════════════════════════════

class GestureDetectorWidget : public SingleChildRenderObjectWidget {
public:
    GestureDetectorConfig config;

    explicit GestureDetectorWidget(GestureDetectorConfig cfg)
        : config(std::move(cfg)) {
        this->child = config.child;
    }

    [[nodiscard]] std::string_view typeName() const override {
        return "GestureDetector";
    }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override;
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

inline WidgetPtr gesture_detector(GestureDetectorConfig config) {
    return std::make_shared<GestureDetectorWidget>(std::move(config));
}

} // namespace vaura
