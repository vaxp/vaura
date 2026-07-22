#include "vaura/widgets/image_widget.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/rendering/image.hpp"
#include <layout_engine/Yoga.h>
#include <iostream>

namespace vaura {

class RenderImage : public RenderBox {
public:
    ImageConfig config;
    std::shared_ptr<Image> image_;

    void load_image() {
        auto result = Image::loadFromFile(config.path);
        if (result.isOk()) {
            image_ = result.value();
        } else {
            std::cerr << "[VAURA] Error: Failed to load image from " << config.path << std::endl;
        }
    }

    void paint(PaintContext& ctx) override {
        if (!image_) return;

        Canvas& canvas = ctx.canvas;

        Rect dest_rect{
            ctx.offset.x, ctx.offset.y,
            size().width, size().height
        };

        // Apply border radius clip if needed
        bool needs_clip = config.border_radius > 0.0f;
        if (needs_clip) {
            canvas.save();
            canvas.clipRRect(dest_rect, BorderRadius{config.border_radius, config.border_radius, config.border_radius, config.border_radius});
        }

        // Calculate source rect and destination rect based on BoxFit
        Rect src_rect{0, 0, static_cast<float>(image_->getWidth()), static_cast<float>(image_->getHeight())};
        
        if (config.fit == BoxFit::Fill) {
            canvas.drawImageRect(image_, src_rect, dest_rect);
        } else {
            // Contain or Cover
            float aspect_src = src_rect.width / src_rect.height;
            float aspect_dst = dest_rect.width / dest_rect.height;

            Rect drawn_dest = dest_rect;
            Rect drawn_src = src_rect;

            if (config.fit == BoxFit::Contain) {
                if (aspect_src > aspect_dst) {
                    // Source is wider, scale to fit width
                    float new_height = dest_rect.width / aspect_src;
                    float y_offset = (dest_rect.height - new_height) / 2.0f;
                    drawn_dest = Rect{dest_rect.x, dest_rect.y + y_offset, dest_rect.width, new_height};
                } else {
                    // Source is taller, scale to fit height
                    float new_width = dest_rect.height * aspect_src;
                    float x_offset = (dest_rect.width - new_width) / 2.0f;
                    drawn_dest = Rect{dest_rect.x + x_offset, dest_rect.y, new_width, dest_rect.height};
                }
            } else if (config.fit == BoxFit::Cover) {
                if (aspect_src > aspect_dst) {
                    // Source is wider, crop width
                    float crop_width = src_rect.height * aspect_dst;
                    float x_offset = (src_rect.width - crop_width) / 2.0f;
                    drawn_src = Rect{x_offset, 0, crop_width, src_rect.height};
                } else {
                    // Source is taller, crop height
                    float crop_height = src_rect.width / aspect_dst;
                    float y_offset = (src_rect.height - crop_height) / 2.0f;
                    drawn_src = Rect{0, y_offset, src_rect.width, crop_height};
                }
            }

            canvas.drawImageRect(image_, drawn_src, drawn_dest);
        }

        if (needs_clip) {
            canvas.restore();
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
            YGNodeStyleSetWidth(node, static_cast<float>(box.image_->getWidth()));
            YGNodeStyleSetHeight(node, static_cast<float>(box.image_->getHeight()));
        }
    }

    box.markNeedsLayout();
}

} // namespace vaura
