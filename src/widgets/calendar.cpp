/// @file calendar.cpp
/// @brief Calendar widget implementation.

#include "vaura/widgets/calendar.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include "vaura/rendering/canvas.hpp"
#include "vaura/tree/render_object.hpp"
#include <layout_engine/Yoga.h>
#include <array>
#include <ctime>

namespace vaura {

// ── Date helpers ──────────────────────────────────────────────

static const std::array<const char*, 12> MONTH_NAMES = {
    "January","February","March","April","May","June",
    "July","August","September","October","November","December"
};
static const std::array<const char*, 7> WEEKDAY_NAMES = {
    "Su","Mo","Tu","We","Th","Fr","Sa"
};

/// Returns 1 (Mon) – 7 (Sun) for the first day of the month.
static int firstWeekday(int year, int month) {
    // Tomohiko Sakamoto's algorithm (returns 0=Sun, 1=Mon, ..., 6=Sat)
    static const int t[] = {0,3,2,5,0,3,5,1,4,6,2,4};
    if (month < 3) year--;
    return (year + year/4 - year/100 + year/400 + t[month-1] + 1) % 7;
}

static int daysInMonth(int year, int month) {
    static const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    int d = days[month - 1];
    if (month == 2 && ((year%4==0 && year%100!=0) || year%400==0)) d = 29;
    return d;
}

static int todayYear() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    return lt->tm_year + 1900;
}
static int todayMonth() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    return lt->tm_mon + 1;
}
static int todayDay() {
    time_t t = time(nullptr);
    tm* lt = localtime(&t);
    return lt->tm_mday;
}

// ─────────────────────────────────────────────────────────────

