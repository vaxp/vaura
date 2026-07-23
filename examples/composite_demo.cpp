/// @file composite_demo.cpp
/// @brief Comprehensive demo for all Phase 1 composite widgets:
///        Dropdown, ContextMenu, PopupMenu, BottomSheet

#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/bottom_sheet.hpp"
#include "vaura/widgets/divider.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <string>

using namespace vaura;

// ─────────────────────────────────────────────
// Demo App
// ─────────────────────────────────────────────

class CompositeDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "CompositeDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class CompositeDemoState : public State {
public:
    std::string dropdown_value   = "react";
    std::string status_message   = "Right-click the card below or use the menus";
    bool        sheet_open       = false;
    bool        popup_open       = false;
    Point       popup_anchor     = {0, 0};

    WidgetPtr build(BuildContext& ctx) override {
        // ── Root ──────────────────────────────────────────────
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100)
             .backgroundColor(0xFF0F172A)
             .direction(FlexDirection::Column)
             .align(Align::Center)
             .padding(Edge::Top, 40);

        // ── Title ─────────────────────────────────────────────
        root->child(Text("Phase 1 — Overlay Widgets", {
            .font_size = 28,
            .color     = 0xFFF8FAFC,
            .weight    = TextStyle::Bold,
        }));

        auto sub = std::make_shared<FlexBox>();
        sub->margin(Edge::Top, 8).margin(Edge::Bottom, 32);
        sub->child(Text(status_message, {.font_size = 14, .color = 0xFF94A3B8}));
        root->child(sub);



        // ── Bottom Sheet ──────────────────────────────────────
        auto sheet_btn = GestureDetector({
            .child = Container({
                .color         = 0xFF0EA5E9,
                .padding       = EdgeInsets{14, 32, 14, 32},
                .border_radius = BorderRadius::circular(10),
                .child         = Text("Open Bottom Sheet", {.color = 0xFFFFFFFF, .weight = TextStyle::SemiBold}),
            }),
            .on_tap = [this]() { setState([this]{ sheet_open = true; }); },
        });
        root->child(sheet_btn);

        // ── Bottom Sheet overlay ──────────────────────────────
        if (sheet_open) {
            auto sheet_content = std::make_shared<FlexBox>();
            sheet_content->direction(FlexDirection::Column).padding(Edge::All, 20);
            sheet_content->child(Text("Share Options", {.font_size = 18, .color = 0xFFF8FAFC, .weight = TextStyle::Bold}));

            for (const char* opt : {"🔗 Copy Link", "🐦 Twitter / X", "📘 Facebook", "💬 WhatsApp"}) {
                auto row = std::make_shared<FlexBox>();
                row->height(52).align(Align::Center).padding(Edge::Horizontal, 4);
                row->child(Text(opt, {.font_size = 15, .color = 0xFFF8FAFC}));
                sheet_content->child(row);
                sheet_content->child(Divider({.thickness = 1, .color = 0xFF1E293B}));
            }

            root->child(BottomSheet({
                .child             = sheet_content,
                .is_open           = sheet_open,
                .on_close          = [this]() { setState([this]{ sheet_open = false; }); },
                .background_color  = 0xFF0F172A,
                .border_radius     = 20,
            }));
        }

        return root;
    }
};

std::unique_ptr<State> CompositeDemoApp::createState() {
    return std::make_unique<CompositeDemoState>();
}

int main(int argc, char** argv) {
    return vaura::runApp(std::make_shared<CompositeDemoApp>(), {
        .title  = "VAURA — Phase 1: Overlay Widgets",
        .width  = 800,
        .height = 700,
        .vsync  = true,
    });
}
