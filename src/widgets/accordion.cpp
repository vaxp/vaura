#include "vaura/widgets/accordion.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/widgets/opacity.hpp"
#include "vaura/state/state.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"

namespace vaura {

class AccordionWidget : public StatefulWidget {
public:
    AccordionConfig config;

    explicit AccordionWidget(AccordionConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "Accordion"; }
    std::unique_ptr<State> createState() override;
};

class AccordionState : public State {
    bool is_expanded = false;
    std::shared_ptr<AnimationController> anim;
    std::unique_ptr<Ticker> ticker;

public:
    void initState() override {
        State::initState();
        auto w = static_cast<const AccordionWidget*>(widget());
        is_expanded = w->config.initially_expanded;
        anim = std::make_shared<AnimationController>(AnimationController::Duration{300}); // 300ms
        if (is_expanded) {
            anim->setValue(1.0f);
        }
        
        anim->addListener([this]() {
            setState([]{});
        });

        ticker = createTicker([this]() {
            anim->tick();
        });
        ticker->start();
    }

    void dispose() override {
        if (ticker) ticker->stop();
        anim->dispose();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto w = static_cast<const AccordionWidget*>(widget());
        const auto& config = w->config;

        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .widthPercent(100);

        // Header Row
        auto header_row = std::make_shared<FlexBox>();
        header_row->flexDirection(YGFlexDirectionRow)
                  .alignItems(YGAlignCenter)
                  .justifyContent(YGJustifySpaceBetween)
                  .widthPercent(100)
                  .padding(YGEdgeAll, 16);

        auto title_text = text(config.title, {
            .font_size = 18,
            .color = config.text_color,
            .weight = TextStyle::Bold
        });
        
        // Use a simple text symbol for the chevron, rotated
        auto chevron = text("v", {
            .font_size = 18,
            .color = config.icon_color,
            .weight = TextStyle::Bold
        });

        // Rotate chevron from 0 to -180 degrees (upwards)
        float rotation = anim->value() * -180.0f;
        auto rotated_chevron = transform({.rotation = rotation, .child = chevron});

        header_row->child(title_text).child(rotated_chevron);

        auto header_container = container({
            .color = config.background_color,
            .width_percent = 100.0f,
            .border_radius = BorderRadius::circular(8.0f),
            .child = header_row
        });

        GestureDetectorConfig gd_config;
        gd_config.child = header_container;
        gd_config.on_tap = [this]() {
            is_expanded = !is_expanded;
            if (is_expanded) {
                anim->forward();
            } else {
                anim->reverse();
            }
        };
        auto clickable_header = gesture_detector(std::move(gd_config));

        root->child(clickable_header);

        // Content
        if (anim->value() > 0.0f && config.content) {
            auto content_container = container({
                .width_percent = 100.0f,
                .padding = {16, 16, 16, 16},
                .child = config.content
            });
            
            auto faded_content = opacity(anim->value(), content_container);
            root->child(faded_content);
        }

        return root;
    }
};

std::unique_ptr<State> AccordionWidget::createState() {
    return std::make_unique<AccordionState>();
}

std::shared_ptr<Widget> accordion(AccordionConfig config) {
    return std::make_shared<AccordionWidget>(std::move(config));
}

} // namespace vaura
