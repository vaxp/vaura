#include "vaura/app/app.hpp"
#include "vaura/state/state.hpp"
#include "vaura/widgets/bottom_sheet.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/rendering/color.hpp"

using namespace vaura;

class BottomSheetDemo : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "BottomSheetDemo"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class BottomSheetDemoState : public State {
    bool show_sheet = false;

public:
    WidgetPtr build(BuildContext& context) override {
        auto add_action = [&](const std::string& label, Color color) {
            return Button({
                .child = Text(label, {.font_size = 16.0f, .color = 0xFFFFFFFF}),
                .on_pressed = [this]() {
                    setState([this]() { show_sheet = false; });
                },
                .color = color,
                .border_radius = 8.0f,
                .padding = EdgeInsets(12, 24, 12, 24),
                .width = 300.0f // Fixed width for actions
            });
        };

        auto body = Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A, // Slate 900
            .children = {
                // Title in Body
                Text("Bottom Sheet Demo", {.font_size = 32.0f, .color = 0xFFF8FAFC}),

                // Spacer
                Column({.height = 40.0f}),

                // Open Button
                Button({
                    .child = Text("Open Action Menu", {.font_size = 18.0f, .color = 0xFFFFFFFF}),
                    .on_pressed = [this]() {
                        setState([this]() { show_sheet = true; });
                    },
                    .color = 0xFF8B5CF6, // Violet 500
                    .hover_color = 0xFF7C3AED,
                    .border_radius = 12.0f,
                    .padding = EdgeInsets(16, 32, 16, 32)
                })
            }
        });

        auto sheet_content = Column({
            .justifyContent = YGJustifyFlexStart,
            .alignItems = YGAlignCenter,
            .gap = 16.0f,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .padding = std::pair{YGEdgeAll, 24.0f},
            .children = {
                Text("Share this Document", {.font_size = 24.0f, .color = 0xFFFFFFFF}),
                add_action("Copy Link", 0xFF3B82F6), // Blue
                add_action("Share to Twitter", 0xFF1DA1F2), // Twitter Blue
                add_action("Email", 0xFF10B981) // Emerald
            }
        });

        // Bottom Sheet Setup
        return BottomSheet({
            .child = sheet_content,
            .body = body,
            .is_open = show_sheet,
            .on_close = [this]() {
                setState([this]() { show_sheet = false; });
            },
            .background_color = 0xFF1E293B, // Slate 800
            .border_radius = 24.0f,
            .max_height_percent = 50.0f
        });
    }
};

std::unique_ptr<State> BottomSheetDemo::createState() {
    return std::make_unique<BottomSheetDemoState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Bottom Sheet Showcase",
        .width = 800,
        .height = 600,
    };
    return vaura::runApp(std::make_shared<BottomSheetDemo>(), config);
}
