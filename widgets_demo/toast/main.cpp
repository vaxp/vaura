// widgets_demo/toast/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/toast.hpp"
#include "vaura/widgets/stack.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class ToastDemoState : public State {
    bool show_toast = false;
    ToastConfig::Position toast_pos = ToastConfig::Position::Bottom;
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionColumn)
               .justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter)
               .backgroundColor(0xFF0F172A)
               .widthPercent(100).heightPercent(100);

        content->child(text("Toast Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40);
        content->child(spacer);

        auto btn_row = std::make_shared<FlexBox>();
        btn_row->flexDirection(YGFlexDirectionRow).justifyContent(YGJustifyCenter);

        ButtonConfig top_btn;
        top_btn.child = text("Top Toast", {.color = 0xFFFFFFFF});
        top_btn.color = 0xFF3B82F6;
        top_btn.on_pressed = [this]() {
            setState([this]() { show_toast = true; toast_pos = ToastConfig::Position::Top; });
        };
        auto w_top = std::make_shared<FlexBox>(); w_top->margin(YGEdgeRight, 10).child(button(top_btn));
        btn_row->child(w_top);

        ButtonConfig bottom_btn;
        bottom_btn.child = text("Bottom Toast", {.color = 0xFFFFFFFF});
        bottom_btn.color = 0xFF10B981;
        bottom_btn.on_pressed = [this]() {
            setState([this]() { show_toast = true; toast_pos = ToastConfig::Position::Bottom; });
        };
        auto w_bot = std::make_shared<FlexBox>(); w_bot->child(button(bottom_btn));
        btn_row->child(w_bot);

        content->child(btn_row);

        StackConfig stack_cfg;
        stack_cfg.alignment = Alignment::Center;
        stack_cfg.children.push_back(content);

        if (show_toast) {
            ToastConfig toast_cfg;
            toast_cfg.message = "This is a toast notification!";
            toast_cfg.position = toast_pos;
            toast_cfg.on_dismissed = [this]() {
                setState([this]() { show_toast = false; });
            };
            
            auto toast_wrapper = std::make_shared<FlexBox>();
            toast_wrapper->widthPercent(100).heightPercent(100);
            toast_wrapper->child(toast(toast_cfg));
            
            stack_cfg.children.push_back(toast_wrapper);
        }

        return stack(stack_cfg);
    }
};

class ToastDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "ToastDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<ToastDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Toast Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<ToastDemo>(), config);
}
