#include "vaura/app/app.hpp"
#include "vaura/widgets/checkbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class CheckboxDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CheckboxDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class CheckboxDemoState : public State {
private:
    bool is_agreed_ = false;
    bool is_subscribed_ = true;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionColumn)
               .backgroundColor(0xFF0F172A)
               .padding(YGEdgeAll, 40)
               .borderRadius(16);

        auto title = Text("Checkbox Demo", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(YGEdgeBottom, 30).child(title);
        content->child(title_container);

        // Helper to build a labeled checkbox row
        auto build_labeled_checkbox = [this](const std::string& label, bool& value) {
            auto row = std::make_shared<FlexBox>();
            row->flexDirection(YGFlexDirectionRow)
               .alignItems(YGAlignCenter)
               .margin(YGEdgeBottom, 20);

            auto cb = Checkbox({
                .value = value,
                .on_changed = [this, &value](bool new_val) {
                    setState([&value, new_val] {
                        value = new_val;
                    });
                },
                .size = 24.0f,
                .active_color = 0xFF38BDF8,
                .inactive_color = 0xFF64748B,
                .checkmark_color = 0xFFFFFFFF,
                .border_radius = 6.0f,
                .stroke_width = 2.5f
            });

            auto txt = Text(label, {
                .font_size = 18,
                .color = 0xFFE2E8F0
            });
            
            auto txt_container = std::make_shared<FlexBox>();
            txt_container->child(txt);
            
            // Checkbox on the left, then margin, then text
            auto cb_container = std::make_shared<FlexBox>();
            cb_container->margin(YGEdgeRight, 15).child(cb);

            row->child(cb_container).child(txt_container);
            
            // Make the whole row tappable
            auto clickable_row = GestureDetector({
                .child = row,
                .on_tap = [this, &value]() {
                    setState([&value] { value = !value; });
                }
            });

            return clickable_row;
        };

        content->child(build_labeled_checkbox("I agree to the Terms of Service", is_agreed_));
        content->child(build_labeled_checkbox("Subscribe to newsletter", is_subscribed_));

        root->child(content);
        return root;
    }
};

std::unique_ptr<State> CheckboxDemoApp::createState() {
    return std::make_unique<CheckboxDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Checkbox Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<CheckboxDemoApp>(), config);
}
