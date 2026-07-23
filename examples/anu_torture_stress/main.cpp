#include "vaura/platform/platform.hpp"
#include "vaura/platform/window.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/paint.hpp"
#include "vaura/rendering/color.hpp"

#include <include/core/SkCanvas.h>
#include <include/core/SkSurface.h>
#include <include/gpu/GrDirectContext.h>
#include <include/gpu/gl/GrGLInterface.h>
#include <vector>
#include <memory>
#include <chrono>
#include <cstdio>

using namespace vaura;

namespace vaura {
extern std::unique_ptr<Canvas> createCanvasWrapper(void* sk_canvas);
}

std::unique_ptr<RenderDecoratedBox> createBox(Color color, float radius = 0) {
    auto box = std::make_unique<RenderDecoratedBox>();
    RenderDecoratedBox::Decoration dec;
    dec.color = color;
    if (radius > 0) dec.border_radius = BorderRadius{radius, radius, radius, radius};
    box->setDecoration(dec);
    return box;
}

int main() {
    auto platform = std::move(Platform::create().value());
    WindowConfig config{"VAURA Torture - Stress Test 1000", 800, 600};
    auto window = std::move(Window::create(*platform, config).value());
    window->makeCurrent();

    auto interface = GrGLMakeNativeInterface();
    sk_sp<GrDirectContext> context = GrDirectContext::MakeGL(interface);
    sk_sp<SkSurface> surface;
    auto update_surface = [&]() {
        Size s = window->getDrawableSize();
        GrGLFramebufferInfo fbInfo{0, 0x8058};
        GrBackendRenderTarget backendRT(s.width, s.height, 0, 8, fbInfo);
        surface = SkSurface::MakeFromBackendRenderTarget(context.get(), backendRT, kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType, nullptr, nullptr);
    };
    update_surface();
    Size current_size = window->getDrawableSize();

    std::vector<std::unique_ptr<RenderObject>> nodes;
    nodes.reserve(1500); // Prevent reallocation
    auto addNode = [&](std::unique_ptr<RenderObject> node) -> RenderObject* {
        nodes.push_back(std::move(node));
        return nodes.back().get();
    };

    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF111111)));
    root->layoutNode().setWidthPercent(100);
    // Let height grow to accommodate 1000 items
    root->layoutNode().setFlexDirection(FlexDirection::Column);
    root->layoutNode().setFlexWrap(true); // Let them wrap around to form a massive grid

    // Add 1000 items
    printf("Building tree with 1000 widgets...\n");
    for (int i = 0; i < 1000; i++) {
        // We make them small so they fit on screen (wrap mode)
        auto item = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF222222 + (i % 255))));
        item->layoutNode().setWidth(10);
        item->layoutNode().setHeight(10);
        item->layoutNode().setMargin(Edge::All, 1);
        root->addChild(item);
    }
    printf("Tree built. Nodes count: %zu\n", nodes.size());

    bool running = true;
    platform->onQuit().connect([&]() { running = false; });

    while (running) {
        if (!platform->pollEvents()) break;

        Size new_size = window->getDrawableSize();
        if (new_size.width != current_size.width || new_size.height != current_size.height) {
            current_size = new_size;
            update_surface();
        }

        Size win_size = window->getSize();
        
        // Measure Layout Time
        auto t1 = std::chrono::high_resolution_clock::now();
        root->layout(win_size.width, NAN); // NAN height allows it to grow
        auto t2 = std::chrono::high_resolution_clock::now();
        double layout_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

        if (surface) {
            auto* sk_canvas = surface->getCanvas();
            auto canvas_wrapper = createCanvasWrapper(sk_canvas);
            canvas_wrapper->clear(0xFF000000);
            
            // Measure Paint Time
            auto t3 = std::chrono::high_resolution_clock::now();
            PaintContext paint_ctx{*canvas_wrapper, {0, 0}, Rect{0, 0, win_size.width, win_size.height}, 1.0f};
            root->paint(paint_ctx);
            auto t4 = std::chrono::high_resolution_clock::now();
            double paint_ms = std::chrono::duration<double, std::milli>(t4 - t3).count();

            context->flush();
            window->swapBuffers();
            
            // Print performance every frame is spammy, maybe once
            static bool printed = false;
            if (!printed) {
                printf("First Frame Performance: Layout=%.2fms, Paint=%.2fms\n", layout_ms, paint_ms);
                printed = true;
            }
        }
    }
    return 0;
}
