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
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .gap(YGGutterAll, 30.0f)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(text("Select a Date", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        DatePickerConfig cfg;
        cfg.selected_year = y;
        cfg.selected_month = m;
        cfg.selected_day = d;
        cfg.on_changed = [this](int yy, int mm, int dd) {
            setState([this, yy, mm, dd]() {
                y = yy;
                m = mm;
                d = dd;
            });
        };
        
        root->child(date_picker(cfg));
        
        std::string selected_str = "You selected: " + std::to_string(y) + "-" + 
                                   (m < 10 ? "0" : "") + std::to_string(m) + "-" + 
                                   (d < 10 ? "0" : "") + std::to_string(d);
                                   
        root->child(text(selected_str, {.font_size = 16.0f, .color = 0xFF94A3B8}));

        return root;
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
