/// @file platform_sdl3.cpp
/// @brief SDL2 platform backend implementation.
/// NOTE: Using SDL2 as the backend for now. Will migrate to SDL3
/// when it becomes available via package manager.

#include "vaura/platform/platform.hpp"
#include "vaura/overlay/popup_window_controller.hpp"

#ifdef VAURA_HAS_SDL3
#include <SDL3/SDL.h>
#elif defined(VAURA_HAS_SDL2)
#include <SDL2/SDL.h>
#else
// No SDL available — provide stubs
#define SDL_STUB_MODE
#endif

#include <chrono>

namespace vaura {

static Platform* g_platform_instance = nullptr;

// ============================================================
// Platform::Impl
// ============================================================
struct Platform::Impl {
    bool initialized = false;
    std::chrono::high_resolution_clock::time_point start_time;
};

// ============================================================
// Platform
// ============================================================

Platform::Platform() : impl_(std::make_unique<Impl>()) {}

Platform::~Platform() {
    if (g_platform_instance == this) {
        g_platform_instance = nullptr;
    }
    if (impl_ && impl_->initialized) {
        SDL_Quit();
        impl_->initialized = false;
    }
}

Platform* Platform::instance() {
    return g_platform_instance;
}

Result<std::unique_ptr<Platform>> Platform::create() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        return Result<std::unique_ptr<Platform>>::err(
            ErrorCode::PlatformError,
            std::string("SDL_Init failed: ") + SDL_GetError()
        );
    }

    auto platform = std::unique_ptr<Platform>(new Platform());
    platform->impl_->initialized = true;
    platform->impl_->start_time = std::chrono::high_resolution_clock::now();

    g_platform_instance = platform.get();

    // Request OpenGL 3.3 Core profile (for Skia)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);  // no depth buffer needed for 2D
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

    return Result<std::unique_ptr<Platform>>::ok(std::move(platform));
}

bool Platform::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // ── Popup-first event routing ───────────────────────────
        // If a popup window owns this event's target, let it handle it.
        // dispatchEvent() returns true if the event was consumed.
        if (PopupWindowController::instance().dispatchEvent(&event)) {
            continue;
        }

        switch (event.type) {
            case SDL_QUIT:
                on_quit_.emit();
                return false;

            case SDL_MOUSEBUTTONDOWN:
                on_mouse_down_.emit(
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y),
                    event.button.button);
                break;

            case SDL_MOUSEBUTTONUP:
                on_mouse_up_.emit(
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y),
                    event.button.button);
                break;

            case SDL_MOUSEMOTION:
                on_mouse_move_.emit(
                    static_cast<float>(event.motion.x),
                    static_cast<float>(event.motion.y));
                break;

            case SDL_TEXTINPUT:
                on_text_input_.emit(std::string_view(event.text.text));
                break;

            case SDL_KEYDOWN:
                on_key_down_.emit(event.key.keysym.sym);
                break;

            case SDL_MOUSEWHEEL:
                on_scroll_.emit(event.wheel.x, event.wheel.y);
                break;

            default:
                break;
        }
    }
    return true;
}


double Platform::getTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(now - impl_->start_time);
    return duration.count();
}

std::string Platform::getClipboardText() const {
    char* text = SDL_GetClipboardText();
    if (!text) return "";
    std::string result(text);
    SDL_free(text);
    return result;
}

void Platform::setClipboardText(std::string_view text) {
    SDL_SetClipboardText(std::string(text).c_str());
}

void Platform::startTextInput() {
    SDL_StartTextInput();
}

void Platform::stopTextInput() {
    SDL_StopTextInput();
}

}  // namespace vaura
