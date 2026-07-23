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

#include <memory>
#include <cstdio>

using namespace vaura;

// جلب المُغلف الخاص بـ Skia (موجود في canvas.cpp)
namespace vaura {
extern std::unique_ptr<Canvas> createCanvasWrapper(void* sk_canvas);
}

// عنصر مخصص يرث من RenderBox ليرسم مباشرة باستخدام Skia
class CustomColorBox : public RenderBox {
public:
    CustomColorBox(Color color) : color_(color) {}

    void paint(PaintContext& context) override {
        Canvas& canvas = context.canvas;
        Rect bounds = paintBounds(); // الأبعاد المحلية (0,0 إلى width,height)
        
        Paint paint;
        paint.setColor(color_);
        
        canvas.save();
        // نقل مؤشر الرسم إلى موقع العنصر الحقيقي (المحسوب بواسطة Anu)
        canvas.translate(context.offset.x, context.offset.y);
        
        // رسم مربع كامل باللون المختار
        canvas.drawRect(bounds, paint);
        
        // رسم مربع أبيض أصغر داخله لتأكيد نجاح عملية الرسم المباشر
        paint.setColor(Colors::White);
        canvas.drawRect(Rect{bounds.x + 10, bounds.y + 10, bounds.width - 20, bounds.height - 20}, paint);
        
        canvas.restore();
    }

private:
    Color color_;
};

int main() {
    printf("=== Starting Pure Anu Layout Demo ===\n");

    // 1. تهيئة المنصة (SDL)
    auto platform_res = Platform::create();
    if (!platform_res.isOk()) return 1;
    auto platform = std::move(platform_res.value());

    // 2. إنشاء النافذة
    WindowConfig config;
    config.title = "VAURA - Pure Anu Layout & Skia Paint";
    config.width = 800;
    config.height = 600;

    auto window_res = Window::create(*platform, config);
    if (!window_res.isOk()) return 1;
    auto window = std::move(window_res.value());
    window->makeCurrent();

    // 3. تهيئة سياق Skia و OpenGL
    auto interface = GrGLMakeNativeInterface();
    sk_sp<GrDirectContext> context = GrDirectContext::MakeGL(interface);

    sk_sp<SkSurface> surface;
    auto update_surface = [&]() {
        Size s = window->getDrawableSize();
        GrGLFramebufferInfo framebufferInfo;
        framebufferInfo.fFBOID = 0;
        framebufferInfo.fFormat = 0x8058; // GL_RGBA8

        GrBackendRenderTarget backendRenderTarget(
            static_cast<int>(s.width), static_cast<int>(s.height),
            0, 8, framebufferInfo);

        surface = SkSurface::MakeFromBackendRenderTarget(
            context.get(), backendRenderTarget, kBottomLeft_GrSurfaceOrigin,
            kRGBA_8888_SkColorType, nullptr, nullptr);
    };
    update_surface();
    Size current_size = window->getDrawableSize();

    // =========================================================================
    // 4. بناء شجرة التخطيط (Render Tree) باستخدام Anu النقي (بدون Widgets!)
    // =========================================================================

    auto root = std::make_unique<RenderFlex>();
    // استخدام دوال Anu الأصلية
    root->layoutNode().setWidthPercent(100);
    root->layoutNode().setHeightPercent(100);
    root->layoutNode().setFlexDirection(FlexDirection::Row); // عرض أفقي
    root->layoutNode().setJustifyContent(MainAxisAlign::SpaceEvenly);
    root->layoutNode().setAlignItems(CrossAxisAlign::Center);

    // الصندوق الأول (أحمر)
    auto box1 = std::make_unique<CustomColorBox>(Colors::Red);
    box1->layoutNode().setWidth(150);
    box1->layoutNode().setHeight(150);

    // الصندوق الثاني (أزرق - أكبر قليلاً)
    auto box2 = std::make_unique<CustomColorBox>(Colors::Blue);
    box2->layoutNode().setWidth(200);
    box2->layoutNode().setHeight(300);
    
    // الصندوق الثالث (أخضر)
    auto box3 = std::make_unique<CustomColorBox>(Colors::Green);
    box3->layoutNode().setWidth(150);
    box3->layoutNode().setHeight(150);

    // إضافة الأبناء إلى الجذر (Anu سيتولى ربط العقد داخلياً)
    root->addChild(box1.get());
    root->addChild(box2.get());
    root->addChild(box3.get());

    // =========================================================================

    bool running = true;
    platform->onQuit().connect([&]() { running = false; });

    // 5. حلقة المعالجة الرئيسية
    while (running) {
        if (!platform->pollEvents()) break;

        Size new_size = window->getDrawableSize();
        if (new_size.width != current_size.width || new_size.height != current_size.height) {
            current_size = new_size;
            update_surface();
        }

        // --- أ. مرحلة التخطيط (Layout Phase) ---
        // استدعاء جذري لـ Anu ليحسب كل الشجرة، ثم نسخ النتائج (sync)
        Size win_size = window->getSize();
        root->layout(win_size.width, win_size.height);

        // --- ب. مرحلة الرسم (Paint Phase) ---
        if (surface) {
            auto* sk_canvas = surface->getCanvas();
            auto canvas_wrapper = createCanvasWrapper(sk_canvas);
            
            // خلفية رمادية فاتحة
            canvas_wrapper->clear(0xFFEEEEEE);

            PaintContext paint_ctx {
                *canvas_wrapper,
                {0, 0}, // الأوفست الابتدائي
                Rect{0, 0, win_size.width, win_size.height},
                1.0f
            };

            // رسم الشجرة بناءً على المواقع التي حسبها Anu
            root->paint(paint_ctx);

            context->flush();
            window->swapBuffers();
        }
    }
    return 0;
}
