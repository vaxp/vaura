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
    WindowConfig config{"VAURA Torture - RTL vs LTR", 800, 600};
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

    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF222222)));
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setFlexDirection(FlexDirection::Column);
    root->layoutNode().setJustifyContent(MainAxisAlign::SpaceEvenly);
    root->layoutNode().setPadding(Edge::All, 20);

    // LTR Row (English style)
    auto ltrRow = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF333333, 10)));
    ltrRow->layoutNode().setWidthPercent(100);
    ltrRow->layoutNode().setHeight(100);
    ltrRow->layoutNode().setFlexDirection(FlexDirection::Row);
    ltrRow->layoutNode().setDirection(LayoutDirection::LTR); // Explicit LTR
    ltrRow->layoutNode().setPadding(Edge::All, 10);
    root->addChild(ltrRow);

    // RTL Row (Arabic style)
    auto rtlRow = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF444444, 10)));
    rtlRow->layoutNode().setWidthPercent(100);
    rtlRow->layoutNode().setHeight(100);
    rtlRow->layoutNode().setFlexDirection(FlexDirection::Row);
    rtlRow->layoutNode().setDirection(LayoutDirection::RTL); // Explicit RTL
    rtlRow->layoutNode().setPadding(Edge::All, 10);
    root->addChild(rtlRow);

    Color colors[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF};
    float widths[] = {50, 100, 150};

    // Add elements to LTR
    for (int i = 0; i < 3; i++) {
        auto box = static_cast<RenderDecoratedBox*>(addNode(createBox(colors[i], 5)));
        box->layoutNode().setWidth(widths[i]);
        box->layoutNode().setHeight(50);
        box->layoutNode().setMargin(Edge::Right, 10); // Margin respects direction in Anu!
        ltrRow->addChild(box);
    }

    // Add same elements to RTL
    for (int i = 0; i < 3; i++) {
        auto box = static_cast<RenderDecoratedBox*>(addNode(createBox(colors[i], 5)));
        box->layoutNode().setWidth(widths[i]);
        box->layoutNode().setHeight(50);
        // Note: Anu's Margin(Edge::End, 10) can be used, but let's test if it flips
        box->layoutNode().setMargin(Edge::Left, 10); // If RTL, Left is End? No, Left is physical Left. Let's use Margin(Edge::Right, 10) to see if Anu mirrors it. Actually Anu uses Edge::Start / Edge::End for direction-aware margins. But here we just want to see the items flip.
        rtlRow->addChild(box);
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