class CalendarWidget : public StatefulWidget {
public:
    CalendarConfig config;
    explicit CalendarWidget(CalendarConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "Calendar"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class CalendarState : public State {
public:
    int hovered_day = -1;

    WidgetPtr build(BuildContext& ctx) override {
        auto* cw = static_cast<const CalendarWidget*>(widget());
        const auto& cfg = cw->config;

        float grid_width = cfg.cell_size * 7.0f;
        int ty = todayYear(), tm_mon = todayMonth(), td = todayDay();

        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
             .width(grid_width)
             .backgroundColor(cfg.background_color);

        // ── Header: ← Month Year → ───────────────────────────
        auto hdr = std::make_shared<FlexBox>();
        hdr->flexDirection(YGFlexDirectionRow)
            .alignItems(YGAlignCenter)
            .justifyContent(YGJustifySpaceBetween)
            .width(grid_width)
            .height(cfg.header_height)
            .padding(YGEdgeHorizontal, 8)
            .backgroundColor(cfg.header_color);

        auto prev_btn = gesture_detector({
            .child = container({
                .padding = EdgeInsets::all(8),
                .child   = icon(Icons::ChevronLeft, {.size = 20.0f, .color = cfg.header_text_color}),
            }),
            .on_tap = [cw, cfg]() {
                if (cfg.on_month_changed) {
                    int nm = cfg.month - 1;
                    int ny = cfg.year;
                    if (nm < 1) { nm = 12; ny--; }
                    cfg.on_month_changed(ny, nm);
                }
            },
        });

        auto next_btn = gesture_detector({
            .child = container({
                .padding = EdgeInsets::all(8),
                .child   = icon(Icons::ChevronRight, {.size = 20.0f, .color = cfg.header_text_color}),
            }),
            .on_tap = [cw, cfg]() {
                if (cfg.on_month_changed) {
                    int nm = cfg.month + 1;
                    int ny = cfg.year;
                    if (nm > 12) { nm = 1; ny++; }
                    cfg.on_month_changed(ny, nm);
                }
            },
        });

        std::string month_label = std::string(MONTH_NAMES[cfg.month - 1]) + " " + std::to_string(cfg.year);
        hdr->child(prev_btn)
            .child(text(month_label, {
                .font_size = 15.0f,
                .color     = cfg.header_text_color,
                .weight    = TextStyle::SemiBold,
                .align     = TextStyle::Center,
            }))
            .child(next_btn);
        root->child(hdr);

        // ── Weekday row: Su Mo Tu We Th Fr Sa ────────────────
        auto wk_row = std::make_shared<FlexBox>();
        wk_row->flexDirection(YGFlexDirectionRow)
               .width(grid_width)
               .padding(YGEdgeVertical, 6);

        for (int w = 0; w < 7; ++w) {
            bool is_weekend = (w == 0 || w == 6);
            auto cell = std::make_shared<FlexBox>();
            cell->width(cfg.cell_size).height(cfg.cell_size * 0.7f)
                 .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter)
                 .child(text(WEEKDAY_NAMES[w], {
                     .font_size = cfg.font_size - 1.0f,
                     .color     = is_weekend ? cfg.weekend_color : cfg.weekday_color,
                     .weight    = TextStyle::SemiBold,
                     .align     = TextStyle::Center,
                 }));
            wk_row->child(cell);
        }
        root->child(wk_row);

        // ── Day grid ──────────────────────────────────────────
        int first_wd  = firstWeekday(cfg.year, cfg.month); // 0=Sun
        int days_cur  = daysInMonth(cfg.year, cfg.month);
        int prev_mon  = cfg.month - 1; int prev_yr = cfg.year;
        if (prev_mon < 1) { prev_mon = 12; prev_yr--; }
        int days_prev = daysInMonth(prev_yr, prev_mon);

        int total_cells = 42; // 6 rows × 7 cols
        auto grid = std::make_shared<FlexBox>();
        grid->flexDirection(YGFlexDirectionRow)
             .flexWrap(YGWrapWrap)
             .width(grid_width);

        for (int cell = 0; cell < total_cells; ++cell) {
            int col = cell % 7;
            bool is_weekend = (col == 0 || col == 6);

            int day, year_of_day, month_of_day;
            bool in_cur_month;

            if (cell < first_wd) {
                // Previous month
                day = days_prev - first_wd + cell + 1;
                year_of_day  = prev_yr; month_of_day = prev_mon;
                in_cur_month = false;
            } else if (cell - first_wd < days_cur) {
                day = cell - first_wd + 1;
                year_of_day  = cfg.year; month_of_day = cfg.month;
                in_cur_month = true;
            } else {
                // Next month
                day = cell - first_wd - days_cur + 1;
                int nm = cfg.month + 1; int ny = cfg.year;
                if (nm > 12) { nm = 1; ny++; }
                year_of_day  = ny; month_of_day = nm;
                in_cur_month = false;
            }

            bool is_today    = in_cur_month && (year_of_day == ty) &&
                               (month_of_day == tm_mon) && (day == td);
            bool is_selected = in_cur_month && (day == cfg.selected_day);
            bool is_hovered  = in_cur_month && (hovered_day == day);

            Color bg_color = is_selected ? cfg.selected_color
                           : is_today    ? cfg.today_color
                           : is_hovered  ? cfg.hover_color
                           :               0x00000000;

            Color num_color = !in_cur_month  ? cfg.other_month_color
                            : is_selected    ? 0xFFFFFFFF
                            : is_weekend     ? cfg.weekend_color
                            :                  cfg.day_color;

            auto day_cell = std::make_shared<FlexBox>();
            day_cell->width(cfg.cell_size).height(cfg.cell_size)
                     .justifyContent(YGJustifyCenter)
                     .alignItems(YGAlignCenter)
                     .borderRadius(cfg.cell_size / 2.0f)
                     .backgroundColor(bg_color)
                     .child(text(std::to_string(day), {
                         .font_size = cfg.font_size,
                         .color     = num_color,
                         .align     = TextStyle::Center,
                     }));

            if (in_cur_month) {
                int cd = day;
                int cy2 = cfg.year, cm2 = cfg.month;
                auto fn = cfg.on_day_selected;
                auto cell_gd = gesture_detector({
                    .child          = day_cell,
                    .on_tap         = [fn, cy2, cm2, cd]() { if (fn) fn(cy2, cm2, cd); },
                    .on_hover_enter = [this, cd]() { setState([this, cd]{ hovered_day = cd; }); },
                    .on_hover_exit  = [this]()     { setState([this]{ hovered_day = -1; }); },
                });
                grid->child(cell_gd);
            } else {
                grid->child(day_cell);
            }
        }

        root->child(grid);
        return root;
    }
};

std::unique_ptr<State> CalendarWidget::createState() {
    return std::make_unique<CalendarState>();
}

std::shared_ptr<Widget> calendar(CalendarConfig config) {
    return std::make_shared<CalendarWidget>(std::move(config));
}

} // namespace vaura
