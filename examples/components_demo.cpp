#include "vaura/app/app.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/switch.hpp"
#include "vaura/widgets/slider.hpp"
#include "vaura/widgets/circular_progress.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/state/state.hpp"
#include <iostream>

using namespace vaura;

class ComponentsDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "ComponentsDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class ComponentsDemoState : public State {
private:
    bool notifications_enabled_ = true;
    bool dark_mode_ = true;
    float volume_ = 0.7f;
    float brightness_ = 0.5f;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(dark_mode_ ? 0xFF0F172A : 0xFFF1F5F9)
            .alignItems(YGAlignCenter)
            .padding(YGEdgeTop, 60);

        auto content = std::make_shared<FlexBox>();
        content->width(400)
               .backgroundColor(dark_mode_ ? 0xFF1E293B : 0xFFFFFFFF)
               .borderRadius(16)
               .padding(YGEdgeAll, 30);

        auto title = text("Settings Demo", {
            .font_size = 28,
            .color = dark_mode_ ? 0xFFFFFFFF : 0xFF0F172A,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(YGEdgeBottom, 30).child(title);

        auto build_switch_row = [this](const std::string& label, bool& value) {
            auto row = std::make_shared<FlexBox>();
            row->widthPercent(100)
               .flexDirection(YGFlexDirectionRow)
               .alignItems(YGAlignCenter)
               .justifyContent(YGJustifySpaceBetween)
               .margin(YGEdgeBottom, 20);

            auto text_widget = text(label, {
                .font_size = 16,
                .color = dark_mode_ ? 0xFFE2E8F0 : 0xFF334155
            });

            auto toggle = toggle_switch({
                .value = value,
                .on_changed = [this, &value](bool val) {
                    setState([this, &value, val] { value = val; });
                }
            });

            row->child(text_widget).child(toggle);
            return row;
        };

        auto build_slider_row = [this](const std::string& label, float& value) {
            auto row = std::make_shared<FlexBox>();
            row->widthPercent(100)
               .flexDirection(YGFlexDirectionColumn)
               .margin(YGEdgeBottom, 20);

            auto text_widget = text(label, {
                .font_size = 16,
                .color = dark_mode_ ? 0xFFE2E8F0 : 0xFF334155
            });

            auto text_container = std::make_shared<FlexBox>();
            text_container->margin(YGEdgeBottom, 10).child(text_widget);

            auto slider_widget = slider({
                .value = value,
                .on_changed = [this, &value](float val) {
                    setState([this, &value, val] { value = val; });
                },
                .width = 340 // 400 - 60 padding
            });

            row->child(text_container).child(slider_widget);
            return row;
        };

        content->child(title_container);
        content->child(build_switch_row("Enable Notifications", notifications_enabled_));
        content->child(build_switch_row("Dark Mode", dark_mode_));
        content->child(build_slider_row("Volume", volume_));
        content->child(build_slider_row("Brightness", brightness_));

        auto progress_row = std::make_shared<FlexBox>();
        progress_row->widthPercent(100)
                    .flexDirection(YGFlexDirectionRow)
                    .alignItems(YGAlignCenter)
                    .justifyContent(YGJustifySpaceBetween)
                    .margin(YGEdgeTop, 20);
        
        auto progress_text = text("Loading State", {
            .font_size = 16,
            .color = dark_mode_ ? 0xFFE2E8F0 : 0xFF334155
        });
        
        auto progress_indicator = circular_progress({
            .color = 0xFF38BDF8,
            .stroke_width = 4.0f,
            .size = 24.0f
        });
        
        progress_row->child(progress_text).child(progress_indicator);
        content->child(progress_row);

        root->child(content);
        return root;
    }
};

std::unique_ptr<State> ComponentsDemoApp::createState() {
    return std::make_unique<ComponentsDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Components Demo",
        .width = 800,
        .height = 600,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<ComponentsDemoApp>(), config);
}
