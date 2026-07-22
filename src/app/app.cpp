/// @file app.cpp
/// @brief VAURA App lifecycle implementation.
///
/// This file owns the "scary" infrastructure code so app developers
/// never have to think about it:
///   - SDL event loop integration
///   - Skia GL backend setup
///   - BuildOwner + Element tree bootstrap
///   - Per-frame: build → layout → paint → flush → swap
///
/// @copyright VAURA Framework — MIT License

#include "vaura/app/app.hpp"
#include "vaura/tree/element.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/widgets/gesture_detector.hpp"

// SDL
#include <SDL2/SDL.h>

// Skia GPU
#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/GrBackendSurface.h>
#include <include/gpu/gl/GrGLInterface.h>

#include "vaura/animation/ticker.hpp"
#include "vaura/overlay/overlay.hpp"
#include "vaura/overlay/popup_window_controller.hpp"

#include <cassert>
#include <stdexcept>
#include <chrono>
#include <thread>

namespace vaura {

// Forward declaration of internal canvas factory
std::unique_ptr<Canvas> createCanvasWrapper(void* sk_canvas);

// ════════════════════════════════════════════════════════════════
// App::Impl — internal state
// ════════════════════════════════════════════════════════════════

struct App::Impl {
    AppConfig config;
    std::string title;

    // Platform layer
    std::unique_ptr<Platform> platform;
    std::unique_ptr<Window>   window;

    // Skia GPU context
    sk_sp<GrDirectContext> gr_context;

    // Widget tree
    WidgetPtr                   root_widget;
    std::unique_ptr<BuildOwner> build_owner;
    std::unique_ptr<Element>    root_element;

    // Loop control
    bool quit_requested = false;

    // Last hovered gesture detector (for hover exit)
    RenderGestureDetector* last_hovered = nullptr;
    float last_pointer_x = 0.0f;
    float last_pointer_y = 0.0f;

    // DPI scale (for coordinate conversion)
    float dpi_scale = 1.0f;

    // Timing
    using Clock = std::chrono::steady_clock;
    Clock::time_point last_frame_time;

    // ── Initialization ──────────────────────────────────────────

    bool initPlatform() {
        auto result = Platform::create();
        if (!result.isOk()) return false;
        platform = std::move(result.value());
        return true;
    }

    bool initWindow() {
        WindowConfig win_cfg;
        win_cfg.title     = config.title;
        win_cfg.width     = config.width;
        win_cfg.height    = config.height;
        win_cfg.resizable = config.resizable;
        win_cfg.vsync     = config.vsync;

        auto result = Window::create(*platform, win_cfg);
        if (!result.isOk()) return false;
        window = std::move(result.value());
        title  = config.title;

        // Connect quit signals
        platform->onQuit().connect([this]() { quit_requested = true; });
        window->onClose().connect([this]()  { quit_requested = true; });

        // Connect mouse events → event dispatcher
        platform->onMouseMove().connect([this](float x, float y) {
            last_pointer_x = x;
            last_pointer_y = y;
            dispatchPointerMove(x, y);
        });
        platform->onMouseDown().connect([this](float x, float y, int btn) {
            dispatchPointerDown(x, y, btn);
        });
        platform->onMouseUp().connect([this](float x, float y, int btn) {
            dispatchPointerUp(x, y, btn);
        });
        platform->onScroll().connect([this](float dx, float dy) {
            dispatchScroll(dx, dy);
        });

        return true;
    }

    bool initSkia() {
        window->makeCurrent();
        auto interface = GrGLMakeNativeInterface();
        if (!interface) return false;
        gr_context = GrDirectContext::MakeGL(interface);
        return gr_context != nullptr;
    }

    bool initWidgetTree() {
        // Wrap user root with the OverlayStack so overlays can float above all content
        auto wrapped_root = overlay_stack(std::move(root_widget));
        root_widget = wrapped_root;

        build_owner  = std::make_unique<BuildOwner>();
        root_element = root_widget->createElement();
        root_element->setOwner(build_owner.get());
        root_element->mount(nullptr, 0);
        build_owner->buildScope(root_element.get());
        return true;
    }

