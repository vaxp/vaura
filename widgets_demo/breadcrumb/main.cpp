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
        // Items setup
        auto make_nav = [this](const std::string& path) {
            return [this, path]() {
                setState([this, path]() {
                    current_path = path;
                });
            };
        };

        // Breadcrumb Configuration
        BreadcrumbConfig crumb_cfg;
        crumb_cfg.separator = ">";
        crumb_cfg.active_color = 0xFF22D3EE; // Cyan 400
        crumb_cfg.inactive_color = 0xFF64748B; // Slate 500
        crumb_cfg.hover_color = 0xFF94A3B8;
        crumb_cfg.separator_color = 0xFF475569;
        crumb_cfg.font_size = 20.0f;

        // Generate the breadcrumb trail dynamically
        std::vector<std::string> full_path = {"Home", "Documents", "Projects", "VAURA"};
        
        for (const auto& path_segment : full_path) {
            crumb_cfg.items.push_back({path_segment, make_nav(path_segment)});
            if (path_segment == current_path) break;
        }

        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .gap = 40.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                // Title showing current path
                Text("Current: " + current_path, {.font_size = 32.0f, .color = 0xFFF8FAFC}),

                // Breadcrumb Wrapper for styling
                Column({
                    .padding = std::pair{YGEdgeAll, 16.0f},
                    .backgroundColor = 0xFF1E293B, // Slate 800
                    .borderRadius = 12.0f,
                    .children = {
                        Breadcrumb(crumb_cfg)
                    }
                })
            }
        });
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
