/// @file popup_window.cpp
/// @brief PopupWindow implementation — fully independent SDL window with
///        its own GL context, Skia GPU context, and widget tree.

#include "vaura/platform/popup_window.hpp"
#include "vaura/tree/element.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/widgets/gesture_detector.hpp"

#ifdef VAURA_HAS_SDL3
#include <SDL3/SDL.h>
#elif defined(VAURA_HAS_SDL2)
#include <SDL2/SDL.h>
#else
#define SDL_STUB_MODE
#endif

// Skia GPU
#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/GrBackendSurface.h>
#include <include/gpu/gl/GrGLInterface.h>

#include <iostream>

namespace vaura {

// Forward declaration from canvas.cpp
std::unique_ptr<Canvas> createCanvasWrapper(void* sk_canvas);

// ════════════════════════════════════════════════════════════════
// PopupWindow::Impl
// ════════════════════════════════════════════════════════════════

struct PopupWindow::Impl {
    PopupWindowConfig config;

    // SDL resources (independent from main window)
    SDL_Window*   sdl_window   = nullptr;
    SDL_GLContext gl_context   = nullptr;

    // Skia resources (independent from main window)
    sk_sp<GrDirectContext> gr_context;

    // Widget tree (independent from main window)
    WidgetPtr                   root_widget;
    std::unique_ptr<BuildOwner> build_owner;
    std::unique_ptr<Element>    root_element;

    // Event dispatch helpers
    RenderGestureDetector* last_hovered = nullptr;
    float last_pointer_x = 0.0f;
    float last_pointer_y = 0.0f;

    // State
    bool closed = false;

    // ── Initialization ───────────────────────────────────────────

    bool init(PopupWindowConfig cfg, WidgetPtr content) {
        config     = cfg;
        root_widget = std::move(content);

        if (!createWindow()) return false;
        if (!createSkia())   return false;
        if (!mountTree())    return false;

        return true;
    }

    bool createWindow() {
        // SDL2: explicitly do NOT share context
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 0);

        // Same GL requirements as the main window
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE,   8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,  8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

        // Popup flags: borderless, always on top, skip taskbar
        Uint32 flags = SDL_WINDOW_OPENGL
                     | SDL_WINDOW_BORDERLESS
                     | SDL_WINDOW_SKIP_TASKBAR
                     | SDL_WINDOW_ALWAYS_ON_TOP;

        sdl_window = SDL_CreateWindow(
            "",                  // no title
            config.x, config.y,
            config.width, config.height,
            flags
        );

        if (!sdl_window) {
            std::cerr << "[VAURA PopupWindow] SDL_CreateWindow failed: "
                      << SDL_GetError() << "\n";
            return false;
        }

        // Independent GL context
        gl_context = SDL_GL_CreateContext(sdl_window);
        if (!gl_context) {
            std::cerr << "[VAURA PopupWindow] SDL_GL_CreateContext failed: "
                      << SDL_GetError() << "\n";
            SDL_DestroyWindow(sdl_window);
            sdl_window = nullptr;
            return false;
        }

        SDL_GL_SetSwapInterval(0); // No VSync for popups (they need to respond fast)
        return true;
    }

    bool createSkia() {
        SDL_GL_MakeCurrent(sdl_window, gl_context);

        auto interface = GrGLMakeNativeInterface();
        if (!interface) {
            std::cerr << "[VAURA PopupWindow] GrGLMakeNativeInterface failed\n";
            return false;
        }

        gr_context = GrDirectContext::MakeGL(interface);
        if (!gr_context) {
            std::cerr << "[VAURA PopupWindow] GrDirectContext::MakeGL failed\n";
            return false;
        }

        return true;
    }

    bool mountTree() {
        build_owner  = std::make_unique<BuildOwner>();
        root_element = root_widget->createElement();
        root_element->setOwner(build_owner.get());
        root_element->mount(nullptr, 0);
        build_owner->buildScope(root_element.get());
        return true;
    }

    // ── Rendering ───────────────────────────────────────────────

    void render() {
        if (closed) return;

        SDL_GL_MakeCurrent(sdl_window, gl_context);

        // After changing GL context, Skia's cached GL state is invalid
        gr_context->resetContext();

        int w, h;
        SDL_GL_GetDrawableSize(sdl_window, &w, &h);
        if (w <= 0 || h <= 0) return;

        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID  = 0;
        fbInfo.fFormat = 0x8058; // GL_RGBA8

        GrBackendRenderTarget backendRT(w, h, 0, 8, fbInfo);

        auto surface = SkSurface::MakeFromBackendRenderTarget(
            gr_context.get(), backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr, nullptr);

        if (!surface) return;

        SkCanvas* sk_canvas = surface->getCanvas();

        const Color cc = config.clear_color;
        sk_canvas->clear(SkColorSetARGB(
            (cc >> 24) & 0xFF,
            (cc >> 16) & 0xFF,
            (cc >>  8) & 0xFF,
            (cc >>  0) & 0xFF));

        build_owner->buildScope(root_element.get());

        auto* root_ro = root_element->findRenderObject();
        if (root_ro) {
            root_ro->layout(static_cast<float>(w), static_cast<float>(h));
            auto canvas = createCanvasWrapper(sk_canvas);
            PaintContext pctx{*canvas, Point{0, 0},
                              Rect{0, 0, static_cast<float>(w), static_cast<float>(h)}, 1.0f};
            root_ro->paint(pctx);
        }

        gr_context->flush();
        SDL_GL_SwapWindow(sdl_window);
    }

