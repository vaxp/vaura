/// @file date_picker.cpp
/// @brief DatePicker widget implementation — field that opens an inline Calendar.

#include "vaura/widgets/date_picker.hpp"
#include "vaura/widgets/calendar.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <layout_engine/Yoga.h>
#include <string>
#include <iomanip>
#include <sstream>

namespace vaura {

class DatePickerWidget : public StatefulWidget {
public:
    DatePickerConfig config;
    explicit DatePickerWidget(DatePickerConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "DatePicker"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class DatePickerState : public State {
public:
    bool is_open    = false;
    int  view_year  = 0;
    int  view_month = 0;
    AnimatedValue<float> open_anim_{0.0f, 1.0f, std::chrono::milliseconds{180}};
    std::unique_ptr<Ticker> ticker_;

    void initState() override {
        State::initState();
        auto* dw = static_cast<const DatePickerWidget*>(widget());
        view_year  = dw->config.selected_year;
        view_month = dw->config.selected_month;
        open_anim_.addListener([this](){ setState([]{}); });
        ticker_ = createTicker([this](){ open_anim_.tick(); });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        open_anim_.dispose();
        State::dispose();
    }

    static std::string formatDate(int y, int m, int d) {
        std::ostringstream ss;
        ss << std::setfill('0') << std::setw(4) << y << "-"
           << std::setw(2) << m << "-"
           << std::setw(2) << d;
        return ss.str();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* dw = static_cast<const DatePickerWidget*>(widget());
        const auto& cfg = dw->config;

        bool has_date = (cfg.selected_day > 0);
        std::string label = has_date
            ? formatDate(cfg.selected_year, cfg.selected_month, cfg.selected_day)
            : cfg.placeholder;
        Color label_color = has_date ? cfg.text_color : 0xFF64748B;

        Color border = is_open ? cfg.focus_color : cfg.border_color;

        // ── Field ─────────────────────────────────────────────
        auto field_row = std::make_shared<FlexBox>();
        field_row->flexDirection(YGFlexDirectionRow)
                  .alignItems(YGAlignCenter)
                  .justifyContent(YGJustifySpaceBetween)
                  .width(cfg.width).height(cfg.height)
                  .padding(YGEdgeLeft, 12).padding(YGEdgeRight, 10);

        field_row->child(text(label, {.font_size = cfg.font_size, .color = label_color}));
        field_row->child(icon(Icons::CalendarToday, {.size = 18.0f, .color = 0xFF64748B}));

        auto field_box = container({
            .color         = cfg.background_color,
            .width         = cfg.width,
            .height        = cfg.height,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{border, 1.5f},
            .child         = field_row,
        });

        auto field_gd = gesture_detector({
            .child  = field_box,
            .on_tap = [this]() {
                is_open = !is_open;
                if (is_open) open_anim_.forward();
                else         open_anim_.reverse();
            },
        });

        if (open_anim_.get() < 0.01f && !is_open) return field_gd;

        // ── Calendar popup ────────────────────────────────────
        auto cal = calendar({
            .year              = view_year,
            .month             = view_month,
            .selected_day      = has_date && cfg.selected_year == view_year
                                          && cfg.selected_month == view_month
                                          ? cfg.selected_day : -1,
            .on_day_selected   = [this, cfg](int y, int m, int d) {
                is_open     = false;
                open_anim_.reverse();
                if (cfg.on_changed) cfg.on_changed(y, m, d);
            },
            .on_month_changed  = [this](int ny, int nm) {
                setState([this, ny, nm]{ view_year = ny; view_month = nm; });
            },
        });

        auto cal_panel = container({
            .color         = 0xFF1E293B,
            .border_radius = BorderRadius::circular(12.0f),
            .border        = Border{cfg.border_color, 1.0f},
            .shadow        = Shadow{0x50000000, 16.0f, {0, 6}},
            .child         = cal,
        });

        auto cal_faded = opacity(open_anim_.get(), cal_panel);

        auto cal_wrapper = std::make_shared<FlexBox>();
        cal_wrapper->positionType(YGPositionTypeAbsolute)
                    .position(YGEdgeTop, cfg.height + 4.0f)
                    .position(YGEdgeLeft, 0.0f);
        cal_wrapper->child(cal_faded);

        // Backdrop
        auto bd = std::make_shared<FlexBox>();
        bd->positionType(YGPositionTypeAbsolute)
           .position(YGEdgeAll, 0.0f)
           .widthPercent(100).heightPercent(100);
        auto bd_gd = gesture_detector({
            .child  = bd,
            .on_tap = [this]() { is_open = false; open_anim_.reverse(); },
        });

        auto root = std::make_shared<FlexBox>();
        root->width(cfg.width).height(cfg.height);
        root->child(field_gd).child(bd_gd).child(cal_wrapper);
        return root;
    }
};

std::unique_ptr<State> DatePickerWidget::createState() {
    return std::make_unique<DatePickerState>();
}

std::shared_ptr<Widget> date_picker(DatePickerConfig config) {
    return std::make_shared<DatePickerWidget>(std::move(config));
}

} // namespace vaura
