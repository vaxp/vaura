#pragma once
/// @file popup_window_controller.hpp
/// @brief PopupWindowController — manages the lifecycle of all active popup windows.
///
/// Popups (ContextMenu, Dropdown) open and close via this singleton.
/// The App's render loop calls `renderAll()` each frame to update them.
/// The Platform's event loop calls `dispatchEvent()` for every SDL event.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/platform/popup_window.hpp"
#include "vaura/tree/widget.hpp"
#include <memory>
#include <vector>
#include <cstdint>

namespace vaura {

using PopupId = uint32_t;

// ════════════════════════════════════════════════════════════════
// PopupWindowController
// ════════════════════════════════════════════════════════════════

/// @brief Global manager for native popup windows.
///
/// Thread safety: all methods must be called from the main thread.
class PopupWindowController {
public:
    /// Get the global singleton instance.
    static PopupWindowController& instance();

    // Non-copyable
    PopupWindowController(const PopupWindowController&) = delete;
    PopupWindowController& operator=(const PopupWindowController&) = delete;

    // ── Popup Management ─────────────────────────────────────────

    /// Open a new popup window. Returns its unique ID.
    PopupId open(PopupWindowConfig config, WidgetPtr content);

    /// Close a specific popup by ID.
    void close(PopupId id);

    /// Close all active popups immediately.
    void closeAll();

    /// @return true if there are any open popups.
    bool hasOpenPopups() const;

    // ── Per-frame integration ────────────────────────────────────

    /// Call once per frame from App::renderFrame() AFTER the main window is rendered.
    /// Renders all active popups, then removes any that have been closed.
    void renderAll();

    // ── Event routing ────────────────────────────────────────────

    /// Called from Platform::pollEvents() for every SDL event.
    /// Routes the event to the correct popup if it owns the target window.
    /// @return true if the event was consumed by a popup (caller should skip it).
    bool dispatchEvent(void* sdl_event);

private:
    PopupWindowController() = default;

    struct Entry {
        PopupId                       id;
        std::unique_ptr<PopupWindow>  window;
    };

    std::vector<Entry> popups_;
    PopupId            next_id_ = 1;
};

}  // namespace vaura