    // ── Event Dispatching ────────────────────────────────────────

    bool dispatchEvent(SDL_Event& event) {
        if (closed) return false;

        // Only handle events addressed to our window
        uint32_t our_id = SDL_GetWindowID(sdl_window);

        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                if (event.button.windowID != our_id) return false;
                dispatchPointerDown(
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y),
                    event.button.button);
                return true;
            }
            case SDL_MOUSEBUTTONUP: {
                if (event.button.windowID != our_id) return false;
                dispatchPointerUp(
                    static_cast<float>(event.button.x),
                    static_cast<float>(event.button.y),
                    event.button.button);
                return true;
            }
            case SDL_MOUSEMOTION: {
                if (event.motion.windowID != our_id) return false;
                last_pointer_x = static_cast<float>(event.motion.x);
                last_pointer_y = static_cast<float>(event.motion.y);
                dispatchPointerMove(last_pointer_x, last_pointer_y);
                return true;
            }
            case SDL_WINDOWEVENT: {
                if (event.window.windowID != our_id) return false;
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                    // Close popup when it loses focus (user clicked elsewhere)
                    closed = true;
                }
                return true;
            }
            default:
                return false;
        }
    }

    void dispatchPointerDown(float x, float y, int btn) {
        if (!root_element) return;
        auto* root_ro = root_element->findRenderObject();
        if (!root_ro) return;

        HitTestResult result;
        root_ro->hitTest(result, {x, y});

        for (const auto& entry : result.path()) {
            if (auto* gd = dynamic_cast<RenderGestureDetector*>(entry.target)) {
                PointerEvent e{
                    .position      = {x, y},
                    .localPosition = entry.localPosition,
                    .button        = static_cast<MouseButton>(btn)
                };
                gd->handlePointerDown(e);
                break;
            }
        }
    }

    void dispatchPointerUp(float x, float y, int btn) {
        if (!root_element) return;
        auto* root_ro = root_element->findRenderObject();
        if (!root_ro) return;

        HitTestResult result;
        root_ro->hitTest(result, {x, y});

        for (const auto& entry : result.path()) {
            if (auto* gd = dynamic_cast<RenderGestureDetector*>(entry.target)) {
                PointerEvent e{
                    .position      = {x, y},
                    .localPosition = entry.localPosition,
                    .button        = static_cast<MouseButton>(btn)
                };
                gd->handlePointerUp(e);
                break;
            }
        }
    }

    void dispatchPointerMove(float x, float y) {
        if (!root_element) return;
        auto* root_ro = root_element->findRenderObject();
        if (!root_ro) return;

        HitTestResult result;
        root_ro->hitTest(result, {x, y});

        RenderGestureDetector* hovered = nullptr;
        for (const auto& entry : result.path()) {
            if (auto* gd = dynamic_cast<RenderGestureDetector*>(entry.target)) {
                hovered = gd;
                PointerEvent e{
                    .position      = {x, y},
                    .localPosition = entry.localPosition,
                    .button        = MouseButton::None
                };
                gd->handlePointerMove(e);
                break;
            }
        }

        if (last_hovered != hovered) {
            if (last_hovered) last_hovered->handlePointerExit({});
            if (hovered)      hovered->handlePointerEnter({});
            last_hovered = hovered;
        }
    }

    // ── Cleanup ──────────────────────────────────────────────────

    void destroy() {
        if (root_element) {
            root_element->unmount();
            root_element.reset();
        }
        build_owner.reset();
        gr_context.reset();

        if (gl_context) {
            SDL_GL_DeleteContext(gl_context);
            gl_context = nullptr;
        }
        if (sdl_window) {
            SDL_DestroyWindow(sdl_window);
            sdl_window = nullptr;
        }
    }
};

// ════════════════════════════════════════════════════════════════
// PopupWindow — public API
// ════════════════════════════════════════════════════════════════

PopupWindow::PopupWindow()  : impl_(std::make_unique<Impl>()) {}
PopupWindow::~PopupWindow() { impl_->destroy(); }

std::unique_ptr<PopupWindow> PopupWindow::create(PopupWindowConfig config, WidgetPtr content) {
    auto pw = std::unique_ptr<PopupWindow>(new PopupWindow());
    if (!pw->impl_->init(config, std::move(content))) {
        return nullptr;
    }
    return pw;
}

void     PopupWindow::render()            { impl_->render(); }
void     PopupWindow::close()             { impl_->closed = true; }
bool     PopupWindow::isClosed()    const { return impl_->closed; }
uint32_t PopupWindow::sdlWindowId() const {
    if (!impl_->sdl_window) return 0;
    return SDL_GetWindowID(impl_->sdl_window);
}
void* PopupWindow::nativeHandle() const { return impl_->sdl_window; }

bool PopupWindow::dispatchEvent(void* sdl_event) {
    return impl_->dispatchEvent(*static_cast<SDL_Event*>(sdl_event));
}

}  // namespace vaura
