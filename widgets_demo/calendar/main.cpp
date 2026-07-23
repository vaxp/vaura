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
        std::string date_str = "Selected: " + std::to_string(current_year) + "-" + 
                               std::to_string(current_month) + "-" + 
                               std::to_string(selected_day);

        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .gap = 40.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                // Selection Display
                Text(date_str, {.font_size = 28.0f, .color = 0xFFF8FAFC}),

                // Wrap calendar to give it a nice border/shadow
                Column({
                    .padding = std::pair{Edge::All, 8.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 16.0f,
                    .children = {
                        Calendar({
                            .year = current_year,
                            .month = current_month,
                            .selected_day = selected_day,
                            .on_day_selected = [this](int y, int m, int d) {
                                setState([this, y, m, d]() {
                                    current_year = y;
                                    current_month = m;
                                    selected_day = d;
                                });
                            },
                            .on_month_changed = [this](int y, int m) {
                                setState([this, y, m]() {
                                    current_year = y;
                                    current_month = m;
                                });
                            },
                            .background_color = 0xFF1E293B, // Slate 800
                            .header_color = 0xFF0F172A,     // Slate 900
                            .header_text_color = 0xFFF8FAFC,
                            .selected_color = 0xFF0EA5E9,   // Sky Blue 500
                            .today_color = 0xFF10B981,      // Emerald 500
                            .cell_size = 48.0f,             // Slightly larger for better touch targets
                            .font_size = 16.0f
                        })
                    }
                })
            }
        });
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
