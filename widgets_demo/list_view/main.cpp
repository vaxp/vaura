// widgets_demo/list_view/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/widgets/list_tile.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/icon.hpp"

using namespace vaura;

class ListViewDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "ListViewDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        root->child(Text("List View Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(20);
        root->child(spacer);

        auto list_container = std::make_shared<FlexBox>();
        list_container->flexDirection(YGFlexDirectionColumn);

        // Add 50 items
        for (int i = 1; i <= 50; ++i) {
            ListTileConfig tile_cfg;
            tile_cfg.title = Text("List Item " + std::to_string(i), {.font_size = 16.0f, .color = 0xFFFFFFFF});
            tile_cfg.subtitle = Text("This is a detailed description for item " + std::to_string(i), {.font_size = 14.0f, .color = 0xFF94A3B8});
            
            // Alternating icons
            if (i % 3 == 0) {
                tile_cfg.leading = Icon(Icons::HeartFill, {.size=24, .color=0xFFEF4444});
            } else if (i % 3 == 1) {
                tile_cfg.leading = Icon(Icons::Settings, {.size=24, .color=0xFF3B82F6});
            } else {
                tile_cfg.leading = Icon(Icons::PersonFill, {.size=24, .color=0xFF10B981});
            }
            
            tile_cfg.trailing = Icon(Icons::ChevronRight, {.size=24, .color=0xFF94A3B8});
            tile_cfg.on_tap = [i]() { /* Handle tap */ };

            auto tile_wrapper = std::make_shared<FlexBox>();
            tile_wrapper->margin(YGEdgeAll, 10.0f);
            tile_wrapper->child(ListTile(tile_cfg));
            list_container->child(tile_wrapper);
            
            // Add a separator except for the last item
            if (i < 50) {
                auto sep = std::make_shared<FlexBox>();
                sep->height(1).backgroundColor(0xFF1E293B).margin(YGEdgeHorizontal, 16);
                list_container->child(sep);
            }
        }

        ScrollViewConfig scroll_cfg;
        scroll_cfg.child = list_container;
        scroll_cfg.show_scrollbar = true;
        auto scroll = ScrollView(scroll_cfg);

        auto list_wrapper = std::make_shared<FlexBox>();
        list_wrapper->width(600).height(500)
                    .backgroundColor(0xFF1E293B)
                    .borderRadius(16)
                    .overflow(YGOverflowHidden)
                    .child(scroll);

        root->child(list_wrapper);
        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA List View Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<ListViewDemo>(), config);
}