    // ── Event Dispatching ───────────────────────────────────────

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
                    .position = {x, y},
                    .localPosition = entry.localPosition,
                    .button = MouseButton::None
                };
                gd->handlePointerMove(e);
                break; // Only the topmost gesture detector gets the hover
            }
        }

        if (last_hovered != hovered) {
            if (last_hovered) {
                last_hovered->handlePointerExit({});
            }
            if (hovered) {
                hovered->handlePointerEnter({});
            }
            last_hovered = hovered;
        }
    }

    void dispatchScroll(float dx, float dy) {
        if (!root_element) return;
        auto* root_ro = root_element->findRenderObject();
        if (!root_ro) return;

        HitTestResult result;
        root_ro->hitTest(result, {last_pointer_x, last_pointer_y});

        for (const auto& entry : result.path()) {
            if (entry.target->handlesScroll()) {
                entry.target->handlePointerScroll(dx, dy);
                break; // Consume the scroll
            }
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
                    .position = {x, y},
                    .localPosition = entry.localPosition,
                    .button = static_cast<MouseButton>(btn)
                };
                gd->handlePointerDown(e);
                break; // Only the topmost gesture detector gets the click
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
                    .position = {x, y},
                    .localPosition = entry.localPosition,
                    .button = static_cast<MouseButton>(btn)
                };
                gd->handlePointerUp(e);
                break; // Only the topmost gesture detector gets the click
            }
        }
    }

    // ── Per-frame rendering ─────────────────────────────────────

    void renderFrame() {
        auto s = window->getDrawableSize();

        GrGLFramebufferInfo fbInfo;
        fbInfo.fFBOID  = 0;
        fbInfo.fFormat = 0x8058; // GL_RGBA8

        GrBackendRenderTarget backendRT(
            static_cast<int>(s.width),
            static_cast<int>(s.height),
            0, 8, fbInfo);

        auto surface = SkSurface::MakeFromBackendRenderTarget(
            gr_context.get(), backendRT,
            kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType,
            nullptr, nullptr);

        if (!surface) return;

        SkCanvas* sk_canvas = surface->getCanvas();

        // Clear background
        const Color cc = config.clear_color;
        sk_canvas->clear(SkColorSetARGB(
            (cc >> 24) & 0xFF,
            (cc >> 16) & 0xFF,
            (cc >>  8) & 0xFF,
            (cc >>  0) & 0xFF));

        // Advance all animations (Tickers / AnimationControllers)
        SchedulerBinding::instance().tick();

        // Rebuild any dirty elements before painting
        build_owner->buildScope(root_element.get());

        // Avoid use-after-free by clearing last_hovered.
        // Re-dispatch pointer move to restore hover states on the newly built tree.
        last_hovered = nullptr;
        
        // Layout + Paint
        auto* root_ro = root_element->findRenderObject();
        if (root_ro) {
            root_ro->layout(s.width, s.height);

            // Re-dispatch hit test after layout to update hover targets.
            dispatchPointerMove(last_pointer_x, last_pointer_y);

            auto canvas = createCanvasWrapper(sk_canvas);
            PaintContext pctx{*canvas, Point{0, 0},
                              Rect{0, 0, s.width, s.height}, 1.0f};
            root_ro->paint(pctx);
        }

        gr_context->flush();
        window->swapBuffers();

        // ② Render all native popup windows (each manages its own GL context)
        PopupWindowController::instance().renderAll();

        // ③ Restore main window GL context for the next frame
        window->makeCurrent();
        gr_context->resetContext();
    }

    // ── Frame timing ────────────────────────────────────────────

    void capFrameRate() {
        if (config.target_fps <= 0) return;

        using namespace std::chrono;
        const auto frame_duration = duration_cast<Clock::duration>(
            duration<double>(1.0 / config.target_fps));

        auto now     = Clock::now();
        auto elapsed = now - last_frame_time;
        if (elapsed < frame_duration) {
            std::this_thread::sleep_for(frame_duration - elapsed);
        }
        last_frame_time = Clock::now();
    }
};

// ════════════════════════════════════════════════════════════════
// App — public API
// ════════════════════════════════════════════════════════════════

App::App() : impl_(std::make_unique<Impl>()) {}

App::~App() {
    if (impl_->root_element) {
        impl_->root_element->unmount();
    }
}

Result<std::unique_ptr<App>> App::create(WidgetPtr root_widget, AppConfig config) {
    auto app = std::unique_ptr<App>(new App());
    auto& impl = *app->impl_;

    impl.config      = config;
    impl.root_widget = std::move(root_widget);

    if (!impl.initPlatform())   return Result<std::unique_ptr<App>>::err(ErrorCode::PlatformError, "Failed to initialize platform (SDL)");
    if (!impl.initWindow())     return Result<std::unique_ptr<App>>::err(ErrorCode::WindowError,   "Failed to create window");
    if (!impl.initSkia())       return Result<std::unique_ptr<App>>::err(ErrorCode::RenderingError, "Failed to initialize Skia GPU context");
    if (!impl.initWidgetTree()) return Result<std::unique_ptr<App>>::err(ErrorCode::NotInitialized, "Failed to build widget tree");

    impl.last_frame_time = Impl::Clock::now();

    return Result<std::unique_ptr<App>>::ok(std::move(app));
}

int App::run() {
    auto& impl = *impl_;

    while (!impl.quit_requested) {
        // 1. Poll platform events
        if (!impl.platform->pollEvents()) {
            impl.quit_requested = true;
            break;
        }

        // 2. Render the frame
        impl.renderFrame();

        // 3. Cap to target FPS
        impl.capFrameRate();
    }

    return 0;
}

void App::quit() { impl_->quit_requested = true; }

const std::string& App::title() const { return impl_->title; }

void App::setTitle(std::string_view title) {
    impl_->title = title;
    impl_->window->setTitle(title);
}

Size App::windowSize() const { return impl_->window->getSize(); }
float App::dpiScale()   const { return impl_->window->getDpiScale(); }
Platform& App::platform()    { return *impl_->platform; }
Window&   App::window()      { return *impl_->window; }

// ════════════════════════════════════════════════════════════════
// runApp — convenience entry point
// ════════════════════════════════════════════════════════════════

int runApp(WidgetPtr root_widget, AppConfig config) {
    auto result = App::create(std::move(root_widget), std::move(config));
    if (!result.isOk()) {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR,
            "VAURA Launch Error",
            "Failed to initialize application.",
            nullptr);
        return 1;
    }
    return result.value()->run();
}

}  // namespace vaura
