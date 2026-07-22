/// @file dropdown.cpp
/// @brief Dropdown widget implementation — uses OverlayController for true overlay rendering.
///
/// Strategy:
///   - The trigger (field) is rendered normally in the widget tree.
///   - When opened, a full-screen overlay is inserted via OverlayController:
///       * An invisible backdrop GestureDetector covering the whole screen (closes on tap)
///       * The dropdown list panel positioned at the computed screen coordinates of the field
///   - Position is stored in State after layout (from globalBounds of the render object).
///   - When closed, the overlay entry is removed.

#include "vaura/widgets/dropdown.hpp"
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
#include "vaura/overlay/popup_window_controller.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

class DropdownWidget : public StatefulWidget {
public:
    DropdownConfig config;
    explicit DropdownWidget(DropdownConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Dropdown"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class DropdownState : public State {
public:
    bool       is_open      = false;
    int        hovered_idx  = -1;
    PopupId    popup_id_    = 0;
    bool       has_popup_   = false;

    // Screen position of the trigger field (set after first layout)
    float field_screen_x_ = 0.0f;
    float field_screen_y_ = 0.0f;

    AnimatedValue<float> open_anim_{0.0f, 1.0f, std::chrono::milliseconds{160}};
    std::unique_ptr<Ticker> ticker_;

    void refreshPopup(const DropdownConfig& cfg) {
        if (has_popup_) {
            PopupWindowController::instance().close(popup_id_);
            popup_id_ = PopupWindowController::instance().open(
                buildPopupConfig(cfg),
                buildListWidget(cfg));
        }
    }

    void initState() override {
        State::initState();
        open_anim_.addListener([this]() { 
            if (auto* dw = static_cast<const DropdownWidget*>(widget())) {
                refreshPopup(dw->config);
            }
            setState([]{}); 
        });
        ticker_ = createTicker([this]() { open_anim_.tick(); });
        ticker_->start();
    }

    void dispose() override {
        closePopup();
        if (ticker_) ticker_->stop();
        open_anim_.dispose();
        State::dispose();
    }

    std::string selectedLabel() const {
        auto* dw = static_cast<const DropdownWidget*>(widget());
        for (auto& item : dw->config.items) {
            if (item.value == dw->config.selected_value) return item.label;
        }
        return "";
    }

    // Build the popup window config — positioned below the trigger field
    PopupWindowConfig buildPopupConfig(const DropdownConfig& cfg) {
        int item_height = static_cast<int>(cfg.height);  // each item same height as field
        int total_height = static_cast<int>(cfg.items.size()) * item_height + 8;
        return PopupWindowConfig{
            .x     = static_cast<int>(field_screen_x_),
            .y     = static_cast<int>(field_screen_y_ + cfg.height + 4.0f),
            .width = static_cast<int>(cfg.width),
            .height= total_height,
            .clear_color = cfg.background_color,
        };
    }

    // Build the dropdown list widget — just the panel, no backdrop or positioning
    WidgetPtr buildListWidget(const DropdownConfig& cfg) {
        auto list_col = std::make_shared<FlexBox>();
        list_col->flexDirection(YGFlexDirectionColumn)
                 .width(cfg.width)
                 .padding(YGEdgeVertical, 4);

        int idx = 0;
        for (auto& item : cfg.items) {
            bool is_selected = (item.value == cfg.selected_value);
            bool is_hovered  = (hovered_idx == idx);

            Color item_bg = is_selected ? cfg.selected_color
                          : is_hovered  ? cfg.hover_color
                          :               0x00000000;

            auto item_row = std::make_shared<FlexBox>();
            item_row->flexDirection(YGFlexDirectionRow)
                     .alignItems(YGAlignCenter)
                     .padding(YGEdgeLeft, 12)
                     .padding(YGEdgeRight, 12)
                     .padding(YGEdgeVertical, 10)
                     .width(cfg.width)
                     .backgroundColor(item_bg);

            if (item.icon) item_row->child(item.icon);
            item_row->child(text(item.label, {.font_size = cfg.font_size, .color = cfg.text_color}));

            int ci = idx;
            std::string cv = item.value;
            auto item_gd = gesture_detector({
                .child          = item_row,
                .on_tap         = [this, cv, cfg]() { close(); if (cfg.on_changed) cfg.on_changed(cv); },
                .on_hover_enter = [this, ci, cfg]() { hovered_idx = ci; refreshPopup(cfg); },
                .on_hover_exit  = [this, cfg]()     { hovered_idx = -1; refreshPopup(cfg); },
            });
            list_col->child(item_gd);
            ++idx;
        }

        auto list_panel = container({
            .color         = cfg.background_color,
            .width         = cfg.width,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{cfg.border_color, 1.0f},
            .child         = list_col,
        });

        return opacity(open_anim_.get(), list_panel);
    }

    void openPopup(const DropdownConfig& cfg) {
        if (has_popup_) return;
        popup_id_ = PopupWindowController::instance().open(
            buildPopupConfig(cfg),
            buildListWidget(cfg));
        has_popup_ = (popup_id_ != 0);
    }

    void closePopup() {
        if (has_popup_) {
            PopupWindowController::instance().close(popup_id_);
            has_popup_ = false;
            popup_id_ = 0;
        }
    }

    void open(const DropdownConfig& cfg) {
        is_open = true;
        open_anim_.forward();
        openPopup(cfg);
    }

    void close() {
        is_open = false;
        hovered_idx = -1;
        open_anim_.reverse();
        closePopup();
        setState([]{});
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* dw = static_cast<const DropdownWidget*>(widget());
        const auto& cfg = dw->config;

        std::string label_text = selectedLabel();
        bool has_selection = !label_text.empty();

        auto label = text(has_selection ? label_text : cfg.placeholder, {
            .font_size = cfg.font_size,
            .color     = has_selection ? cfg.text_color : cfg.placeholder_color,
        });

        auto chevron = icon(is_open ? Icons::ChevronUp : Icons::ChevronDown, {
            .size  = 18.0f,
            .color = cfg.text_color,
        });

        auto chevron_wrap = std::make_shared<FlexBox>();
        chevron_wrap->width(20).height(20)
                     .justifyContent(YGJustifyCenter)
                     .alignItems(YGAlignCenter)
                     .child(chevron);

        auto field_row = std::make_shared<FlexBox>();
        field_row->flexDirection(YGFlexDirectionRow)
                  .alignItems(YGAlignCenter)
                  .justifyContent(YGJustifySpaceBetween)
                  .padding(YGEdgeLeft, 12)
                  .padding(YGEdgeRight, 10)
                  .width(cfg.width)
                  .height(cfg.height);
        field_row->child(label).child(chevron_wrap);

        Color border_col = is_open ? cfg.focus_color : cfg.border_color;
        auto field_box = container({
            .color         = cfg.background_color,
            .width         = cfg.width,
            .height        = cfg.height,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{border_col, 1.5f},
            .child         = field_row,
        });

        // Use a GestureDetector that also reads its own global position
        // after layout to position the overlay correctly
        auto clickable_field = gesture_detector({
            .child  = field_box,
            .on_tap = [this, cfg]() {
                if (is_open) {
                    close();
                } else {
                    // Read field position from element's render object
                    if (auto* el = element()) {
                        if (auto* ro = el->findRenderObject()) {
                            Rect gb = ro->globalBounds();
                            field_screen_x_ = gb.x;
                            field_screen_y_ = gb.y;
                        }
                    }
                    open(cfg);
                }
            },
        });

        return clickable_field;
    }
};

std::unique_ptr<State> DropdownWidget::createState() {
    return std::make_unique<DropdownState>();
}

std::shared_ptr<Widget> dropdown(DropdownConfig config) {
    return std::make_shared<DropdownWidget>(std::move(config));
}

}  // namespace vaura
