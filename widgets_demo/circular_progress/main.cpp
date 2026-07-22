#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/circular_progress.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class CircularProgressDemo : public StatelessWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CircularProgressDemo"; }

    WidgetPtr build(BuildContext& context) override {
        // Main Background Body
        auto body = std::make_shared<FlexBox>();
        body->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .gap(YGGutterAll, 60.0f)
            .widthPercent(100.0f)
            .heightPercent(100.0f)
            .backgroundColor(0xFF0F172A); // Slate 900

        TextConfig title_cfg;
        title_cfg.font_size = 32.0f;
        title_cfg.color = 0xFFFFFFFF;
        body->child(std::make_shared<TextWidget>("Loading States", title_cfg));

        // Row for indicators
        auto row = std::make_shared<FlexBox>();
        row->flexDirection(YGFlexDirectionRow)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .gap(YGGutterAll, 80.0f);

        // 1. Indeterminate Progress
        auto ind_col = std::make_shared<FlexBox>();
        ind_col->flexDirection(YGFlexDirectionColumn)
               .alignItems(YGAlignCenter)
               .gap(YGGutterAll, 16.0f);
               
        CircularProgressConfig ind_cfg;
        ind_cfg.size = 80.0f;
        ind_cfg.stroke_width = 8.0f;
        ind_cfg.color = 0xFF8B5CF6; // Violet 500
        ind_col->child(CircularProgress(ind_cfg));
        
        TextConfig ind_txt;
        ind_txt.font_size = 18.0f;
        ind_txt.color = 0xFF94A3B8;
        ind_col->child(std::make_shared<TextWidget>("Indeterminate", ind_txt));
        
        row->child(ind_col);

        // 2. Determinate Progress
        auto det_col = std::make_shared<FlexBox>();
        det_col->flexDirection(YGFlexDirectionColumn)
               .alignItems(YGAlignCenter)
               .gap(YGGutterAll, 16.0f);
               
        CircularProgressConfig det_cfg;
        det_cfg.value = 0.75f; // 75%
        det_cfg.size = 80.0f;
        det_cfg.stroke_width = 8.0f;
        det_cfg.color = 0xFF10B981; // Emerald 500
        det_cfg.background_color = 0xFF1E293B; // Slate 800
        det_col->child(CircularProgress(det_cfg));
        
        TextConfig det_txt;
        det_txt.font_size = 18.0f;
        det_txt.color = 0xFF94A3B8;
        det_col->child(std::make_shared<TextWidget>("75%", det_txt));

        row->child(det_col);
        body->child(row);

        return body;
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Circular Progress Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<CircularProgressDemo>(), config);
}
