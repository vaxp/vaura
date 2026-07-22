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
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("Time Picker Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        root->child(spacer);

        TimePickerConfig tp_cfg;
        tp_cfg.hour = current_hour;
        tp_cfg.minute = current_minute;
        tp_cfg.use_24h = true;
        tp_cfg.clock_size = 360.0f;
        tp_cfg.on_changed = [this](int h, int m) {
            setState([this, h, m]() {
                current_hour = h;
                current_minute = m;
            });
        };

        root->child(TimePicker(tp_cfg));

        auto spacer2 = std::make_shared<FlexBox>();
        spacer2->height(20);
        root->child(spacer2);

        char buf[64];
        snprintf(buf, sizeof(buf), "Selected Time: %02d:%02d", current_hour, current_minute);
        root->child(Text(buf, {.font_size = 24.0f, .color = 0xFF10B981, .weight = TextStyle::SemiBold}));

        return root;
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
