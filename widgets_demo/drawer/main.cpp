#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/drawer.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class DrawerDemoState : public State {
    bool is_open = false;
public:
    WidgetPtr build(BuildContext& ctx) override {
        // --- Drawer Content ---
        auto drawer_content = std::make_shared<FlexBox>();
        drawer_content->flexDirection(YGFlexDirectionColumn)
                      .padding(YGEdgeAll, 24.0f)
                      .widthPercent(100).heightPercent(100);
                      
        // Profile Section
        auto profile_row = std::make_shared<FlexBox>();
        profile_row->flexDirection(YGFlexDirectionRow).alignItems(YGAlignCenter).margin(YGEdgeBottom, 24.0f);
        
        AvatarConfig av_cfg;
        av_cfg.size = 64.0f;
        av_cfg.initials = "JD";
        av_cfg.background_color = 0xFF6366F1;
        
        auto user_info = std::make_shared<FlexBox>();
        user_info->flexDirection(YGFlexDirectionColumn).justifyContent(YGJustifyCenter).margin(YGEdgeLeft, 16.0f);
        user_info->child(text("John Doe", {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        user_info->child(text("john@vaura.dev", {.font_size = 14.0f, .color = 0xFF94A3B8}));
        
        profile_row->child(avatar(av_cfg)).child(user_info);
        drawer_content->child(profile_row);
        
        DividerConfig div_cfg;
        div_cfg.color = 0xFF334155;
        drawer_content->child(divider(div_cfg));
        
        // Navigation Links (Simulated)
        auto make_nav_link = [](const std::string& label, Color txt_color) {
            auto btn_cfg = ButtonConfig{};
            btn_cfg.child = text(label, {.font_size = 16.0f, .color = txt_color});
            btn_cfg.color = 0x00000000;
            btn_cfg.width_percent = 100.0f;
            btn_cfg.on_pressed = [](){};
            
            auto wrap = std::make_shared<FlexBox>();
            wrap->margin(YGEdgeTop, 12.0f).child(button(btn_cfg));
            return wrap;
        };
        
        drawer_content->child(make_nav_link("Dashboard", 0xFFF8FAFC));
        drawer_content->child(make_nav_link("Projects", 0xFF94A3B8));
        drawer_content->child(make_nav_link("Team", 0xFF94A3B8));
        drawer_content->child(make_nav_link("Settings", 0xFF94A3B8));

        // --- Main Body ---
        auto body = std::make_shared<FlexBox>();
        body->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);
            
        body->child(text("Main Screen", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        body->child(text("Click the button below to open the drawer.", {.font_size = 16.0f, .color = 0xFF94A3B8}));
             
        ButtonConfig open_cfg;
        open_cfg.child = text("Open Drawer", {.font_size = 16.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold});
        open_cfg.color = 0xFF3B82F6;
        open_cfg.on_pressed = [this]() {
            setState([this]() { is_open = true; });
        };
        
        auto btn_wrap = std::make_shared<FlexBox>();
        btn_wrap->margin(YGEdgeTop, 32.0f).child(button(open_cfg));
        body->child(btn_wrap);

        // --- Drawer Wrapper ---
        DrawerConfig drawer_cfg;
        drawer_cfg.body = body;
        drawer_cfg.child = drawer_content;
        drawer_cfg.is_open = is_open;
        drawer_cfg.side = DrawerConfig::Side::Left;
        drawer_cfg.on_close = [this]() {
            setState([this]() { is_open = false; });
        };
        
        return drawer(drawer_cfg);
    }
};

class DrawerDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "DrawerDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<DrawerDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Drawer Demo";
    config.width = 800;
    config.height = 600;

    return runApp(std::make_shared<DrawerDemo>(), config);
}
