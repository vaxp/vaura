#pragma once
/// @file window.hpp
/// @brief Window management.

#include "vaura/core/types.hpp"
#include "vaura/core/result.hpp"
#include "vaura/core/signal.hpp"
#include "vaura/platform/platform.hpp"
#include <memory>
#include <string>
#include <string_view>

namespace vaura {

/// Configuration for window creation.
struct WindowConfig {
    std::string title     = "VAURA App";
    int         width     = 800;
    int         height    = 600;
    bool        resizable = true;
    bool        fullscreen = false;
    int         min_width  = 320;
    int         min_height = 240;
    bool        vsync      = true;
};

/// Represents an OS window with an OpenGL context.
class Window {
public:
    /// Create a new window.
    static Result<std::unique_ptr<Window>> create(Platform& platform, WindowConfig config);

    ~Window();

    // Non-copyable
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    /// Set the window title.
    void setTitle(std::string_view title);

    /// Get the window size in pixels.
    [[nodiscard]] Size getSize() const;

    /// Get the drawable size (may differ from window size on HiDPI).
    [[nodiscard]] Size getDrawableSize() const;

    /// Get the DPI scale factor.
    [[nodiscard]] float getDpiScale() const;

    /// Make the OpenGL context of this window current.
    void makeCurrent();

    /// Swap front/back buffers.
    void swapBuffers();

    /// Get the native window handle (SDL_Window*).
    void* getNativeHandle() const;

    /// Get the OpenGL context handle.
    void* getGLContext() const;

    // --- Signals ---
    Signal<int, int>& onResize() { return on_resize_; }
    Signal<>&         onClose()  { return on_close_; }

    /// Internal — access the impl.
    struct Impl;
    Impl* impl() { return impl_.get(); }

private:
    Window();
    std::unique_ptr<Impl> impl_;
    Signal<int, int> on_resize_;
    Signal<>         on_close_;
};

}  // namespace vaura
