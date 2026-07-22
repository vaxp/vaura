#pragma once
/// @file popup_window.hpp
/// @brief PopupWindow — a fully independent borderless OS window for popups.
///
/// Each PopupWindow owns:
///   - An SDL_Window (borderless, always-on-top, no taskbar entry)
///   - Its own SDL_GLContext (NOT shared with the main window)
///   - Its own GrDirectContext (Skia GPU context)
///   - Its own SkSurface
///   - Its own BuildOwner + Element tree
///
/// This is the correct, framework-quality approach used by Qt, GTK, and Win32
/// for popup menus. No shared context hacks, no overlay workarounds.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/core/result.hpp"
#include "vaura/tree/widget.hpp"
#include <memory>
#include <cstdint>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// PopupWindowConfig
// ════════════════════════════════════════════════════════════════

struct PopupWindowConfig {
    int   x           = 0;         ///< Screen X position (absolute, not relative to parent)
    int   y           = 0;         ///< Screen Y position
    int   width       = 200;       ///< Width in pixels
    int   height      = 300;       ///< Height in pixels
    Color clear_color = 0xFF1E293B; ///< Background clear color
};

// ════════════════════════════════════════════════════════════════
// PopupWindow
// ════════════════════════════════════════════════════════════════

/// @brief An independent borderless OS window suitable for popup menus and dropdowns.
///
/// Lifecycle:
///   1. Create via `PopupWindow::create()`.
///   2. Call `render()` each frame from within the main render loop.
///   3. Call `close()` when done; `isClosed()` returns true afterwards.
///   4. Destroy (unique_ptr going out of scope) cleans up all resources.
class PopupWindow {
public:
    ~PopupWindow();

    // Non-copyable
    PopupWindow(const PopupWindow&) = delete;
    PopupWindow& operator=(const PopupWindow&) = delete;

    /// Create a new fully independent popup window.
    /// Must be called from the main thread.
    static std::unique_ptr<PopupWindow> create(PopupWindowConfig config, WidgetPtr content);

    /// Render one frame: build → layout → paint → flush → swap.
    /// Must be called from the main thread. Internally calls makeCurrent.
    void render();

    /// Dispatch an SDL event to this popup's widget tree.
    /// @return true if the event was consumed.
    bool dispatchEvent(void* sdl_event);

    /// Programmatically close and mark this popup as done.
    void close();

    /// @return true if close() has been called or the window was dismissed.
    bool isClosed() const;

    /// @return the SDL windowID of this popup (used for event routing).
    uint32_t sdlWindowId() const;

    /// @return the SDL_Window* handle (opaque void* for callers).
    void* nativeHandle() const;

private:
    PopupWindow();
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace vaura
