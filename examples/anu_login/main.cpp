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
    WindowConfig config{"VAURA - Anu Login Demo", 800, 600};
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

    // Root - Centered
    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF1E1E1E)));
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setJustifyContent(MainAxisAlign::Center);
    root->layoutNode().setAlignItems(CrossAxisAlign::Center);

    // Login Card
    auto card = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF2D2D30, 20)));
    card->layoutNode().setWidth(400);
    card->layoutNode().setHeight(500);
    card->layoutNode().setFlexDirection(FlexDirection::Column);
    card->layoutNode().setPadding(Edge::All, 40);
    card->layoutNode().setAlignItems(CrossAxisAlign::Center);
    root->addChild(card);

    // Logo Placeholder
    auto logo = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF007ACC, 40)));
    logo->layoutNode().setWidth(80);
    logo->layoutNode().setHeight(80);
    logo->layoutNode().setMargin(Edge::Bottom, 40);
    card->addChild(logo);

    // Username Field
    auto userField = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF1E1E1E, 10)));
    userField->layoutNode().setWidthPercent(100);
    userField->layoutNode().setHeight(50);
    userField->layoutNode().setMargin(Edge::Bottom, 20);
    card->addChild(userField);

    // Password Field
    auto passField = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF1E1E1E, 10)));
    passField->layoutNode().setWidthPercent(100);
    passField->layoutNode().setHeight(50);
    passField->layoutNode().setMargin(Edge::Bottom, 40);
    card->addChild(passField);

    // Login Button
    auto loginBtn = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF007ACC, 10)));
    loginBtn->layoutNode().setWidthPercent(100);
    loginBtn->layoutNode().setHeight(50);
    card->addChild(loginBtn);

    // Links Row
    auto linksRow = static_cast<RenderDecoratedBox*>(addNode(createBox(0x00000000)));
    linksRow->layoutNode().setFlexDirection(FlexDirection::Row);
    linksRow->layoutNode().setJustifyContent(MainAxisAlign::SpaceBetween);
    linksRow->layoutNode().setWidthPercent(100);
    linksRow->layoutNode().setMargin(Edge::Top, 30);
    card->addChild(linksRow);

    auto forgotLink = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF555555, 5)));
    forgotLink->layoutNode().setWidth(100);
    forgotLink->layoutNode().setHeight(15);
    linksRow->addChild(forgotLink);

    auto signupLink = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF555555, 5)));
    signupLink->layoutNode().setWidth(80);
    signupLink->layoutNode().setHeight(15);
    linksRow->addChild(signupLink);

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
