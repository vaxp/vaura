#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/date_picker.hpp"
#include "vaura/state/state.hpp"
#include <string>

using namespace vaura;

class DatePickerDemoState : public State {
    int y = 2025;
    int m = 8;
    int d = 15;
    WidgetPtr build(BuildContext& ctx) override {
        std::string selected_str = "You selected: " + std::to_string(y) + "-" + 
                                   (m < 10 ? "0" : "") + std::to_string(m) + "-" + 
                                   (d < 10 ? "0" : "") + std::to_string(d);

        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .gap = 30.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Select a Date", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                
                DatePicker({
                    .selected_year = y,
                    .selected_month = m,
                    .selected_day = d,
                    .on_changed = [this](int yy, int mm, int dd) {
                        setState([this, yy, mm, dd]() {
                            y = yy;
                            m = mm;
                            d = dd;
                        });
                    }
                }),
                
                Text(selected_str, {.font_size = 16.0f, .color = 0xFF94A3B8})
            }
        });
    }
};

class DatePickerDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "DatePickerDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<DatePickerDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA DatePicker Demo";
    config.width = 800;
    config.height = 600;

    return runApp(std::make_shared<DatePickerDemo>(), config);
}
