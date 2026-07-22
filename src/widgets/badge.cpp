#include "vaura/widgets/badge.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/app/app.hpp"
#include "vaura/rendering/font_manager.hpp"
#include <modules/skparagraph/include/Paragraph.h>
#include <modules/skparagraph/include/ParagraphBuilder.h>
#include <modules/skparagraph/include/TextStyle.h>
#include <layout_engine/Yoga.h>
#include <cmath>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// RenderBadge
// ════════════════════════════════════════════════════════════════

class RenderBadge : public RenderBox {
public:
    BadgeConfig config;
    std::unique_ptr<skia::textlayout::Paragraph> text_paragraph;

    explicit RenderBadge(BadgeConfig config) : config(std::move(config)) {}

    void setConfig(const BadgeConfig& new_config) {
        bool needs_layout = false;
        
        if (config.text != new_config.text || 
            config.font_size != new_config.font_size ||
            config.padding != new_config.padding) {
            needs_layout = true;
            text_paragraph.reset(); // Rebuild text
        }
        
        config = new_config;

        if (needs_layout) {
            markNeedsLayout();
        } else {
            markNeedsPaint();
        }
    }

    void buildText() {
        if (!text_paragraph) {
            skia::textlayout::ParagraphStyle para_style;
            skia::textlayout::TextStyle text_style;
            text_style.setColor(config.text_color);
            text_style.setFontSize(config.font_size);
            
            auto builder = skia::textlayout::ParagraphBuilder::make(para_style, FontManager::getFontCollection());
            builder->pushStyle(text_style);
            builder->addText(config.text.c_str(), config.text.length());
            
            text_paragraph = builder->Build();
            text_paragraph->layout(1000.0f); // Unconstrained width
        }
    }



    void paint(PaintContext& context) override {
        // Paint child first
        if (!children_.empty()) {
            auto* child = children_.front();
            PaintContext child_ctx = context.withOffset(child->offset());
            child->paint(child_ctx);
        }

        if (config.text.empty()) return;

        buildText();

        float text_w = text_paragraph->getMaxIntrinsicWidth();
        float text_h = text_paragraph->getHeight();
        
        float badge_size = std::max(text_w, text_h) + config.padding * 2.0f;
        
        // Position top-right
        float bx = context.offset.x + size_.width - badge_size / 2.0f + config.offset.x;
        float by = context.offset.y - badge_size / 2.0f + config.offset.y;
        
        Rect badge_rect{bx, by, std::max(badge_size, text_w + config.padding * 2), badge_size};

        // Draw red background
        Paint bg_paint;
        bg_paint.setColor(config.background_color);
        bg_paint.setStyle(PaintStyle::Fill);
        
        auto& canvas = context.canvas;
        canvas.drawRRect(badge_rect, BorderRadius::circular(badge_size / 2.0f), bg_paint);
        
        // Draw text centered
        float tx = bx + (badge_rect.width - text_w) / 2.0f;
        float ty = by + (badge_rect.height - text_h) / 2.0f;
        text_paragraph->paint(static_cast<SkCanvas*>(context.canvas.getNativeHandle()), tx, ty);
    }
};

// ════════════════════════════════════════════════════════════════
// BadgeWidget
// ════════════════════════════════════════════════════════════════

class BadgeWidget : public SingleChildRenderObjectWidget {
public:
    BadgeConfig config;

    explicit BadgeWidget(BadgeConfig config) : config(std::move(config)) {
        this->child = this->config.child;
    }

    [[nodiscard]] std::string_view typeName() const override { return "BadgeWidget"; }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override {
        auto obj = std::make_unique<RenderBadge>(config);
        return obj;
    }

    void updateRenderObject(BuildContext& ctx, RenderObject& render_object) override {
        auto& ro = static_cast<RenderBadge&>(render_object);
        ro.setConfig(config);
    }
};

std::shared_ptr<Widget> badge(BadgeConfig config) {
    return std::make_shared<BadgeWidget>(std::move(config));
}

} // namespace vaura
