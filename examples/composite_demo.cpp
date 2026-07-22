/// @file composite_demo.cpp
/// @brief Comprehensive demo for all Phase 1 composite widgets:
///        Dropdown, ContextMenu, PopupMenu, BottomSheet

#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/dropdown.hpp"
#include "vaura/widgets/context_menu.hpp"
#include "vaura/widgets/popup_menu.hpp"
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
             .flexDirection(YGFlexDirectionColumn)
             .alignItems(YGAlignCenter)
             .padding(YGEdgeTop, 40);

        // ── Title ─────────────────────────────────────────────
        root->child(Text("Phase 1 — Overlay Widgets", {
            .font_size = 28,
            .color     = 0xFFF8FAFC,
            .weight    = TextStyle::Bold,
        }));

        auto sub = std::make_shared<FlexBox>();
        sub->margin(YGEdgeTop, 8).margin(YGEdgeBottom, 32);
        sub->child(Text(status_message, {.font_size = 14, .color = 0xFF94A3B8}));
        root->child(sub);

        // ── Dropdown ──────────────────────────────────────────
        auto dw = std::make_shared<FlexBox>();
        dw->flexDirection(YGFlexDirectionColumn).alignItems(YGAlignCenter).margin(YGEdgeBottom, 24);
        dw->child(Text("Framework Selector", {.font_size = 13, .color = 0xFF64748B}));

        auto drop_wrap = std::make_shared<FlexBox>();
        drop_wrap->margin(YGEdgeTop, 8).child(Dropdown({
            .items = {
                {.label = "⚛️  React",     .value = "react"},
                {.label = "🅥  Vue.js",    .value = "vue"},
                {.label = "🅰️  Angular",   .value = "angular"},
                {.label = "🔥  Svelte",    .value = "svelte"},
                {.label = "🦕  SolidJS",   .value = "solid"},
            },
            .selected_value    = dropdown_value,
            .on_changed        = [this](const std::string& v) {
                setState([this, v]{ dropdown_value = v; status_message = "Selected: " + v; });
            },
            .placeholder       = "Select Framework",
            .background_color  = 0xFF1E293B,
            .text_color        = 0xFFF8FAFC,
            .placeholder_color = 0xFF64748B,
            .border_color      = 0xFF334155,
            .focus_color       = 0xFF0EA5E9,
            .selected_color    = 0x330EA5E9,
            .hover_color       = 0xFF1E293B,
            .border_radius     = 10,
            .width             = 280,
            .height            = 48,
            .font_size         = 15,
        }));
        dw->child(drop_wrap);
        root->child(dw);

        // ── Context Menu card ─────────────────────────────────
        root->child(Text("Right-click the card:", {.font_size = 13, .color = 0xFF64748B}));
        auto ctx_card = Container({
            .color         = 0xFF1E293B,
            .width         = 320,
            .height        = 90,
            .padding       = EdgeInsets::all(16),
            .border_radius = BorderRadius::circular(12),
            .border        = Border{0xFF334155, 1},
            .child         = Text("Right-click me!", {
                                .font_size = 16, .color = 0xFFCBD5E1,
                                .align = TextStyle::Center }),
        });

        auto ctx_gd = ContextMenu({
            .child = ctx_card,
            .items = {
                {.label = "✂️  Cut",   .on_tap = [this]{ setState([this]{ status_message = "Cut"; }); }},
                {.label = "📋 Copy",  .on_tap = [this]{ setState([this]{ status_message = "Copy"; }); }},
                {.label = "📌 Paste", .on_tap = [this]{ setState([this]{ status_message = "Paste"; }); }},
                {.is_separator = true},
                {.label = "🗑️  Delete", .on_tap = [this]{ setState([this]{ status_message = "Delete"; }); }, .is_danger = true},
            },
            .background_color = 0xFF1E293B,
            .border_color     = 0xFF334155,
        });

        auto ctx_wrap = std::make_shared<FlexBox>();
        ctx_wrap->margin(YGEdgeTop, 8).margin(YGEdgeBottom, 24).child(ctx_gd);
        root->child(ctx_wrap);

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
            sheet_content->flexDirection(YGFlexDirectionColumn).padding(YGEdgeAll, 20);
            sheet_content->child(Text("Share Options", {.font_size = 18, .color = 0xFFF8FAFC, .weight = TextStyle::Bold}));

            for (const char* opt : {"🔗 Copy Link", "🐦 Twitter / X", "📘 Facebook", "💬 WhatsApp"}) {
                auto row = std::make_shared<FlexBox>();
                row->height(52).alignItems(YGAlignCenter).padding(YGEdgeHorizontal, 4);
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
