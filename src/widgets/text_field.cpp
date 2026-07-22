/// @file text_field.cpp
/// @brief TextFieldWidget implementation.

#include "vaura/widgets/text_field.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/platform/platform.hpp"

#include <SDL2/SDL.h>

namespace vaura {

class TextFieldState;
static TextFieldState* g_active_text_field = nullptr;

class TextFieldState : public State {
public:
    std::string text_;
    bool is_focused_ = false;
    SlotId text_input_slot_ = InvalidSlotId;
    SlotId key_down_slot_ = InvalidSlotId;

    void initState() override {
        State::initState();
        auto* tf = static_cast<const TextFieldWidget*>(widget());
        text_ = tf->config.initial_text;
    }

    void dispose() override {
        unfocus();
        State::dispose();
    }

    void focus() {
        if (is_focused_) return;
        
        if (g_active_text_field && g_active_text_field != this) {
            g_active_text_field->unfocus();
        }
        g_active_text_field = this;

        is_focused_ = true;
        if (auto* p = Platform::instance()) {
            p->startTextInput();
            text_input_slot_ = p->onTextInput().connect([this](std::string_view t) {
                text_ += t;
                auto* tf = static_cast<const TextFieldWidget*>(widget());
                if (tf->config.on_changed) tf->config.on_changed(text_);
                setState([]{});
            });
            key_down_slot_ = p->onKeyDown().connect([this](int keycode) {
                if (keycode == SDLK_BACKSPACE) {
                    if (!text_.empty()) {
                        // Handle UTF-8 simple backspace (removes last codepoint)
                        while (!text_.empty() && (text_.back() & 0xC0) == 0x80) {
                            text_.pop_back();
                        }
                        if (!text_.empty()) {
                            text_.pop_back();
                        }
                        auto* tf = static_cast<const TextFieldWidget*>(widget());
                        if (tf->config.on_changed) tf->config.on_changed(text_);
                        setState([]{});
                    }
                } else if (keycode == SDLK_RETURN) {
                    auto* tf = static_cast<const TextFieldWidget*>(widget());
                    if (tf->config.on_submitted) tf->config.on_submitted(text_);
                }
            });
        }
        setState([]{});
    }

    void unfocus() {
        if (!is_focused_) return;
        is_focused_ = false;
        
        if (g_active_text_field == this) {
            g_active_text_field = nullptr;
        }

        if (auto* p = Platform::instance()) {
            p->stopTextInput();
            p->onTextInput().disconnect(text_input_slot_);
            p->onKeyDown().disconnect(key_down_slot_);
        }
        setState([]{});
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* tf = static_cast<const TextFieldWidget*>(widget());

        // Background and borders
        Color border_color = is_focused_ ? tf->config.cursor_color : 0x00000000;
        float border_width = is_focused_ ? 2.0f : 0.0f;

        // Display text
        std::string display_text = text_;
        Color current_text_color = tf->config.text_color;
        
        if (display_text.empty()) {
            display_text = tf->config.placeholder;
            current_text_color = tf->config.placeholder_color;
        }

        // Add a fake cursor if focused
        if (is_focused_) {
            display_text += "|";
        }

        auto content = std::make_shared<FlexBox>();
        if (tf->config.width > 0) content->width(tf->config.width);
        if (tf->config.width_percent.has_value()) content->widthPercent(tf->config.width_percent.value());
        content->height(tf->config.height)
            .backgroundColor(tf->config.bg_color)
            .borderRadius(tf->config.border_radius)
            .padding(YGEdgeLeft, tf->config.padding.left)
            .padding(YGEdgeRight, tf->config.padding.right)
            .padding(YGEdgeTop, tf->config.padding.top)
            .padding(YGEdgeBottom, tf->config.padding.bottom)
            .justifyContent(YGJustifyCenter); // Center vertically

        // To show focus, we can slightly lighten the background color or rely on cursor.
        // We will just keep the original bg_color, and maybe draw a border in the future.
        if (is_focused_) {
            content->backgroundColor(0xFF3B4A60); // Slightly lighter for focus state
        } else {
            content->backgroundColor(tf->config.bg_color);
        }

        content->child(text(display_text, {
            .font_size = tf->config.font_size,
            .color = current_text_color,
        }));

        // Wrap in GestureDetector to capture focus
        return gesture_detector({
            .child = std::move(content),
            .on_tap_down = [this]() {
                focus();
            }
        });
    }
};

std::unique_ptr<State> TextFieldWidget::createState() {
    return std::make_unique<TextFieldState>();
}

} // namespace vaura
