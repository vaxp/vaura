#include "vaura/app/app.hpp"
#include "vaura/widgets/radio.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class RadioDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "RadioDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class RadioDemoState : public State {
private:
    int selected_option_ = 1;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justify(Justify::Center)
            .align(Align::Center);

        auto content = std::make_shared<FlexBox>();
        content->direction(FlexDirection::Column)
               .backgroundColor(0xFF0F172A)
               .padding(Edge::All, 40)
               .borderRadius(16);

        auto title = Text("Radio Button Demo", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(Edge::Bottom, 30).child(title);
        content->child(title_container);

        auto build_radio_option = [this](const std::string& label, int value) {
            auto row = std::make_shared<FlexBox>();
            row->direction(FlexDirection::Row)
               .align(Align::Center)
               .margin(Edge::Bottom, 20);

            auto rb = Radio({
                .value = (selected_option_ == value),
                .on_changed = nullptr, // Handled by row
                .size = 24.0f,
                .active_color = 0xFF38BDF8,
                .inactive_color = 0xFF64748B,
                .stroke_width = 2.5f,
                .inner_padding = 4.0f
            });

            auto txt = Text(label, {
                .font_size = 18,
                .color = 0xFFE2E8F0
            });
            
            auto txt_container = std::make_shared<FlexBox>();
            txt_container->child(txt);
            
            auto rb_container = std::make_shared<FlexBox>();
            rb_container->margin(Edge::Right, 15).child(rb);

            row->child(rb_container).child(txt_container);
            
            auto clickable_row = GestureDetector({
                .child = row,
                .on_tap = [this, value]() {
                    setState([this, value] {
                        selected_option_ = value;
                    });
                }
            });

            return clickable_row;
        };

        content->child(build_radio_option("Option 1", 1));
        content->child(build_radio_option("Option 2", 2));
        content->child(build_radio_option("Option 3", 3));

        root->child(content);
        return root;
    }
};

std::unique_ptr<State> RadioDemoApp::createState() {
    return std::make_unique<RadioDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Radio Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<RadioDemoApp>(), config);
}
