/// @file popup_window_controller.cpp
/// @brief PopupWindowController implementation.

#include "vaura/overlay/popup_window_controller.hpp"

#ifdef VAURA_HAS_SDL3
#include <SDL3/SDL.h>
#elif defined(VAURA_HAS_SDL2)
#include <SDL2/SDL.h>
#endif

#include <algorithm>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// Singleton
// ════════════════════════════════════════════════════════════════

PopupWindowController& PopupWindowController::instance() {
    static PopupWindowController inst;
    return inst;
}

// ════════════════════════════════════════════════════════════════
// Popup Management
// ════════════════════════════════════════════════════════════════

PopupId PopupWindowController::open(PopupWindowConfig config, WidgetPtr content) {
    auto window = PopupWindow::create(config, std::move(content));
    if (!window) return 0;  // Creation failed — fail gracefully

    PopupId id = next_id_++;
    popups_.push_back({ id, std::move(window) });
    return id;
}

void PopupWindowController::close(PopupId id) {
    for (auto& entry : popups_) {
        if (entry.id == id) {
            entry.window->close();
            return;
        }
    }
}

void PopupWindowController::closeAll() {
    for (auto& entry : popups_) {
        entry.window->close();
    }
}

bool PopupWindowController::hasOpenPopups() const {
    for (const auto& entry : popups_) {
        if (!entry.window->isClosed()) return true;
    }
    return false;
}

// ════════════════════════════════════════════════════════════════
// Per-frame rendering
// ════════════════════════════════════════════════════════════════

void PopupWindowController::renderAll() {
    // Render all open popups
    for (auto& entry : popups_) {
        if (!entry.window->isClosed()) {
            entry.window->render();
        }
    }

    // Remove popups that have been closed (deferred cleanup)
    popups_.erase(
        std::remove_if(popups_.begin(), popups_.end(),
            [](const Entry& e) { return e.window->isClosed(); }),
        popups_.end()
    );
}

// ════════════════════════════════════════════════════════════════
// Event routing
// ════════════════════════════════════════════════════════════════

bool PopupWindowController::dispatchEvent(void* sdl_event) {
    if (popups_.empty()) return false;

    SDL_Event* event = static_cast<SDL_Event*>(sdl_event);

    // Extract the windowID from the event (varies by event type)
    uint32_t target_window_id = 0;
    switch (event->type) {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            target_window_id = event->button.windowID;
            break;
        case SDL_MOUSEMOTION:
            target_window_id = event->motion.windowID;
            break;
        case SDL_WINDOWEVENT:
            target_window_id = event->window.windowID;
            break;
        default:
            return false;
    }

    // Route to the popup that owns this window
    for (auto& entry : popups_) {
        if (entry.window->isClosed()) continue;
        if (entry.window->sdlWindowId() == target_window_id) {
            return entry.window->dispatchEvent(sdl_event);
        }
    }

    // Event was for the main window but we have open popups:
    // A click on the main window while a popup is open should close all popups.
    if (event->type == SDL_MOUSEBUTTONDOWN && hasOpenPopups()) {
        closeAll();
        return false;  // Don't consume — let the main window also process it
    }

    return false;
}

}  // namespace vaura
