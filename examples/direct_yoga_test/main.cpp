#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/image_widget.hpp"
#include "vaura/animation/animation_controller.hpp"
#include "vaura/animation/ticker.hpp"
#include "vaura/animation/tween.hpp"
#include "vaura/state/state.hpp"
#include <memory>
#include <vector>
#include <string>
#include <cmath>

using namespace vaura;

// ============================================================================
// Animated Product Card
// ============================================================================
class AnimatedProductCard : public StatefulWidget {
public:
    std::string title;
    std::string price;

    AnimatedProductCard(std::string t, std::string p) : title(std::move(t)), price(std::move(p)) {}

    std::unique_ptr<State> createState() override;
    std::string_view typeName() const override { return "AnimatedProductCard"; }
};

class AnimatedProductCardState : public State {
    AnimationController controller_;
    std::unique_ptr<Ticker> ticker_;
    FloatTween margin_tween_{24.0f, 10.0f};

public:
    AnimatedProductCardState() : controller_(std::chrono::milliseconds(250)) {}

    void initState() override {
        State::initState();
        ticker_ = createTicker([this]() {
            if (controller_.tick()) {
                setState([]{});
            }
        });
        ticker_->start();
    }

    void dispose() override {
        if (ticker_) ticker_->stop();
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* widget_ptr = static_cast<const AnimatedProductCard*>(this->widget());
        
        float progress = controller_.value();
        // Custom ease-out curve mapping
        float t = 1.0f - std::pow(1.0f - progress, 3.0f); 
        
        float top_margin = margin_tween_.evaluate(t);
        float bottom_margin = 48.0f - top_margin;

        // Manually interpolate colors to avoid hex math weirdness
        auto lerp_channel = [](uint32_t a, uint32_t b, float t) {
            return static_cast<uint32_t>(a + (b - a) * t);
        };
        
        uint32_t start_btn = 0xFF111827;
        uint32_t end_btn   = 0xFF4F46E5;
        
        uint32_t r = lerp_channel((start_btn >> 16) & 0xFF, (end_btn >> 16) & 0xFF, t);
        uint32_t g = lerp_channel((start_btn >> 8) & 0xFF, (end_btn >> 8) & 0xFF, t);
        uint32_t b = lerp_channel(start_btn & 0xFF, end_btn & 0xFF, t);
        uint32_t btn_color = 0xFF000000 | (r << 16) | (g << 8) | b;

        auto card = std::make_shared<FlexBox>();
        card->width(260)
            .margin(YGEdgeRight, 24)
            .margin(YGEdgeTop, top_margin)
            .margin(YGEdgeBottom, bottom_margin)
            .padding(YGEdgeAll, 16)
            .backgroundColor(0xFFFFFFFF)
            .borderRadius(12)
            .flexDirection(YGFlexDirectionColumn);

        // Image container (relative for absolute badge)
        auto image_container = std::make_shared<FlexBox>();
        image_container->widthPercent(100)
                       .height(180)
                       .margin(YGEdgeBottom, 16)
                       .positionType(YGPositionTypeRelative);
        
        auto prod_image = ImageView({
            .path = "/home/vaxp/Downloads/VAURA/examples/direct_yoga_test/logo.png",
            .width_percent = 100.0f,
            .height_percent = 100.0f,
            .fit = BoxFit::Cover,
            .border_radius = 12.0f
        });

        // Sale badge
        auto badge = std::make_shared<FlexBox>();
        badge->positionType(YGPositionTypeAbsolute)
             .position(YGEdgeTop, 12)
             .position(YGEdgeRight, 12)
             .padding(YGEdgeHorizontal, 10)
             .padding(YGEdgeVertical, 4)
             .backgroundColor(0xFFEF4444); // Red
        
        badge->child(Text("SALE", {
            .font_size = 11,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        }));

        image_container->child(prod_image);
        image_container->child(badge);

        auto prod_title = Text(widget_ptr->title, {
            .font_size = 18,
            .color = 0xFF111827,
            .weight = TextStyle::SemiBold
        });

        auto prod_subtitle = Text("In stock", {
            .font_size = 13,
            .color = 0xFF10B981,
            .weight = TextStyle::Medium
        });

        auto spacer = std::make_shared<FlexBox>();
        spacer->height(12);

        auto action_row = std::make_shared<FlexBox>();
        action_row->flexDirection(YGFlexDirectionRow)
                  .justifyContent(YGJustifySpaceBetween)
                  .alignItems(YGAlignCenter)
                  .margin(YGEdgeTop, 12);

        auto price_text = Text(widget_ptr->price, {
            .font_size = 20,
            .color = 0xFF4F46E5,
            .weight = TextStyle::ExtraBold
        });

        auto buy_btn = std::make_shared<FlexBox>();
        buy_btn->padding(YGEdgeHorizontal, 20)
               .padding(YGEdgeVertical, 10)
               .backgroundColor(btn_color)
               .justifyContent(YGJustifyCenter)
               .alignItems(YGAlignCenter);
        
        buy_btn->child(Text("Add", {
            .font_size = 14,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Medium
        }));

        action_row->child(price_text);
        action_row->child(buy_btn);

        card->child(image_container);
        card->child(prod_title);
        card->child(prod_subtitle);
        card->child(spacer);
        card->child(action_row);

        auto gesture = GestureDetector({
            .child = card,
            .on_hover_enter = [this]() { controller_.forward(); },
            .on_hover_exit = [this]() { controller_.reverse(); }
        });

        return gesture;
    }
};

