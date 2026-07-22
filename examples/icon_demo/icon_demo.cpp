#include "vaura/app/app.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/font_manager.hpp"
#include <iostream>

using namespace vaura;

class IconDemoApp : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "IconDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter);

        auto content = std::make_shared<FlexBox>();
        content->flexDirection(YGFlexDirectionColumn)
               .alignItems(YGAlignCenter);

        auto title = Text("Cupertino Icons Demo", {
            .font_size = 28,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        auto title_container = std::make_shared<FlexBox>();
        title_container->margin(YGEdgeBottom, 40).child(title);
        content->child(title_container);

        auto icon_row = std::make_shared<FlexBox>();
        icon_row->flexDirection(YGFlexDirectionRow)
                .justifyContent(YGJustifyCenter);

        auto pad_icon = [](IconData icon_data, uint32_t color) {
            auto wrapper = std::make_shared<FlexBox>();
            wrapper->margin(YGEdgeHorizontal, 15).child(Icon(icon_data, {.size = 56, .color = color}));
            return wrapper;
        };

        // Cupertino Icons using pre-defined constants
        icon_row->child(pad_icon(Icons::Home, 0xFF38BDF8))
                 .child(pad_icon(Icons::Search, 0xFF4ADE80))
                 .child(pad_icon(Icons::Settings, 0xFFFBBF24))
                 .child(pad_icon(Icons::Heart, 0xFFF87171))
                 .child(pad_icon(Icons::Star, 0xFFFACC15));

        content->child(icon_row);
        root->child(content);

        return root;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Icon Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    bool font_loaded = FontManager::loadFont("examples/icon_demo/CupertinoIcons.ttf", "CupertinoIcons");
    if (!font_loaded) {
        std::cerr << "Failed to load CupertinoIcons.ttf. Please make sure the TTF file is in examples/icon_demo/." << std::endl;
    } else {
        std::cout << "Successfully loaded CupertinoIcons." << std::endl;
    }
    
    return vaura::runApp(std::make_shared<IconDemoApp>(), config);
}
