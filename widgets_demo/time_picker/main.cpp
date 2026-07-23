// widgets_demo/time_picker/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/time_picker.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class TimePickerDemoState : public State {
    int current_hour = 14;
    int current_minute = 30;

public:
    WidgetPtr build(BuildContext& ctx) override {
        char buf[64];
        snprintf(buf, sizeof(buf), "Selected Time: %02d:%02d", current_hour, current_minute);

        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Time Picker Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                TimePicker({
                    .hour = current_hour,
                    .minute = current_minute,
                    .use_24h = true,
                    .on_changed = [this](int h, int m) {
                        setState([this, h, m]() {
                            current_hour = h;
                            current_minute = m;
                        });
                    },
                    .clock_size = 360.0f
                }),
                Column({.height = 20.0f}),
                Text(buf, {.font_size = 24.0f, .color = 0xFF10B981, .weight = TextStyle::SemiBold})
            }
        });
    }
};

class TimePickerDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "TimePickerDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<TimePickerDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Time Picker Demo";
    config.width = 390;
    config.height = 780;
    return runApp(std::make_shared<TimePickerDemo>(), config);
}
