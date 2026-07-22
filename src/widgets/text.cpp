/// @file text.cpp
/// @brief Widget: text implementation (Phase 7).
#include "vaura/widgets/text.hpp"
#include "vaura/rendering/canvas.hpp"
#include <layout_engine/Yoga.h>
#include <modules/skparagraph/include/Paragraph.h>
#include <modules/skparagraph/include/ParagraphBuilder.h>
#include <modules/skparagraph/include/FontCollection.h>
#include <modules/skparagraph/include/TextStyle.h>
#include <include/core/SkFontMgr.h>
#include "vaura/rendering/font_manager.hpp"

namespace vaura {

class RenderText : public RenderBox {
public:
    RenderText(std::string text, TextConfig config) 
        : text_(std::move(text)), config_(std::move(config)) {
        
        // Setup Yoga measure function
        YGNodeSetContext(yogaNode(), this);
        YGNodeSetMeasureFunc(yogaNode(), [](YGNodeConstRef node, float width, YGMeasureMode widthMode, float height, YGMeasureMode heightMode) -> YGSize {
            auto* self = static_cast<RenderText*>(YGNodeGetContext(node));
            float layout_width = 10000.0f;
            if (widthMode == YGMeasureModeExactly || widthMode == YGMeasureModeAtMost) {
                layout_width = width;
            }
            
            self->buildParagraph(layout_width);
            
            float final_width = static_cast<float>(self->paragraph_->getMaxIntrinsicWidth());
            if (widthMode == YGMeasureModeExactly) {
                final_width = width;
            } else if (widthMode == YGMeasureModeAtMost) {
                final_width = std::min(width, final_width);
            }

            return YGSize{
                final_width,
                static_cast<float>(self->paragraph_->getHeight())
            };
        });
    }

    void setText(const std::string& text, const TextConfig& config) {
        if (text_ != text || config_.font_size != config.font_size || 
            config_.color != config.color || config_.weight != config.weight ||
            config_.align != config.align) {
            text_ = text;
            config_ = config;
            paragraph_.reset(); // Force rebuild
            markNeedsLayout();
        }
    }

    void paint(PaintContext& context) override {
        if (!paragraph_) {
            buildParagraph(size().width);
        }
        
        // We already have size() from layout, layout paragraph one more time to fit
        paragraph_->layout(size().width);
        
        // Render
        SkCanvas* canvas = static_cast<SkCanvas*>(context.canvas.getNativeHandle());
        paragraph_->paint(canvas, context.offset.x, context.offset.y);
    }

private:
    void buildParagraph(float width_constraint) {
        skia::textlayout::TextStyle style;
        
        // Default fonts for VAURA
        style.setFontFamilies({SkString(config_.font_family.c_str()), 
                               SkString("Noto Sans Arabic"), 
                               SkString("sans-serif")});
        
        style.setFontSize(config_.font_size);
        
        // Color with PaintContext opacity applied? We handle color opacity manually for now
        style.setColor(config_.color);

        // Font Weight mapping
        SkFontStyle::Weight sk_weight = static_cast<SkFontStyle::Weight>(config_.weight);
        style.setFontStyle(SkFontStyle(sk_weight, SkFontStyle::kNormal_Width, 
            config_.style == TextStyle::Italic ? SkFontStyle::kItalic_Slant : SkFontStyle::kUpright_Slant));

        skia::textlayout::ParagraphStyle paraStyle;
        paraStyle.setTextStyle(style);

        // Alignment and RTL
        // If the text contains Arabic characters, we should probably set RTL. 
        // For now, we support kRtl default for Arabic test, or we let Skia auto-detect using Bidi.
        // Actually, SkParagraph handles bidi automatically if text direction is LTR or RTL.
        // Let's check alignment
        if (config_.align == TextStyle::Right) paraStyle.setTextAlign(skia::textlayout::TextAlign::kRight);
        else if (config_.align == TextStyle::Center) paraStyle.setTextAlign(skia::textlayout::TextAlign::kCenter);
        else if (config_.align == TextStyle::Justify) paraStyle.setTextAlign(skia::textlayout::TextAlign::kJustify);
        else paraStyle.setTextAlign(skia::textlayout::TextAlign::kLeft);

        if (config_.max_lines > 0) {
            paraStyle.setMaxLines(config_.max_lines);
            if (config_.overflow == TextStyle::Ellipsis) {
                paraStyle.setEllipsis(u"\u2026");
            }
        }

        auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, FontManager::getFontCollection());
        builder->addText(text_.c_str(), text_.length());
        paragraph_ = builder->Build();
        
        // If width_constraint is NaN or infinite, SkParagraph uses unconstrained width.
        float layout_width = std::isfinite(width_constraint) ? width_constraint : 10000.0f;
        paragraph_->layout(layout_width);
    }

    std::string text_;
    TextConfig config_;
    std::unique_ptr<skia::textlayout::Paragraph> paragraph_;
};

std::unique_ptr<RenderObject> TextWidget::createRenderObject(BuildContext& ctx) {
    return std::make_unique<RenderText>(content, config);
}

void TextWidget::updateRenderObject(BuildContext& ctx, RenderObject& renderObject) {
    auto& render_text = static_cast<RenderText&>(renderObject);
    render_text.setText(content, config);
}

} // namespace vaura
