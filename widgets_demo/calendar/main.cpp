#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/calendar.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"
#include <iostream>

using namespace vaura;

class CalendarDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CalendarDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class CalendarDemoState : public State {
    int current_year = 2025;
    int current_month = 7;
    int selected_day = 15;

public:
    WidgetPtr build(BuildContext& context) override {
        // Main Background Body
        auto body = std::make_shared<FlexBox>();
        body->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .gap(YGGutterAll, 40.0f)
            .widthPercent(100.0f)
            .heightPercent(100.0f)
            .backgroundColor(0xFF0F172A); // Slate 900

        // Selection Display
        TextConfig title_cfg;
        title_cfg.font_size = 28.0f;
        title_cfg.color = 0xFFF8FAFC;
        std::string date_str = "Selected: " + std::to_string(current_year) + "-" + 
                               std::to_string(current_month) + "-" + 
                               std::to_string(selected_day);
        body->child(std::make_shared<TextWidget>(date_str, title_cfg));

        // Calendar Configuration
        CalendarConfig cal_cfg;
        cal_cfg.year = current_year;
        cal_cfg.month = current_month;
        cal_cfg.selected_day = selected_day;
        cal_cfg.background_color = 0xFF1E293B; // Slate 800
        cal_cfg.header_color = 0xFF0F172A;     // Slate 900
        cal_cfg.header_text_color = 0xFFF8FAFC;
        cal_cfg.selected_color = 0xFF0EA5E9;   // Sky Blue 500
        cal_cfg.today_color = 0xFF10B981;      // Emerald 500
        cal_cfg.cell_size = 48.0f;             // Slightly larger for better touch targets
        cal_cfg.font_size = 16.0f;
        
        cal_cfg.on_day_selected = [this](int y, int m, int d) {
            setState([this, y, m, d]() {
                current_year = y;
                current_month = m;
                selected_day = d;
            });
        };

        cal_cfg.on_month_changed = [this](int y, int m) {
            setState([this, y, m]() {
                current_year = y;
                current_month = m;
            });
        };

        // Wrap calendar to give it a nice border/shadow
        auto cal_wrapper = std::make_shared<FlexBox>();
        cal_wrapper->backgroundColor(0xFF1E293B)
                   .borderRadius(16.0f)
                   .padding(YGEdgeAll, 8.0f);
                   
        cal_wrapper->child(calendar(cal_cfg));

        body->child(cal_wrapper);

        return body;
    }
};

std::unique_ptr<State> CalendarDemo::createState() {
    return std::make_unique<CalendarDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Calendar Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<CalendarDemo>(), config);
}
