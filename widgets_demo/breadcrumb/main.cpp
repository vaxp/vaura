#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/breadcrumb.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"
#include <iostream>

using namespace vaura;

class BreadcrumbDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "BreadcrumbDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class BreadcrumbDemoState : public State {
    std::string current_path = "VAURA";

public:
    WidgetPtr build(BuildContext& context) override {
        // Main Background Body
        auto body = std::make_shared<FlexBox>();
        body->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .gap(YGGutterAll, 40.0f)
            .widthPercent(100.0f)
            .heightPercent(100.0f)
            .backgroundColor(0xFF0F172A); // Slate 900

        // Title showing current path
        TextConfig title_cfg;
        title_cfg.font_size = 32.0f;
        title_cfg.color = 0xFFF8FAFC;
        body->child(std::make_shared<TextWidget>("Current: " + current_path, title_cfg));

        // Breadcrumb Configuration
        BreadcrumbConfig crumb_cfg;
        crumb_cfg.separator = ">";
        crumb_cfg.active_color = 0xFF22D3EE; // Cyan 400
        crumb_cfg.inactive_color = 0xFF64748B; // Slate 500
        crumb_cfg.hover_color = 0xFF94A3B8;
        crumb_cfg.separator_color = 0xFF475569;
        crumb_cfg.font_size = 20.0f;

        // Items setup
        auto make_nav = [this](const std::string& path) {
            return [this, path]() {
                setState([this, path]() {
                    current_path = path;
                });
            };
        };

        // Generate the breadcrumb trail dynamically
        std::vector<std::string> full_path = {"Home", "Documents", "Projects", "VAURA"};
        
        for (const auto& path_segment : full_path) {
            if (path_segment == current_path) {
                // This is the current page. We make it clickable (no-op or redundant state update)
                // to maintain the same widget tree structure and avoid segfaults caused by
                // replacing a GestureDetector with a Text widget while processing its click event.
                crumb_cfg.items.push_back({path_segment, make_nav(path_segment)});
                break; // Stop adding items after the current page
            } else {
                // This is a parent page, so it's clickable
                crumb_cfg.items.push_back({path_segment, make_nav(path_segment)});
            }
        }

        // Breadcrumb Wrapper for styling
        auto crumb_wrapper = std::make_shared<FlexBox>();
        crumb_wrapper->backgroundColor(0xFF1E293B) // Slate 800
                     .padding(YGEdgeAll, 16.0f)
                     .borderRadius(12.0f);
        crumb_wrapper->child(breadcrumb(crumb_cfg));

        body->child(crumb_wrapper);

        return body;
    }
};

std::unique_ptr<State> BreadcrumbDemo::createState() {
    return std::make_unique<BreadcrumbDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Breadcrumb Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<BreadcrumbDemo>(), config);
}
