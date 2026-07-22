#pragma once
/// @file platform.hpp
/// @brief Platform abstraction interface.
/// Wraps SDL for window creation, event handling, and OpenGL context.

#include "vaura/core/types.hpp"
#include "vaura/core/result.hpp"
#include "vaura/core/signal.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace vaura {

/// Platform subsystem — manages SDL initialization and the event loop.
class Platform {
public:
    /// Create and initialize the platform subsystem.
    static Result<std::unique_ptr<Platform>> create();

    /// Get the active Platform instance (singleton).
    static Platform* instance();

    ~Platform();

    // Non-copyable
    Platform(const Platform&) = delete;
    Platform& operator=(const Platform&) = delete;

    /// Poll and dispatch all pending events.
    /// @return false if the application should quit.
    bool pollEvents();

    /// Get high-resolution time in seconds since initialization.
    [[nodiscard]] double getTime() const;

    /// Get the clipboard text (UTF-8).
    [[nodiscard]] std::string getClipboardText() const;

    /// Set the clipboard text (UTF-8).
    void setClipboardText(std::string_view text);

    /// Signal emitted when the platform requests quit.
    Signal<>& onQuit() { return on_quit_; }

    /// Signal emitted on mouse button press. Args: x, y, button (1=left).
    Signal<float, float, int>& onMouseDown() { return on_mouse_down_; }

    /// Signal emitted on mouse button release. Args: x, y, button.
    Signal<float, float, int>& onMouseUp() { return on_mouse_up_; }

    /// Signal emitted on mouse move. Args: x, y.
    Signal<float, float>& onMouseMove() { return on_mouse_move_; }

    /// Signal emitted on mouse scroll. Args: xoffset, yoffset.
    Signal<float, float>& onScroll() { return on_scroll_; }

    /// Signal emitted on text input (UTF-8).
    Signal<std::string_view>& onTextInput() { return on_text_input_; }

    /// Signal emitted on key down. Args: keycode (e.g. SDLK_BACKSPACE).
    Signal<int>& onKeyDown() { return on_key_down_; }

    /// Enable text input events (shows OS keyboard on mobile).
    void startTextInput();

    /// Disable text input events.
    void stopTextInput();

    /// Access the underlying implementation (for Window creation).
    struct Impl;
    Impl* impl() { return impl_.get(); }

private:
    Platform();
    std::unique_ptr<Impl> impl_;
    Signal<> on_quit_;
    Signal<float, float, int> on_mouse_down_;
    Signal<float, float, int> on_mouse_up_;
    Signal<float, float>      on_mouse_move_;
    Signal<float, float>      on_scroll_;
    Signal<std::string_view>  on_text_input_;
    Signal<int>               on_key_down_;
};

}  // namespace vaura
