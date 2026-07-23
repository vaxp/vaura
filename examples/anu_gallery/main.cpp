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
    WindowConfig config{"VAURA - Anu Gallery Demo", 1000, 700};
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

    // Root - Column
    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF222222)));
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setFlexDirection(FlexDirection::Column);
    root->layoutNode().setPadding(Edge::All, 20);

    // Header
    auto header = static_cast<RenderDecoratedBox*>(addNode(createBox(0x00000000)));
    header->layoutNode().setWidthPercent(100);
    header->layoutNode().setHeight(60);
    header->layoutNode().setFlexDirection(FlexDirection::Row);
    header->layoutNode().setJustifyContent(MainAxisAlign::SpaceBetween);
    header->layoutNode().setAlignItems(CrossAxisAlign::Center);
    root->addChild(header);

    auto title = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFFFFFFFF, 4)));
    title->layoutNode().setWidth(200);
    title->layoutNode().setHeight(20);
    header->addChild(title);

    // Grid Area
    auto grid = static_cast<RenderDecoratedBox*>(addNode(createBox(0x00000000)));
    grid->layoutNode().setFlexGrow(1.0f);
    grid->layoutNode().setFlexDirection(FlexDirection::Row);
    grid->layoutNode().setFlexWrap(true);
    grid->layoutNode().setJustifyContent(MainAxisAlign::Center);
    grid->layoutNode().setAlignItems(CrossAxisAlign::Start);
    root->addChild(grid);

    // Images
    int heights[] = {200, 150, 300, 200, 250, 150, 200, 250, 150, 200};
    Color colors[] = {0xFFEF476F, 0xFFFFD166, 0xFF06D6A0, 0xFF118AB2, 0xFF073B4C};

    for (int i = 0; i < 10; i++) {
        auto imgCard = static_cast<RenderDecoratedBox*>(addNode(createBox(colors[i % 5], 10)));
        imgCard->layoutNode().setWidthPercent(22); // 4 per row with margins
        imgCard->layoutNode().setHeight(heights[i]);
        imgCard->layoutNode().setMargin(Edge::All, 10);
        grid->addChild(imgCard);
    }

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
