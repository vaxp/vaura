#include "vaura/widgets/image_widget.hpp"
#include "vaura/rendering/canvas.hpp"
#include "include/core/SkCanvas.h"
#include "include/core/SkImage.h"
#include "include/core/SkData.h"
#include "include/core/SkRect.h"
#include "include/core/SkRRect.h"
#include "include/core/SkPaint.h"
#include <layout_engine/Yoga.h>
#include <iostream>

namespace vaura {

class RenderImage : public RenderBox {
public:
    ImageConfig config;
    sk_sp<SkImage> image_;

    void load_image() {
        auto data = SkData::MakeFromFileName(config.path.c_str());
        if (data) {
            image_ = SkImage::MakeFromEncoded(data);
        } else {
            std::cerr << "[VAURA] Error: Failed to load image from " << config.path << std::endl;
        }
    }

    void paint(PaintContext& ctx) override {
        if (!image_) return;

        SkCanvas* canvas = static_cast<SkCanvas*>(ctx.canvas.getNativeHandle());

        SkRect dest_rect = SkRect::MakeXYWH(
            ctx.offset.x, ctx.offset.y,
            size().width, size().height
        );

        // Apply border radius clip if needed
        bool needs_clip = config.border_radius > 0.0f;
        if (needs_clip) {
            canvas->save();
            SkRRect rrect = SkRRect::MakeRectXY(dest_rect, config.border_radius, config.border_radius);
            canvas->clipRRect(rrect, true);
        }

        // Calculate source rect and destination rect based on BoxFit
        SkRect src_rect = SkRect::MakeWH(image_->width(), image_->height());
        
        if (config.fit == BoxFit::Fill) {
            canvas->drawImageRect(image_, src_rect, dest_rect, SkSamplingOptions(SkFilterMode::kLinear), nullptr, SkCanvas::kFast_SrcRectConstraint);
        } else {
            // Contain or Cover
            float aspect_src = src_rect.width() / src_rect.height();
            float aspect_dst = dest_rect.width() / dest_rect.height();

            SkRect drawn_dest = dest_rect;
            SkRect drawn_src = src_rect;

            if (config.fit == BoxFit::Contain) {
                if (aspect_src > aspect_dst) {
                    // Source is wider, scale to fit width
                    float new_height = dest_rect.width() / aspect_src;
                    float y_offset = (dest_rect.height() - new_height) / 2.0f;
                    drawn_dest = SkRect::MakeXYWH(dest_rect.x(), dest_rect.y() + y_offset, dest_rect.width(), new_height);
                } else {
                    // Source is taller, scale to fit height
                    float new_width = dest_rect.height() * aspect_src;
                    float x_offset = (dest_rect.width() - new_width) / 2.0f;
                    drawn_dest = SkRect::MakeXYWH(dest_rect.x() + x_offset, dest_rect.y(), new_width, dest_rect.height());
                }
            } else if (config.fit == BoxFit::Cover) {
                if (aspect_src > aspect_dst) {
                    // Source is wider, crop width
                    float crop_width = src_rect.height() * aspect_dst;
                    float x_offset = (src_rect.width() - crop_width) / 2.0f;
                    drawn_src = SkRect::MakeXYWH(x_offset, 0, crop_width, src_rect.height());
                } else {
                    // Source is taller, crop height
                    float crop_height = src_rect.width() / aspect_dst;
                    float y_offset = (src_rect.height() - crop_height) / 2.0f;
                    drawn_src = SkRect::MakeXYWH(0, y_offset, src_rect.width(), crop_height);
                }
            }

            canvas->drawImageRect(image_, drawn_src, drawn_dest, SkSamplingOptions(SkFilterMode::kLinear), nullptr, SkCanvas::kFast_SrcRectConstraint);
        }

        if (needs_clip) {
            canvas->restore();
        }
    }
};

std::unique_ptr<RenderObject> ImageWidget::createRenderObject(BuildContext& ctx) {
    auto ro = std::make_unique<RenderImage>();
    updateRenderObject(ctx, *ro);
    ro->load_image();
    return ro;
}

void ImageWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& box = static_cast<RenderImage&>(renderObject);
    
    // Reload image if path changed
    if (box.config.path != config.path) {
        box.config = config;
        box.load_image();
    } else {
        box.config = config;
    }

    auto node = box.yogaNode();

    // Apply sizing
    if (config.width.has_value()) {
        YGNodeStyleSetWidth(node, config.width.value());
    } else if (config.width_percent.has_value()) {
        YGNodeStyleSetWidthPercent(node, config.width_percent.value());
    } else {
        YGNodeStyleSetWidth(node, YGUndefined);
    }

    if (config.height.has_value()) {
        YGNodeStyleSetHeight(node, config.height.value());
    } else if (config.height_percent.has_value()) {
        YGNodeStyleSetHeightPercent(node, config.height_percent.value());
    } else {
        YGNodeStyleSetHeight(node, YGUndefined);
    }

    // Default to image intrinsic size when no explicit size was provided.
    if (!config.width.has_value() && !config.height.has_value() &&
        !config.width_percent.has_value() && !config.height_percent.has_value()) {
        if (box.image_) {
            YGNodeStyleSetWidth(node, static_cast<float>(box.image_->width()));
            YGNodeStyleSetHeight(node, static_cast<float>(box.image_->height()));
        }
    }

    box.markNeedsLayout();
}

} // namespace vaura
