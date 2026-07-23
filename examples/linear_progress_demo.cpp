#include "vaura/app/app.hpp"
#include "vaura/widgets/linear_progress.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/card.hpp"
#include "vaura/state/state.hpp"

using namespace vaura;

class LinearProgressDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "LinearProgressDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class LinearProgressDemoState : public State {
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .justify(Justify::Center)
            .align(Align::Center);

        auto content_col = std::make_shared<FlexBox>();
        content_col->direction(FlexDirection::Column)
                   .align(Align::Stretch)
                   .width(300);

        // Indeterminate Progress
        auto title1 = Text("Indeterminate Progress", { .font_size = 18, .color = 0xFFFFFFFF, .weight = TextStyle::Bold });
        auto wrap_title1 = std::make_shared<FlexBox>(); wrap_title1->margin(Edge::Bottom, 10).child(title1);
        auto ind_prog = LinearProgress({
            .value = std::nullopt,
            .thickness = 6.0f,
            .border_radius = 3.0f,
            .active_color = 0xFF3B82F6, // Blue
            .inactive_color = 0xFF334155  // Slate 700
        });
        auto wrap_ind = std::make_shared<FlexBox>(); wrap_ind->margin(Edge::Bottom, 30).child(ind_prog);

        // Determinate Progress
        auto title2 = Text("Determinate Progress (75%)", { .font_size = 18, .color = 0xFFFFFFFF, .weight = TextStyle::Bold });
        auto wrap_title2 = std::make_shared<FlexBox>(); wrap_title2->margin(Edge::Bottom, 10).child(title2);
        auto det_prog = LinearProgress({
            .value = 0.75f,
            .thickness = 6.0f,
            .border_radius = 3.0f,
            .active_color = 0xFF10B981, // Emerald
            .inactive_color = 0xFF334155  // Slate 700
        });

        content_col->child(wrap_title1);
        content_col->child(wrap_ind);
        content_col->child(wrap_title2);
        content_col->child(det_prog);

        auto c = Card({
            .color = 0xFF0F172A,
            .border_radius = 20.0f,
            .elevation = 15.0f,
            .shadow_color = 0x80000000,
            .padding = {30, 30, 30, 30}
        }, content_col);

        root->child(c);
        return root;
    }
};

std::unique_ptr<State> LinearProgressDemoApp::createState() {
    return std::make_unique<LinearProgressDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA LinearProgress Demo",
        .width = 600,
        .height = 400,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<LinearProgressDemoApp>(), config);
}