std::unique_ptr<State> AnimatedProductCard::createState() {
    return std::make_unique<AnimatedProductCardState>();
}


// ============================================================================
// Main Store App
// ============================================================================
class TestApp : public StatelessWidget {
public:
    WidgetPtr build(BuildContext& ctx) override {
        // Root Container
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .flexDirection(YGFlexDirectionColumn)
            .backgroundColor(0xFFF0F2F5);

        // --- Header ---
        auto header = std::make_shared<FlexBox>();
        header->height(70)
              .flexDirection(YGFlexDirectionRow)
              .justifyContent(YGJustifySpaceBetween)
              .alignItems(YGAlignCenter)
              .padding(YGEdgeHorizontal, 32)
              .backgroundColor(0xFF1E1B4B);

        auto store_title = Text("VAURA Store", {
            .font_size = 26,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::ExtraBold,
            .letter_spacing = 1.2f
        });

        auto nav_actions = std::make_shared<FlexBox>();
        nav_actions->flexDirection(YGFlexDirectionRow)
                   .alignItems(YGAlignCenter);

        auto cart_icon = Text("CART (3)", {
            .font_size = 14,
            .color = 0xFF818CF8,
            .weight = TextStyle::Bold
        });
        
        auto profile_btn = std::make_shared<FlexBox>();
        profile_btn->width(40).height(40)
                   .margin(YGEdgeLeft, 24)
                   .backgroundColor(0xFF4F46E5)
                   .justifyContent(YGJustifyCenter)
                   .alignItems(YGAlignCenter);
        
        profile_btn->child(Text("U", {
            .font_size = 18,
            .color = 0xFFFFFFFF,
            .weight = TextStyle::Bold
        }));

        nav_actions->child(cart_icon);
        nav_actions->child(profile_btn);

        header->child(store_title);
        header->child(nav_actions);

        // --- Main Content Area ---
        auto main_area = std::make_shared<FlexBox>();
        main_area->flexGrow(1.0f)
                 .flexDirection(YGFlexDirectionRow);

        // Sidebar
        auto sidebar = std::make_shared<FlexBox>();
        sidebar->width(240)
               .padding(YGEdgeAll, 24)
               .backgroundColor(0xFFFFFFFF)
               .flexDirection(YGFlexDirectionColumn);

        auto cat_title = Text("Categories", {
            .font_size = 16,
            .color = 0xFF9CA3AF,
            .weight = TextStyle::SemiBold
        });
        sidebar->child(cat_title);

        std::vector<std::string> categories = {"Electronics", "Clothing", "Home & Garden", "Sports", "Toys", "Automotive"};
        for (size_t i = 0; i < categories.size(); ++i) {
            auto cat_item = std::make_shared<FlexBox>();
            cat_item->margin(YGEdgeTop, 16)
                    .padding(YGEdgeVertical, 8)
                    .padding(YGEdgeHorizontal, 12)
                    .backgroundColor(i == 0 ? 0xFFEEF2FF : 0x00000000);

            cat_item->child(Text(categories[i], {
                .font_size = 15,
                .color = (Color)(i == 0 ? 0xFF4F46E5 : 0xFF4B5563),
                .weight = i == 0 ? TextStyle::Bold : TextStyle::Medium
            }));
            sidebar->child(cat_item);
        }

        // Product Grid Area
        auto grid_area = std::make_shared<FlexBox>();
        grid_area->flexGrow(1.0f)
                 .padding(YGEdgeAll, 32)
                 .flexDirection(YGFlexDirectionRow)
                 .flexWrap(YGWrapWrap)
                 .justifyContent(YGJustifyFlexStart)
                 .alignItems(YGAlignFlexStart);

        std::vector<std::pair<std::string, std::string>> products = {
            {"Premium Headphones", "$299.99"},
            {"Mechanical Keyboard", "$149.50"},
            {"Wireless Mouse", "$79.00"},
            {"4K Ultra Monitor", "$399.00"},
            {"USB-C Hub Pro", "$45.99"},
            {"Gaming Desk Mat", "$29.99"}
        };

        for (const auto& prod : products) {
            // Using the Animated Stateful Widget we built!
            grid_area->child(std::make_shared<AnimatedProductCard>(prod.first, prod.second));
        }

        main_area->child(sidebar);
        main_area->child(grid_area);

        root->child(header);
        root->child(main_area);

        return root;
    }

    std::string_view typeName() const override { return "TestApp"; }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Direct Yoga Storefront - Animated Edition";
    config.width = 1280;
    config.height = 800;
    config.vsync = true;

    return vaura::runApp(std::make_shared<TestApp>(), config);
}
