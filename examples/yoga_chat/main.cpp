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
    WindowConfig config{"VAURA - Yoga Chat Demo", 400, 700};
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
    auto root = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFFECEFF1)));
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setFlexDirection(FlexDirection::Column);

    // Header
    auto header = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF00796B)));
    header->layoutNode().setWidthPercent(100);
    header->layoutNode().setHeight(80);
    header->layoutNode().setFlexDirection(FlexDirection::Row);
    header->layoutNode().setAlignItems(CrossAxisAlign::Center);
    header->layoutNode().setPadding(Edge::Horizontal, 20);
    header->layoutNode().setPadding(Edge::Top, 20); // simulate status bar
    root->addChild(header);

    auto backBtn = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFFFFFFFF, 15)));
    backBtn->layoutNode().setWidth(30);
    backBtn->layoutNode().setHeight(30);
    backBtn->layoutNode().setMargin(Edge::Right, 15);
    header->addChild(backBtn);

    auto contactName = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFFFFFFFF, 5)));
    contactName->layoutNode().setWidth(150);
    contactName->layoutNode().setHeight(15);
    header->addChild(contactName);

    // Messages Area (FlexGrow: 1, pushes input to bottom)
    auto msgArea = static_cast<RenderDecoratedBox*>(addNode(createBox(0x00000000)));
    msgArea->layoutNode().setFlexGrow(1.0f);
    msgArea->layoutNode().setFlexDirection(FlexDirection::Column);
    msgArea->layoutNode().setJustifyContent(MainAxisAlign::End); // anchor messages to bottom
    msgArea->layoutNode().setPadding(Edge::All, 15);
    root->addChild(msgArea);

    bool isMe[] = {false, true, false, false, true, true};
    int widths[] = {200, 150, 250, 100, 180, 80};

    for (int i = 0; i < 6; i++) {
        // Bubble
        auto bubble = static_cast<RenderDecoratedBox*>(addNode(createBox(isMe[i] ? 0xFF00897B : 0xFFFFFFFF, 15)));
        bubble->layoutNode().setWidth(widths[i]);
        bubble->layoutNode().setHeight(40);
        bubble->layoutNode().setMargin(Edge::Bottom, 10);
        // Align Self to Left or Right depending on sender
        bubble->layoutNode().setAlignSelf(isMe[i] ? CrossAxisAlign::End : CrossAxisAlign::Start);
        
        // Special border radius logic (Yoga doesn't handle border radius, we do it in VAURA)
        RenderDecoratedBox::Decoration dec = bubble->decoration();
        if (isMe[i]) {
            dec.border_radius.bottom_right = 0;
        } else {
            dec.border_radius.bottom_left = 0;
        }
        bubble->setDecoration(dec);

        msgArea->addChild(bubble);
    }

    // Input Area
    auto inputArea = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFFFFFFFF)));
    inputArea->layoutNode().setWidthPercent(100);
    inputArea->layoutNode().setHeight(60);
    inputArea->layoutNode().setFlexDirection(FlexDirection::Row);
    inputArea->layoutNode().setAlignItems(CrossAxisAlign::Center);
    inputArea->layoutNode().setPadding(Edge::Horizontal, 15);
    root->addChild(inputArea);

    auto textField = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFFECEFF1, 20)));
    textField->layoutNode().setFlexGrow(1.0f);
    textField->layoutNode().setHeight(40);
    inputArea->addChild(textField);

    auto sendBtn = static_cast<RenderDecoratedBox*>(addNode(createBox(0xFF00897B, 20)));
    sendBtn->layoutNode().setWidth(40);
    sendBtn->layoutNode().setHeight(40);
    sendBtn->layoutNode().setMargin(Edge::Left, 10);
    inputArea->addChild(sendBtn);

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
