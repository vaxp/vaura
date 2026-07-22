#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/bottom_sheet.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class BottomSheetDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "BottomSheetDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class BottomSheetDemoState : public State {
    bool show_sheet = false;

public:
    WidgetPtr build(BuildContext& context) override {
        // Main Background Body
        auto body = std::make_shared<FlexBox>();
        body->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .widthPercent(100.0f)
            .heightPercent(100.0f)
            .backgroundColor(0xFF0F172A); // Slate 900

        // Title in Body
        TextConfig title_cfg;
        title_cfg.font_size = 32.0f;
        title_cfg.color = 0xFFF8FAFC;
        body->child(std::make_shared<TextWidget>("Bottom Sheet Demo", title_cfg));

        // Spacer
        auto spacer = std::make_shared<FlexBox>();
        spacer->height(40.0f);
        body->child(spacer);

        // Open Button
        ButtonConfig btn_cfg;
        btn_cfg.color = 0xFF8B5CF6; // Violet 500
        btn_cfg.hover_color = 0xFF7C3AED;
        btn_cfg.border_radius = 12.0f;
        btn_cfg.padding = EdgeInsets(16, 32, 16, 32);
        btn_cfg.on_pressed = [this]() {
            setState([this]() { show_sheet = true; });
        };
        
        TextConfig btn_txt;
        btn_txt.font_size = 18.0f;
        btn_txt.color = 0xFFFFFFFF;
        btn_cfg.child = std::make_shared<TextWidget>("Open Action Menu", btn_txt);
        
        body->child(std::make_shared<ButtonWidget>(btn_cfg));

        // Sheet Content
        auto sheet_content = std::make_shared<FlexBox>();
        sheet_content->flexDirection(YGFlexDirectionColumn)
                     .justifyContent(YGJustifyFlexStart)
                     .alignItems(YGAlignCenter)
                     .padding(YGEdgeAll, 24.0f)
                     .gap(YGGutterAll, 16.0f)
                     .widthPercent(100.0f)
                     .heightPercent(100.0f);

        TextConfig sheet_title_cfg;
        sheet_title_cfg.font_size = 24.0f;
        sheet_title_cfg.color = 0xFFFFFFFF;
        sheet_content->child(std::make_shared<TextWidget>("Share this Document", sheet_title_cfg));

        auto add_action = [&](const std::string& label, Color color) {
            ButtonConfig action_btn;
            action_btn.color = color;
            action_btn.border_radius = 8.0f;
            action_btn.padding = EdgeInsets(12, 24, 12, 24);
            action_btn.width = 300.0f; // Fixed width for actions
            action_btn.on_pressed = [this]() {
                setState([this]() { show_sheet = false; });
            };
            
            TextConfig action_txt;
            action_txt.font_size = 16.0f;
            action_txt.color = 0xFFFFFFFF;
            action_btn.child = std::make_shared<TextWidget>(label, action_txt);
            
            sheet_content->child(std::make_shared<ButtonWidget>(action_btn));
        };

        add_action("Copy Link", 0xFF3B82F6); // Blue
        add_action("Share to Twitter", 0xFF1DA1F2); // Twitter Blue
        add_action("Email", 0xFF10B981); // Emerald

        // Bottom Sheet Setup
        BottomSheetConfig sheet_cfg;
        sheet_cfg.body = body;
        sheet_cfg.child = sheet_content;
        sheet_cfg.is_open = show_sheet;
        sheet_cfg.on_close = [this]() {
            setState([this]() { show_sheet = false; });
        };
        sheet_cfg.background_color = 0xFF1E293B; // Slate 800
        sheet_cfg.border_radius = 24.0f;
        sheet_cfg.max_height_percent = 50.0f;

        return BottomSheet(sheet_cfg);
    }
};

std::unique_ptr<State> BottomSheetDemo::createState() {
    return std::make_unique<BottomSheetDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Bottom Sheet Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<BottomSheetDemo>(), config);
}
