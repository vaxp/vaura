/// @file window.cpp
/// @brief Window management implementation using SDL2.

#include "vaura/platform/window.hpp"

#ifdef VAURA_HAS_SDL3
#include <SDL3/SDL.h>
#elif defined(VAURA_HAS_SDL2)
#include <SDL2/SDL.h>
#else
#define SDL_STUB_MODE
#endif

namespace vaura {

// ============================================================
// Window::Impl
// ============================================================
struct Window::Impl {
    SDL_Window*   sdl_window  = nullptr;
    SDL_GLContext  gl_context  = nullptr;
    WindowConfig   config;
};

// ============================================================
// Window
// ============================================================

Window::Window() : impl_(std::make_unique<Impl>()) {}

Window::~Window() {
    if (impl_) {
        if (impl_->gl_context) {
            SDL_GL_DeleteContext(impl_->gl_context);
            impl_->gl_context = nullptr;
        }
        if (impl_->sdl_window) {
            SDL_DestroyWindow(impl_->sdl_window);
            impl_->sdl_window = nullptr;
        }
    }
}

Result<std::unique_ptr<Window>> Window::create(Platform& platform, WindowConfig config) {
    (void)platform;  // Platform must be initialized before creating windows

    auto window = std::unique_ptr<Window>(new Window());
    window->impl_->config = config;

    // Build window flags
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;
    if (config.resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    if (config.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    // Create SDL window
    window->impl_->sdl_window = SDL_CreateWindow(
        config.title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        config.width,
        config.height,
        flags
    );

    if (!window->impl_->sdl_window) {
        return Result<std::unique_ptr<Window>>::err(
            ErrorCode::WindowError,
            std::string("SDL_CreateWindow failed: ") + SDL_GetError()
        );
    }

    // Set minimum size
    SDL_SetWindowMinimumSize(window->impl_->sdl_window, config.min_width, config.min_height);

    // Create OpenGL context
    window->impl_->gl_context = SDL_GL_CreateContext(window->impl_->sdl_window);
    if (!window->impl_->gl_context) {
        SDL_DestroyWindow(window->impl_->sdl_window);
        return Result<std::unique_ptr<Window>>::err(
            ErrorCode::RenderingError,
            std::string("SDL_GL_CreateContext failed: ") + SDL_GetError()
        );
    }

    // Enable VSync
    if (config.vsync) {
        SDL_GL_SetSwapInterval(1);
    } else {
        SDL_GL_SetSwapInterval(0);
    }

    return Result<std::unique_ptr<Window>>::ok(std::move(window));
}

void Window::setTitle(std::string_view title) {
    SDL_SetWindowTitle(impl_->sdl_window, std::string(title).c_str());
}

Size Window::getSize() const {
    int w, h;
    SDL_GetWindowSize(impl_->sdl_window, &w, &h);
    return {static_cast<float>(w), static_cast<float>(h)};
}

Size Window::getDrawableSize() const {
    int w, h;
    SDL_GL_GetDrawableSize(impl_->sdl_window, &w, &h);
    return {static_cast<float>(w), static_cast<float>(h)};
}

float Window::getDpiScale() const {
    Size win_size = getSize();
    Size draw_size = getDrawableSize();
    if (win_size.width > 0) {
        return draw_size.width / win_size.width;
    }
    return 1.0f;
}

void Window::makeCurrent() {
    SDL_GL_MakeCurrent(impl_->sdl_window, impl_->gl_context);
}

void Window::swapBuffers() {
    SDL_GL_SwapWindow(impl_->sdl_window);
}

void* Window::getNativeHandle() const {
    return impl_->sdl_window;
}

void* Window::getGLContext() const {
    return impl_->gl_context;
}

}  // namespace vaura
