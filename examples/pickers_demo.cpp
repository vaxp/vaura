/// @file pickers_demo.cpp
/// @brief Demo for Phase 3: Selection & Data Pickers
///        SearchBar, DatePicker, TimePicker, ColorPicker

#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/search_bar.hpp"
#include "vaura/widgets/calendar.hpp"
#include "vaura/widgets/time_picker.hpp"
#include "vaura/widgets/color_picker.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <string>
#include <sstream>
#include <iomanip>

using namespace vaura;

class PickersDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "PickersDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class PickersDemoState : public State {
public:
    int active_tab = 0;
    
    // SearchBar
    std::string search_value = "";
    std::vector<std::string> search_sugs = {"Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig"};
    
    // Calendar
    int cal_year  = 2025;
    int cal_month = 7;
    int cal_day   = 15;
    
    // TimePicker
    int time_hour = 14;
    int time_min  = 30;
    bool time_is_am = false;
    
    // ColorPicker
    Color picked_color = 0xFF0EA5E9;

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100)
             .backgroundColor(0xFF0F172A)
             .flexDirection(YGFlexDirectionRow); // Sidebar + Content

        // ── Sidebar (Tabs) ────────────────────────────────────
        auto sidebar = std::make_shared<FlexBox>();
        sidebar->width(220).heightPercent(100)
               .backgroundColor(0xFF1E293B)
               .padding(YGEdgeAll, 16);

        sidebar->child(Text("Phase 3", {.font_size=20, .color=0xFFF8FAFC, .weight=TextStyle::Bold}));
        sidebar->child(Text("Data Pickers", {.font_size=14, .color=0xFF94A3B8}));
        auto sp1 = std::make_shared<FlexBox>(); sp1->height(30); sidebar->child(sp1);

        auto add_tab = [&](int idx, const char* label) {
            bool active = (active_tab == idx);
            auto btn = GestureDetector({
                .child = Container({
                    .color         = active ? Color(0x220EA5E9) : Color(0x00000000),
                    .border_radius = BorderRadius::circular(8),
                    .child         = Text(label, {
                        .font_size = 14,
                        .color     = active ? 0xFF0EA5E9 : 0xFFCBD5E1,
                        .weight    = active ? TextStyle::SemiBold : TextStyle::Regular,
                    }),
                }),
                .on_tap = [this,idx](){ setState([this,idx]{ active_tab = idx; }); },
            });
            auto bw = std::make_shared<FlexBox>(); bw->margin(YGEdgeBottom,8).child(btn);
            sidebar->child(bw);
        };
        add_tab(0, "🔍 SearchBar");
        add_tab(1, "📅 DatePicker");
        add_tab(2, "⏰ TimePicker");
        add_tab(3, "🎨 ColorPicker");

        root->child(sidebar);

        // ── Content Area ──────────────────────────────────────
        auto content = std::make_shared<FlexBox>();
        content->flexGrow(1).padding(YGEdgeAll, 40)
               .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter);

        if (active_tab == 0) {
            // SearchBar
            auto sbw = std::make_shared<FlexBox>();
            sbw->flexDirection(YGFlexDirectionColumn).alignItems(YGAlignCenter);
            sbw->child(Text("Integrated Search", {.font_size=24,.color=0xFFF8FAFC,.weight=TextStyle::Bold}));
            auto ssp = std::make_shared<FlexBox>(); ssp->height(24); sbw->child(ssp);
            
            sbw->child(SearchBar({
                .value             = search_value,
                .on_changed        = [this](const std::string& v){ setState([this,v]{ search_value = v; }); },
                .on_submitted      = [this](const std::string& v){ setState([this,v]{ search_value = v; }); },
                .on_suggestion_tap = [this](const std::string& v){ setState([this,v]{ search_value = v; }); },
                .suggestions       = search_sugs,
                .placeholder       = "Search fruits...",
                .background_color  = 0xFF1E293B,
                .text_color        = 0xFFF8FAFC,
                .placeholder_color = 0xFF64748B,
                .border_color      = 0xFF334155,
                .focus_color       = 0xFF0EA5E9,
                .icon_color        = 0xFF64748B,
                .clear_color       = 0xFF64748B,
                .suggestion_hover  = 0xFF334155,
                .border_radius     = 24,
                .width             = 320,
                .height            = 48,
                .font_size         = 15,
            }));
            content->child(sbw);

        } else if (active_tab == 1) {
            // Calendar
            auto cw = std::make_shared<FlexBox>();
            cw->flexDirection(YGFlexDirectionColumn).alignItems(YGAlignCenter);
            cw->child(Text("Select a Date", {.font_size=24,.color=0xFFF8FAFC,.weight=TextStyle::Bold}));
            auto csp = std::make_shared<FlexBox>(); csp->height(24); cw->child(csp);
            
            cw->child(Calendar({
                .year              = cal_year,
                .month             = cal_month,
                .selected_day      = cal_day,
                .on_day_selected   = [this](int y, int m, int d){ setState([this,y,m,d]{ cal_year=y; cal_month=m; cal_day=d; }); },
                .on_month_changed  = [this](int y, int m){ setState([this,y,m]{ cal_year=y; cal_month=m; }); },
                .background_color  = 0xFF1E293B,
                .header_color      = 0xFF0F172A,
                .header_text_color = 0xFFF8FAFC,
                .weekday_color     = 0xFF64748B,
                .day_color         = 0xFFF8FAFC,
                .weekend_color     = 0xFFEF4444,
                .selected_color    = 0xFF0EA5E9,
                .today_color       = 0xFF22C55E,
                .other_month_color = 0xFF334155,
                .hover_color       = 0xFF334155,
            }));
            content->child(cw);

        } else if (active_tab == 2) {
            // TimePicker
            auto tw = std::make_shared<FlexBox>();
            tw->flexDirection(YGFlexDirectionColumn).alignItems(YGAlignCenter);
            tw->child(Text("Select Time", {.font_size=24,.color=0xFFF8FAFC,.weight=TextStyle::Bold}));
            auto tsp = std::make_shared<FlexBox>(); tsp->height(24); tw->child(tsp);
            
            tw->child(TimePicker({
                .hour              = time_hour,
                .minute            = time_min,
                .use_24h           = !time_is_am,
                .on_changed        = [this](int h, int m) {
                    setState([this,h,m]{ time_hour=h; time_min=m; });
                },
                .background_color  = 0xFF1E293B,
                .face_color        = 0xFF0F172A,
                .hand_color        = 0xFF0EA5E9,
                .center_dot_color  = 0xFF0EA5E9,
                .text_color        = 0xFFF8FAFC,
                .selected_text     = 0xFFFFFFFF,
                .dial_bg           = 0xFF334155,
                .clock_size        = 240.0f,
                .font_size         = 14.0f,
            }));
            content->child(tw);

        } else if (active_tab == 3) {
            // ColorPicker
            auto cpw = std::make_shared<FlexBox>();
            cpw->flexDirection(YGFlexDirectionColumn).alignItems(YGAlignCenter);
            cpw->child(Text("Theme Color", {.font_size=24,.color=0xFFF8FAFC,.weight=TextStyle::Bold}));
            auto cpsp = std::make_shared<FlexBox>(); cpsp->height(24); cpw->child(cpsp);
            
            cpw->child(ColorPicker({
                .selected_color   = picked_color,
                .on_changed       = [this](Color c){ setState([this,c]{ picked_color = c; }); },
                .show_alpha       = true,
                .show_hex_input   = true,
                .width            = 300,
                .background_color = 0xFF1E293B,
                .text_color       = 0xFFF8FAFC,
                .font_size        = 13.0f,
            }));
            content->child(cpw);
        }

        root->child(content);
        return root;
    }
};

std::unique_ptr<State> PickersDemoApp::createState() {
    return std::make_unique<PickersDemoState>();
}

int main(int argc, char** argv) {
    return vaura::runApp(std::make_shared<PickersDemoApp>(), {
        .title  = "VAURA — Phase 3: Pickers Demo",
        .width  = 900,
        .height = 640,
        .vsync  = true,
    });
}
