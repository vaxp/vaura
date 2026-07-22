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
    WindowConfig config{"VAURA Torture - Deep Tree 100", 800, 600};
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
    auto addNode = [&](std::unique_ptr<RenderObject> node) -> RenderObject* {
        nodes.push_back(std::move(node));
        return nodes.back().get();
    };

    printf("Building 100 levels deep tree...\n");
    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF1E1E1E)));
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setJustifyContent(MainAxisAlign::Center);
    root->layoutNode().setAlignItems(CrossAxisAlign::Center);

    RenderDecoratedBox* current_parent = root;
    
    // Create 100 nested containers
    for (int i = 0; i < 100; i++) {
        // Color gradient from dark to light
        uint8_t c = static_cast<uint8_t>(30 + i * 2);
        Color col = (0xFF << 24) | (c << 16) | (c << 8) | c;
        auto box = static_cast<RenderDecoratedBox*>(addNode(createBox(col, 2)));
        
        // Each box is slightly smaller than its parent
        box->layoutNode().setWidthPercent(95);
        box->layoutNode().setHeightPercent(95);
        box->layoutNode().setJustifyContent(MainAxisAlign::Center);
        box->layoutNode().setAlignItems(CrossAxisAlign::Center);
        
        current_parent->addChild(box);
        current_parent = box;
    }
    printf("Deep tree built successfully.\n");

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
        root->layout(win_size.width, win_size.height);

        if (surface) {
            auto* sk_canvas = surface->getCanvas();
            auto canvas_wrapper = createCanvasWrapper(sk_canvas);
            canvas_wrapper->clear(0xFF000000);
            PaintContext paint_ctx{*canvas_wrapper, {0, 0}, Rect{0, 0, win_size.width, win_size.height}, 1.0f};
            root->paint(paint_ctx);
            context->flush();
            window->swapBuffers();
        }
    }
    return 0;
}
