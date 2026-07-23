#include "vaura/app/app.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include <iostream>

using namespace vaura;

class AnimationsDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "AnimationsDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class AnimationsDemoState : public State {
private:
    AnimatedValue<float> animation_{0.0f, 1.0f, std::chrono::milliseconds{2000}, &Curves::elasticOut};
    std::unique_ptr<Ticker> ticker_;

public:
    void initState() override {
        State::initState();
        
        animation_.controller().setRepeats(true);
        animation_.controller().setPingPong(true);
        
        ticker_ = createTicker([this]() {
            if (animation_.tick()) {
                setState([]{});
            }
        });
        
        animation_.forward();
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) {
            ticker_->stop();
        }
        animation_.dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .backgroundColor(0xFF1E293B)
            .align(Align::Center)
            .justify(Justify::Center);

        float t = animation_.get();

        // Animate scale from 0.5 to 1.5
        float current_scale = 0.5f + t * 1.0f;
        // Animate rotation from 0 to 360
        float current_rotation = t * 360.0f;
        // Animate opacity from 0.2 to 1.0
        float current_opacity = 0.2f + t * 0.8f;

        auto box = std::make_shared<FlexBox>();
        box->width(150)
           .height(150)
           .backgroundColor(0xFF38BDF8)
           .borderRadius(20)
           .align(Align::Center)
           .justify(Justify::Center);
           
        auto text_widget = Text("VAURA", {
            .font_size = 24,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        });
        
        box->child(text_widget);

        auto animated_box = Opacity(current_opacity, 
            Transform({
                .scale_x = current_scale,
                .scale_y = current_scale,
                .rotation = current_rotation,
                .child = box
            })
        );

        root->child(animated_box);
        return root;
    }
};

std::unique_ptr<State> AnimationsDemoApp::createState() {
    return std::make_unique<AnimationsDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA Animations Demo",
        .width = 800,
        .height = 600,
        .vsync = true
    };
    
    return vaura::runApp(std::make_shared<AnimationsDemoApp>(), config);
}
