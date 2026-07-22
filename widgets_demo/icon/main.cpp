// widgets_demo/icon/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class IconDemoState : public State {
    int hovered_idx = -1;

public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        auto title = std::make_shared<FlexBox>();
        title->margin(YGEdgeBottom, 40)
             .child(text("Icon Gallery", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        root->child(title);

        auto grid = std::make_shared<FlexBox>();
        grid->flexDirection(YGFlexDirectionRow)
            .flexWrap(YGWrapWrap)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .width(500);

        std::vector<std::pair<IconData, std::string>> icons = {
            {Icons::House, "Home"},
            {Icons::Gear, "Settings"},
            {Icons::Person, "User"},
            {Icons::Heart, "Favorite"},
            {Icons::Star, "Rate"}
        };

        for (size_t i = 0; i < icons.size(); ++i) {
            bool is_hovered = (static_cast<int>(i) == hovered_idx);
            
            auto item_box = std::make_shared<FlexBox>();
            item_box->flexDirection(YGFlexDirectionColumn)
                    .justifyContent(YGJustifyCenter)
                    .alignItems(YGAlignCenter)
                    .width(120).height(120)
                    .margin(YGEdgeAll, 10)
                    .backgroundColor(is_hovered ? 0xFF334155 : 0xFF1E293B)
                    .borderRadius(16.0f);
            
            IconConfig icfg;
            icfg.size = is_hovered ? 56.0f : 48.0f; // Scale up on hover
            icfg.color = is_hovered ? 0xFF0EA5E9 : 0xFF94A3B8; // Color change
            
            item_box->child(icon(icons[i].first, icfg));
            
            auto t = std::make_shared<FlexBox>();
            t->margin(YGEdgeTop, 12).child(text(icons[i].second, {.font_size = 14.0f, .color = is_hovered ? 0xFFF8FAFC : 0xFF94A3B8}));
            item_box->child(t);
            
            GestureDetectorConfig gd_cfg;
            gd_cfg.child = item_box;
            gd_cfg.on_hover_enter = [this, i]() { setState([this, i]() { hovered_idx = i; }); };
            gd_cfg.on_hover_exit = [this, i]() { setState([this, i]() { if (hovered_idx == static_cast<int>(i)) hovered_idx = -1; }); };
            
            grid->child(gesture_detector(gd_cfg));
        }

        root->child(grid);
        return root;
    }
};

class IconDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "IconDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<IconDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Icon Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<IconDemo>(), config);
}
