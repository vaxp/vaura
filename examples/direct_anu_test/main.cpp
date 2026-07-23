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

        auto badge = Column({
            .positionType = PositionType::Absolute,
            .backgroundColor = 0xFFEF4444, // Red
            .children = {
                Text("SALE", {
                    .font_size = 11,
                    .color = 0xFFFFFFFF,
                    .weight = TextStyle::Bold
                })
            }
        });
        badge->position(Edge::Top, 12.0f)
             .position(Edge::Right, 12.0f)
             .padding(Edge::Horizontal, 10.0f)
             .padding(Edge::Vertical, 4.0f);

        auto image_container = Column({
            .widthPercent = 100.0f,
            .height = 180.0f,
            .margin = std::pair{Edge::Bottom, 16.0f},
            .positionType = PositionType::Relative,
            .children = {
                ImageView({
                    .path = "/home/vaxp/Downloads/VAURA/examples/direct_anu_test/logo.png",
                    .width_percent = 100.0f,
                    .height_percent = 100.0f,
                    .fit = BoxFit::Cover,
                    .border_radius = 12.0f
                }),
                badge
            }
        });

        auto buy_btn = Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .backgroundColor = btn_color,
            .children = {
                Text("Add", {
                    .font_size = 14,
                    .color = 0xFFFFFFFF,
                    .weight = TextStyle::Medium
                })
            }
        });
        buy_btn->padding(Edge::Horizontal, 20.0f)
               .padding(Edge::Vertical, 10.0f);

        auto action_row = Row({
            .justify = Justify::SpaceBetween,
            .align = Align::Center,
            .margin = std::pair{Edge::Top, 12.0f},
            .children = {
                Text(widget_ptr->price, {
                    .font_size = 20,
                    .color = 0xFF4F46E5,
                    .weight = TextStyle::ExtraBold
                }),
                buy_btn
            }
        });

        auto card = Column({
            .width = 260.0f,
            .padding = std::pair{Edge::All, 16.0f},
            .backgroundColor = 0xFFFFFFFF,
            .borderRadius = 12.0f,
            .children = {
                image_container,
                Text(widget_ptr->title, {
                    .font_size = 18,
                    .color = 0xFF111827,
                    .weight = TextStyle::SemiBold
                }),
                Text("In stock", {
                    .font_size = 13,
                    .color = 0xFF10B981,
                    .weight = TextStyle::Medium
                }),
                Column({ .height = 12.0f }),
                action_row
            }
        });

        card->margin(Edge::Right, 24.0f)
            .margin(Edge::Top, top_margin)
            .margin(Edge::Bottom, bottom_margin);

        return GestureDetector({
            .child = card,
            .on_hover_enter = [this]() { controller_.forward(); },
            .on_hover_exit = [this]() { controller_.reverse(); }
        });
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
        std::vector<std::string> categories = {"Electronics", "Clothing", "Home & Garden", "Sports", "Toys", "Automotive"};
        std::vector<WidgetPtr> sidebar_items;
        sidebar_items.push_back(Text("Categories", {
            .font_size = 16,
            .color = 0xFF9CA3AF,
            .weight = TextStyle::SemiBold
        }));
        
        for (size_t i = 0; i < categories.size(); ++i) {
            auto cat_item = Column({
                .margin = std::pair{Edge::Top, 16.0f},
                .backgroundColor = (Color)(i == 0 ? 0xFFEEF2FF : 0x00000000),
                .children = {
                    Text(categories[i], {
                        .font_size = 15,
                        .color = (Color)(i == 0 ? 0xFF4F46E5 : 0xFF4B5563),
                        .weight = i == 0 ? TextStyle::Bold : TextStyle::Medium
                    })
                }
            });
            cat_item->padding(Edge::Vertical, 8.0f)
                    .padding(Edge::Horizontal, 12.0f);
            sidebar_items.push_back(cat_item);
        }

        std::vector<std::pair<std::string, std::string>> products = {
            {"Premium Headphones", "$299.99"},
            {"Mechanical Keyboard", "$149.50"},
            {"Wireless Mouse", "$79.00"},
            {"4K Ultra Monitor", "$399.00"},
            {"USB-C Hub Pro", "$45.99"},
            {"Gaming Desk Mat", "$29.99"}
        };
        std::vector<WidgetPtr> grid_items;
        for (const auto& prod : products) {
            grid_items.push_back(std::make_shared<AnimatedProductCard>(prod.first, prod.second));
        }

        return Column({
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFFF0F2F5,
            .children = {
                // --- Header ---
                Row({
                    .justify = Justify::SpaceBetween,
                    .align = Align::Center,
                    .height = 70.0f,
                    .padding = std::pair{Edge::Horizontal, 32.0f},
                    .backgroundColor = 0xFF1E1B4B,
                    .children = {
                        Text("VAURA Store", {
                            .font_size = 26,
                            .color = 0xFFFFFFFF,
                            .weight = TextStyle::ExtraBold,
                            .letter_spacing = 1.2f
                        }),
                        Row({
                            .align = Align::Center,
                            .children = {
                                Text("CART (3)", {
                                    .font_size = 14,
                                    .color = 0xFF818CF8,
                                    .weight = TextStyle::Bold
                                }),
                                Column({
                                    .justify = Justify::Center,
                                    .align = Align::Center,
                                    .width = 40.0f,
                                    .height = 40.0f,
                                    .margin = std::pair{Edge::Left, 24.0f},
                                    .backgroundColor = 0xFF4F46E5,
                                    .children = {
                                        Text("U", {
                                            .font_size = 18,
                                            .color = 0xFFFFFFFF,
                                            .weight = TextStyle::Bold
                                        })
                                    }
                                })
                            }
                        })
                    }
                }),
                
                // --- Main Content Area ---
                Row({
                    .flexGrow = 1.0f,
                    .children = {
                        // Sidebar
                        Column({
                            .width = 240.0f,
                            .padding = std::pair{Edge::All, 24.0f},
                            .backgroundColor = 0xFFFFFFFF,
                            .children = sidebar_items
                        }),
                        
                        // Product Grid Area
                        Row({
                            .justify = Justify::Start,
                            .align = Align::Start,
                            .wrap = FlexWrap::Wrap,
                            .flexGrow = 1.0f,
                            .padding = std::pair{Edge::All, 32.0f},
                            .children = grid_items
                        })
                    }
                })
            }
        });
    }

    std::string_view typeName() const override { return "TestApp"; }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Direct Anu Storefront - Animated Edition";
    config.width = 1120;
    config.height = 860;
    config.vsync = true;

    return vaura::runApp(std::make_shared<TestApp>(), config);
}
