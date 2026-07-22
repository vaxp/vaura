/// @file canvas.cpp
/// @brief Canvas implementation using Skia.

#include "vaura/rendering/canvas.hpp"
#include <include/core/SkCanvas.h>
#include <include/core/SkPaint.h>
#include <include/core/SkPath.h>
#include <include/core/SkRRect.h>
#include <include/core/SkFont.h>
#include <include/core/SkTypeface.h>
#include <include/core/SkTextBlob.h>
#include <include/effects/SkImageFilters.h>

namespace vaura {

namespace {

// Helper to convert vaura::Paint to SkPaint
SkPaint toSkPaint(const Paint& paint) {
    SkPaint sk_paint;
    sk_paint.setColor(paint.getColor());
    sk_paint.setAntiAlias(paint.isAntiAlias());
    sk_paint.setStrokeWidth(paint.getStrokeWidth());

    switch (paint.getStyle()) {
        case PaintStyle::Fill:          sk_paint.setStyle(SkPaint::kFill_Style); break;
        case PaintStyle::Stroke:        sk_paint.setStyle(SkPaint::kStroke_Style); break;
        case PaintStyle::StrokeAndFill: sk_paint.setStyle(SkPaint::kStrokeAndFill_Style); break;
    }

    switch (paint.getStrokeCap()) {
        case StrokeCap::Butt:   sk_paint.setStrokeCap(SkPaint::kButt_Cap); break;
        case StrokeCap::Round:  sk_paint.setStrokeCap(SkPaint::kRound_Cap); break;
        case StrokeCap::Square: sk_paint.setStrokeCap(SkPaint::kSquare_Cap); break;
    }

    switch (paint.getStrokeJoin()) {
        case StrokeJoin::Miter: sk_paint.setStrokeJoin(SkPaint::kMiter_Join); break;
        case StrokeJoin::Round: sk_paint.setStrokeJoin(SkPaint::kRound_Join); break;
        case StrokeJoin::Bevel: sk_paint.setStrokeJoin(SkPaint::kBevel_Join); break;
    }

    if (paint.hasShadow()) {
        sk_paint.setImageFilter(SkImageFilters::DropShadow(
            paint.getShadowDx(), paint.getShadowDy(),
            paint.getShadowBlur(), paint.getShadowBlur(),
            paint.getShadowColor(),
            nullptr, nullptr
        ));
    }

    return sk_paint;
}

// Concrete Skia canvas wrapper
class SkiaCanvas : public Canvas {
public:
    explicit SkiaCanvas(SkCanvas* canvas) : canvas_(canvas) {}

    void save() override { canvas_->save(); }
    void restore() override { canvas_->restore(); }
    
    void saveLayerAlpha(float alpha, const Rect* bounds = nullptr) override {
        SkRect sk_rect;
        if (bounds) {
            sk_rect = SkRect::MakeXYWH(bounds->x, bounds->y, bounds->width, bounds->height);
        }
        
        SkPaint paint;
        paint.setAlphaf(std::clamp(alpha, 0.0f, 1.0f));
        canvas_->saveLayer(bounds ? &sk_rect : nullptr, &paint);
    }
    
    void translate(float dx, float dy) override { canvas_->translate(dx, dy); }
    void scale(float sx, float sy) override { canvas_->scale(sx, sy); }
    void rotate(float degrees) override { canvas_->rotate(degrees); }

    void clipRect(const Rect& rect) override {
        canvas_->clipRect(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height), true);
    }

    void clipRRect(const Rect& rect, const BorderRadius& radius) override {
        SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
        SkVector radii[4] = {
            {radius.top_left, radius.top_left},
            {radius.top_right, radius.top_right},
            {radius.bottom_right, radius.bottom_right},
            {radius.bottom_left, radius.bottom_left}
        };
        SkRRect rrect;
        rrect.setRectRadii(sk_rect, radii);
        canvas_->clipRRect(rrect, true);
    }

    void clear(Color color) override {
        canvas_->clear(color);
    }

    void drawLine(Point p1, Point p2, const Paint& paint) override {
        canvas_->drawLine(p1.x, p1.y, p2.x, p2.y, toSkPaint(paint));
    }

    void drawRect(const Rect& rect, const Paint& paint) override {
        canvas_->drawRect(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height), toSkPaint(paint));
    }

    void drawRRect(const Rect& rect, const BorderRadius& radius, const Paint& paint) override {
        SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
        SkVector radii[4] = {
            {radius.top_left, radius.top_left},
            {radius.top_right, radius.top_right},
            {radius.bottom_right, radius.bottom_right},
            {radius.bottom_left, radius.bottom_left}
        };
        SkRRect rrect;
        rrect.setRectRadii(sk_rect, radii);
        canvas_->drawRRect(rrect, toSkPaint(paint));
    }

    void drawCircle(Point center, float radius, const Paint& paint) override {
        canvas_->drawCircle(center.x, center.y, radius, toSkPaint(paint));
    }

    void drawPath(const Path& path, const Paint& paint) override {
        SkPath* sk_path = static_cast<SkPath*>(path.getNativeHandle());
        if (sk_path) {
            canvas_->drawPath(*sk_path, toSkPaint(paint));
        }
    }
    
    void drawText(std::string_view text, Point pos, const Paint& paint, float font_size) override {
        SkFont font(SkTypeface::MakeDefault(), font_size);
        auto blob = SkTextBlob::MakeFromText(text.data(), text.size(), font, SkTextEncoding::kUTF8);
        canvas_->drawTextBlob(blob, pos.x, pos.y, toSkPaint(paint));
    }

    void drawArc(const Rect& rect, float startAngle, float sweepAngle,
                 bool useCenter, const Paint& paint) override {
        SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
        canvas_->drawArc(sk_rect, startAngle, sweepAngle, useCenter, toSkPaint(paint));
    }

    void drawOval(const Rect& rect, const Paint& paint) override {
        SkRect sk_rect = SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height);
        canvas_->drawOval(sk_rect, toSkPaint(paint));
    }

    void drawPoints(const Point* points, size_t count, const Paint& paint) override {
        std::vector<SkPoint> sk_points(count);
        for (size_t i = 0; i < count; ++i) {
            sk_points[i] = SkPoint::Make(points[i].x, points[i].y);
        }
        canvas_->drawPoints(SkCanvas::kPoints_PointMode,
                            sk_points.size(), sk_points.data(), toSkPaint(paint));
    }

    void* getNativeHandle() override { return canvas_; }

private:
    SkCanvas* canvas_;
};

} // namespace

// Factory function (will be used by the framework surface wrapper)
std::unique_ptr<Canvas> createCanvasWrapper(void* sk_canvas) {
    return std::make_unique<SkiaCanvas>(static_cast<SkCanvas*>(sk_canvas));
}

}  // namespace vaura
