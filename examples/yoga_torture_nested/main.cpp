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
    WindowConfig config{"VAURA Torture - Nested & Resize", 800, 600};
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

    // Container
    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF1E1E1E)));
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setPadding(Edge::All, 20);

    // Simulated Scroll (A huge container that overflows)
    auto scroll = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF2A2A2A, 10)));
    scroll->layoutNode().setWidthPercent(100);
    scroll->layoutNode().setHeightPercent(100);
    scroll->layoutNode().setFlexDirection(FlexDirection::Column);
    root->addChild(scroll);

    // Stack (Absolute Positioning Container)
    auto stack = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF333333, 10)));
    stack->layoutNode().setFlexGrow(1.0f); // Takes all available scroll space
    stack->layoutNode().setMargin(Edge::All, 20);
    scroll->addChild(stack);

    // Absolute element inside Stack
    auto absBg = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF550000)));
    absBg->layoutNode().setPositionType(true);
    absBg->layoutNode().setPosition(Edge::All, 0); // Fill Stack
    stack->addChild(absBg);

    // Column inside Stack
    auto column = static_cast<RenderDecoratedBox*>(addNode(createBox(0x00000000))); // Transparent
    column->layoutNode().setWidthPercent(100);
    column->layoutNode().setHeightPercent(100);
    column->layoutNode().setFlexDirection(FlexDirection::Column);
    column->layoutNode().setJustifyContent(MainAxisAlign::SpaceAround);
    column->layoutNode().setAlignItems(CrossAxisAlign::Center);
    stack->addChild(column);

    // Row inside Column
    for (int i = 0; i < 5; i++) {
        auto row = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF005500, 5)));
        row->layoutNode().setWidthPercent(80);
        row->layoutNode().setHeight(50);
        row->layoutNode().setFlexDirection(FlexDirection::Row);
        row->layoutNode().setJustifyContent(MainAxisAlign::SpaceBetween);
        row->layoutNode().setAlignItems(CrossAxisAlign::Center);
        row->layoutNode().setPadding(Edge::Horizontal, 10);
        column->addChild(row);

        // Items inside Row
        for (int j = 0; j < 4; j++) {
            auto item = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF000055, 20)));
            item->layoutNode().setWidth(30);
            item->layoutNode().setHeight(30);
            row->addChild(item);
        }
    }

    bool running = true;
    platform->onQuit().connect([&]() { running = false; });

    while (running) {
        if (!platform->pollEvents()) break;

        Size new_size = window->getDrawableSize();
        if (new_size.width != current_size.width || new_size.height != current_size.height) {
            current_size = new_size;
            update_surface();
            printf("Resized to: %.0fx%.0f. Yoga will recalculate.\n", new_size.width, new_size.height);
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
