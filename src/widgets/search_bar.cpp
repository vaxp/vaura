/// @file search_bar.cpp
/// @brief SearchBar widget implementation.
///
/// Fix: SearchBar now acts as a proper interactive text field.
///   - Clicking it requests keyboard focus via Platform::startTextInput().
///   - It connects to onTextInput() and onKeyDown() signals for real typing.
///   - Clicking away (backdrop) unfocuses and disconnects signals.
///   - Suggestions panel is shown via OverlayController so it floats above
///     other content (no more clipping inside parent containers).

#include "vaura/widgets/search_bar.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/tree/element.hpp"
#include "vaura/tree/render_object.hpp"
#include "vaura/platform/platform.hpp"
#include "vaura/overlay/overlay.hpp"
#ifdef VAURA_HAS_SDL3
#include <SDL3/SDL.h>
#elif defined(VAURA_HAS_SDL2)
#include <SDL2/SDL.h>
#endif
#include <layout_engine/Anu.h>
#include <string>

namespace vaura {

// ── Global active search bar (only one can be focused at a time) ─
class SearchBarState;
static SearchBarState* g_active_search_bar = nullptr;

class SearchBarWidget : public StatefulWidget {
public:
    SearchBarConfig config;
    explicit SearchBarWidget(SearchBarConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "SearchBar"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class SearchBarState : public State {
public:
    bool        is_focused_   = false;
    std::string typed_value_  = "";
    int         hovered_sugg_ = -1;

    SlotId text_input_slot_ = InvalidSlotId;
    SlotId key_down_slot_   = InvalidSlotId;

    // Suggestion overlay
    bool      has_overlay_ = false;
    OverlayId overlay_id_  = 0;
    float     field_x_     = 0.0f;
    float     field_y_     = 0.0f;

    void initState() override {
        State::initState();
        auto* sw = static_cast<const SearchBarWidget*>(widget());
        typed_value_ = sw->config.value;
    }

    void dispose() override {
        unfocus();
        removeSuggOverlay();
        State::dispose();
    }

    // ── Focus management ─────────────────────────────────────────

    void focus() {
        if (is_focused_) return;

        // Defocus any previously active search bar
        if (g_active_search_bar && g_active_search_bar != this) {
            g_active_search_bar->unfocus();
        }
        g_active_search_bar = this;
        is_focused_ = true;

        auto* p = Platform::instance();
        if (!p) { setState([]{}); return; }

        p->startTextInput();

        text_input_slot_ = p->onTextInput().connect([this](std::string_view t) {
            typed_value_ += t;
            auto* sw = static_cast<const SearchBarWidget*>(widget());
            if (sw && sw->config.on_changed) sw->config.on_changed(typed_value_);
            refreshSuggOverlay();
            setState([]{});
        });

        key_down_slot_ = p->onKeyDown().connect([this](int keycode) {
            if (keycode == SDLK_BACKSPACE) {
                if (!typed_value_.empty()) {
                    // UTF-8 safe pop
                    while (!typed_value_.empty() && (typed_value_.back() & 0xC0) == 0x80)
                        typed_value_.pop_back();
                    if (!typed_value_.empty())
                        typed_value_.pop_back();
                    auto* sw = static_cast<const SearchBarWidget*>(widget());
                    if (sw && sw->config.on_changed) sw->config.on_changed(typed_value_);
                    refreshSuggOverlay();
                    setState([]{});
                }
            } else if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER) {
                auto* sw = static_cast<const SearchBarWidget*>(widget());
                if (sw && sw->config.on_submitted) sw->config.on_submitted(typed_value_);
                unfocus();
            } else if (keycode == SDLK_ESCAPE) {
                unfocus();
            }
        });

        setState([]{});
    }

    void unfocus() {
        if (!is_focused_) return;
        is_focused_ = false;
        hovered_sugg_ = -1;

        if (g_active_search_bar == this) g_active_search_bar = nullptr;

        auto* p = Platform::instance();
        if (p) {
            p->stopTextInput();
            p->onTextInput().disconnect(text_input_slot_);
            p->onKeyDown().disconnect(key_down_slot_);
        }
        text_input_slot_ = InvalidSlotId;
        key_down_slot_   = InvalidSlotId;

        removeSuggOverlay();
        setState([]{});
    }

    // ── Suggestion overlay ───────────────────────────────────────

    WidgetPtr buildSuggOverlay(const SearchBarConfig& cfg) {
        // Invisible full-screen backdrop to close on outside click
        auto bd = std::make_shared<FlexBox>();
        bd->positionType(PositionType::Absolute)
           .position(Edge::All, 0.0f)
           .widthPercent(100).heightPercent(100);
        auto bd_gd = gesture_detector({
            .child  = bd,
            .on_tap = [this]() { unfocus(); },
        });

        // Suggestion list
        auto sugg_col = std::make_shared<FlexBox>();
        sugg_col->direction(FlexDirection::Column)
                 .width(cfg.width)
                 .padding(Edge::Vertical, 4);

        int idx = 0;
        for (const auto& sugg : cfg.suggestions) {
            if (sugg.find(typed_value_) == std::string::npos &&
                !typed_value_.empty()) {
                ++idx; continue; // basic filter
            }
            bool hov = (hovered_sugg_ == idx);
            auto sugg_row = std::make_shared<FlexBox>();
            sugg_row->height(38)
                     .padding(Edge::Left, 16).padding(Edge::Right, 12)
                     .align(Align::Center)
                     .backgroundColor(hov ? cfg.suggestion_hover : 0x00000000)
                     .child(text(sugg, {.font_size = cfg.font_size, .color = cfg.text_color}));

            int  ci = idx;
            std::string sv = sugg;
            auto fn_sugg = cfg.on_suggestion_tap;
            auto fn_ch   = cfg.on_changed;
            auto s_gd = gesture_detector({
                .child          = sugg_row,
                .on_tap         = [this, sv, fn_sugg, fn_ch]() {
                    typed_value_ = sv;
                    if (fn_sugg) fn_sugg(sv);
                    else if (fn_ch) fn_ch(sv);
                    unfocus();
                },
                .on_hover_enter = [this, ci, cfg]() { hovered_sugg_ = ci; refreshSuggOverlay(); },
                .on_hover_exit  = [this, cfg]()     { hovered_sugg_ = -1; refreshSuggOverlay(); },
            });
            sugg_col->child(s_gd);
            ++idx;
        }

        auto sugg_panel = container({
            .color         = cfg.background_color,
            .width         = cfg.width,
            .border_radius = BorderRadius::circular(8.0f),
            .border        = Border{cfg.border_color, 1.0f},
            .shadow        = Shadow{0x50000000, 16.0f, {0, 4}},
            .child         = sugg_col,
        });

        // Position below the field using stored screen coordinates
        auto panel_wrap = std::make_shared<FlexBox>();
        panel_wrap->positionType(PositionType::Absolute)
                   .position(Edge::Left, field_x_)
                   .position(Edge::Top,  field_y_ + cfg.height + 4.0f)
                   .width(cfg.width);
        panel_wrap->child(sugg_panel);

        auto overlay_root = std::make_shared<FlexBox>();
        overlay_root->widthPercent(100).heightPercent(100);
        overlay_root->child(bd_gd).child(panel_wrap);
        return overlay_root;
    }

    void insertSuggOverlay(const SearchBarConfig& cfg) {
        if (has_overlay_) return;
        overlay_id_ = OverlayController::instance().insert({
            .content         = buildSuggOverlay(cfg),
            .absorbs_pointer = true,
        });
        has_overlay_ = true;
    }

    void refreshSuggOverlay() {
        if (!has_overlay_) return;
        auto* sw = static_cast<const SearchBarWidget*>(widget());
        if (!sw) return;
        OverlayController::instance().remove(overlay_id_);
        overlay_id_ = OverlayController::instance().insert({
            .content         = buildSuggOverlay(sw->config),
            .absorbs_pointer = true,
        });
    }

    void removeSuggOverlay() {
        if (has_overlay_) {
            OverlayController::instance().remove(overlay_id_);
            has_overlay_ = false;
            overlay_id_  = 0;
        }
    }

    // ── Build ────────────────────────────────────────────────────

    WidgetPtr build(BuildContext& ctx) override {
        auto* sw = static_cast<const SearchBarWidget*>(widget());
        const auto& cfg = sw->config;

        // Sync external value only when not focused (don't override user typing)
        if (!is_focused_) {
            typed_value_ = cfg.value;
        }

        bool has_text = !typed_value_.empty();
        Color border  = is_focused_ ? cfg.focus_color : cfg.border_color;

        // ── Search icon ─────────────────────────────────────────
        auto search_icon_wrap = std::make_shared<FlexBox>();
        search_icon_wrap->width(20).height(20)
                         .margin(Edge::Right, 8)
                         .justify(Justify::Center)
                         .align(Align::Center)
                         .child(icon(Icons::Search, {.size = 17.0f, .color = cfg.icon_color}));

        // ── Text / cursor display ────────────────────────────────
        std::string display = has_text ? typed_value_ : cfg.placeholder;
        if (is_focused_) display += "|"; // blinking cursor simulation
        Color txt_col = has_text ? cfg.text_color : cfg.placeholder_color;

        auto value_text = text(display, {.font_size = cfg.font_size, .color = txt_col});
        auto text_wrap  = std::make_shared<FlexBox>();
        text_wrap->flexGrow(1.0f).padding(Edge::Vertical, 4).child(value_text);

        // ── Field row ────────────────────────────────────────────
        auto field_row = std::make_shared<FlexBox>();
        field_row->direction(FlexDirection::Row)
                  .align(Align::Center)
                  .width(cfg.width).height(cfg.height)
                  .padding(Edge::Left, 12)
                  .padding(Edge::Right, has_text ? 4.0f : 12.0f);
        field_row->child(search_icon_wrap).child(text_wrap);

        // ── Clear button ─────────────────────────────────────────
        if (has_text && cfg.on_changed) {
            auto fn = cfg.on_changed;
            auto clear_btn = gesture_detector({
                .child  = container({
                    .padding = EdgeInsets::all(6),
                    .child   = icon(Icons::Xmark, {.size = 14.0f, .color = cfg.clear_color}),
                }),
                .on_tap = [this, fn]() {
                    typed_value_ = "";
                    fn("");
                    removeSuggOverlay();
                    setState([]{});
                },
            });
            field_row->child(clear_btn);
        }

        auto field_box = container({
            .color         = cfg.background_color,
            .width         = cfg.width,
            .height        = cfg.height,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{border, is_focused_ ? 2.0f : 1.5f},
            .child         = field_row,
        });

        // ── Tap to focus ─────────────────────────────────────────
        auto field_gd = gesture_detector({
            .child  = field_box,
            .on_tap = [this, cfg]() {
                // Capture field screen position before focusing
                if (auto* el = element()) {
                    if (auto* ro = el->findRenderObject()) {
                        Rect gb = ro->globalBounds();
                        field_x_ = gb.x;
                        field_y_  = gb.y;
                    }
                }
                focus();
                // Show suggestions overlay if we have suggestions and text
                if (!cfg.suggestions.empty() && has_overlay_ == false && !typed_value_.empty()) {
                    insertSuggOverlay(cfg);
                }
            },
        });


        return field_gd;
    }
};

std::unique_ptr<State> SearchBarWidget::createState() {
    return std::make_unique<SearchBarState>();
}

std::shared_ptr<Widget> search_bar(SearchBarConfig config) {
    return std::make_shared<SearchBarWidget>(std::move(config));
}

} // namespace vaura
