/// @file feedback_layouts_demo.cpp
/// @brief Demo for Phase 5 (Snackbar, Toast) and Phase 6 (SplitView, ResizablePanels)

#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/snackbar.hpp"
#include "vaura/widgets/toast.hpp"
#include "vaura/widgets/split_view.hpp"
#include "vaura/widgets/resizable_panels.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"

using namespace vaura;

enum class DemoSnackbarType { Success, Info, Warning, Error };

// ─── Helper: placeholder panel ────────────────────────────────
static WidgetPtr makePanel(const char* title, Color bg, Color fg) {
    auto f = std::make_shared<FlexBox>();
    f->widthPercent(100).heightPercent(100)
     .backgroundColor(bg)
     .justifyContent(YGJustifyCenter)
     .alignItems(YGAlignCenter);
    f->child(text(title, {.font_size=16,.color=fg,.weight=TextStyle::SemiBold}));
    return f;
}

class FeedbackLayoutsDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "FeedbackLayoutsDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class FeedbackLayoutsDemoState : public State {
public:
    int  active_tab = 0;

    // Snackbar state
    bool snackbar_visible = false;
    std::string snackbar_msg;
    DemoSnackbarType snackbar_type = DemoSnackbarType::Info;

    // Toast state
    bool toast_visible = false;
    std::string toast_msg;
    ToastConfig::Position toast_pos = ToastConfig::Position::Bottom;

    void showSnackbar(const std::string& msg, DemoSnackbarType t) {
        setState([this, msg, t]{
            snackbar_msg     = msg;
            snackbar_type    = t;
            snackbar_visible = true;
        });
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100)
             .backgroundColor(0xFF0F172A)
             .flexDirection(YGFlexDirectionColumn);

        // ── Header ────────────────────────────────────────────
        auto hdr = std::make_shared<FlexBox>();
        hdr->flexDirection(YGFlexDirectionRow)
            .alignItems(YGAlignCenter)
            .padding(YGEdgeAll, 20)
            .backgroundColor(0xFF1E293B);

        hdr->child(text("Phase 5 & 6  —  Feedback + Layouts", {
            .font_size=20,.color=0xFFF8FAFC,.weight=TextStyle::Bold,
        }));
        root->child(hdr);

        // ── Tab bar ───────────────────────────────────────────
        auto tabs_row = std::make_shared<FlexBox>();
        tabs_row->flexDirection(YGFlexDirectionRow)
                 .padding(YGEdgeHorizontal, 20)
                 .padding(YGEdgeVertical, 12)
                 .backgroundColor(0xFF0F172A);

        for (auto [lbl, idx] : std::vector<std::pair<const char*, int>>{
            {"Snackbar / Toast", 0},
            {"SplitView",        1},
            {"Resizable Panels", 2},
        }) {
            bool active = (active_tab == idx);
            int ci = idx;
            auto btn = gesture_detector({
                .child = container({
                    .color         = active ? 0xFF0EA5E9 : 0xFF1E293B,
                    .padding       = EdgeInsets{10, 18, 10, 18},
                    .border_radius = BorderRadius::circular(8),
                    .child         = text(lbl, {
                        .font_size = 13,
                        .color     = active ? 0xFFFFFFFF : 0xFF94A3B8,
                        .weight    = active ? TextStyle::SemiBold : TextStyle::Regular,
                    }),
                }),
                .on_tap = [this,ci](){ setState([this,ci]{ active_tab=ci; }); },
            });
            auto bw = std::make_shared<FlexBox>(); bw->margin(YGEdgeRight,8).child(btn);
            tabs_row->child(bw);
        }
        root->child(tabs_row);

        // ── Content area ──────────────────────────────────────
        auto content = std::make_shared<FlexBox>();
        content->flexGrow(1).widthPercent(100);

        if (active_tab == 0) {
            // Snackbar + Toast trigger buttons
            auto center = std::make_shared<FlexBox>();
            center->widthPercent(100).heightPercent(100)
                   .justifyContent(YGJustifyCenter)
                   .alignItems(YGAlignCenter)
                   .flexDirection(YGFlexDirectionColumn);

            center->child(text("Notification Widgets", {
                .font_size=22,.color=0xFFF8FAFC,.weight=TextStyle::Bold
            }));
            auto sp = std::make_shared<FlexBox>(); sp->height(20); center->child(sp);

            // Snackbar buttons
            center->child(text("Snackbars:", {.font_size=13,.color=0xFF64748B}));
            auto snack_row = std::make_shared<FlexBox>();
            snack_row->flexDirection(YGFlexDirectionRow).margin(YGEdgeTop,8).margin(YGEdgeBottom,20);

            struct SBtn { const char* lbl; Color col; DemoSnackbarType type; };
            for (auto [lbl, col, type] : std::vector<SBtn>{
                {"✅ Success", 0xFF22C55E, DemoSnackbarType::Success},
                {"ℹ️  Info",   0xFF0EA5E9, DemoSnackbarType::Info},
                {"⚠️  Warning",0xFFF59E0B, DemoSnackbarType::Warning},
                {"❌ Error",   0xFFEF4444, DemoSnackbarType::Error},
            }) {
                std::string msg = std::string(lbl) + " notification shown!";
                DemoSnackbarType t  = type;
                auto btn = gesture_detector({
                    .child = container({
                        .color         = col,
                        .padding       = EdgeInsets{10, 16, 10, 16},
                        .border_radius = BorderRadius::circular(8),
                        .child         = text(lbl, {.font_size=13,.color=0xFFFFFFFF,.weight=TextStyle::SemiBold}),
                    }),
                    .on_tap = [this,msg,t](){ showSnackbar(msg, t); },
                });
                auto bw = std::make_shared<FlexBox>(); bw->margin(YGEdgeRight,8).child(btn);
                snack_row->child(bw);
            }
            center->child(snack_row);

            // Toast button
            center->child(text("Toasts:", {.font_size=13,.color=0xFF64748B}));
            auto toast_row = std::make_shared<FlexBox>();
            toast_row->flexDirection(YGFlexDirectionRow).margin(YGEdgeTop,8);

            for (auto [lbl, pos] : std::vector<std::pair<const char*, ToastConfig::Position>>{
                {"Top",    ToastConfig::Position::Top},
                {"Center", ToastConfig::Position::Center},
                {"Bottom", ToastConfig::Position::Bottom},
            }) {
                std::string msg = std::string("Toast at ") + lbl;
                ToastConfig::Position p = pos;
                auto btn = gesture_detector({
                    .child = container({
                        .color         = 0xFF334155,
                        .padding       = EdgeInsets{10, 16, 10, 16},
                        .border_radius = BorderRadius::circular(8),
                        .child         = text(std::string("Toast ") + lbl,
                                             {.font_size=13,.color=0xFFFFFFFF}),
                    }),
                    .on_tap = [this, msg, p](){
                        setState([this, msg, p]{ toast_msg = msg; toast_pos = p; toast_visible = true; });
                    },
                });
                auto bw = std::make_shared<FlexBox>(); bw->margin(YGEdgeRight,8).child(btn);
                toast_row->child(bw);
            }
            center->child(toast_row);

            content->child(center);

            // Snackbar overlay
            if (snackbar_visible) {
                Color bg_col = 0xFF1E293B;
                if (snackbar_type == DemoSnackbarType::Success) bg_col = 0xFF22C55E;
                if (snackbar_type == DemoSnackbarType::Warning) bg_col = 0xFFF59E0B;
                if (snackbar_type == DemoSnackbarType::Error)   bg_col = 0xFFEF4444;

                content->child(snackbar({
                    .message       = snackbar_msg,
                    .action_label  = "Dismiss",
                    .on_action     = [this](){ setState([this]{ snackbar_visible=false; }); },
                    .on_dismissed  = [this](){ setState([this]{ snackbar_visible=false; }); },
                    .duration_ms   = 3000,
                    .background_color = bg_col,
                }));
            }

            // Toast overlay
            if (toast_visible) {
                content->child(toast({
                    .message      = toast_msg,
                    .on_dismissed = [this](){ setState([this]{ toast_visible=false; }); },
                    .duration_ms  = 2000,
                    .position     = toast_pos,
                }));
            }

        } else if (active_tab == 1) {
            // SplitView
            content->child(split_view({
                .first  = makePanel("📁 File Browser", 0xFF1E293B, 0xFFCBD5E1),
                .second = makePanel("📝 Editor",       0xFF0F172A, 0xFFF8FAFC),
                .axis   = SplitViewConfig::Axis::Horizontal,
                .initial_ratio   = 0.28f,
                .min_first_size  = 120,
                .min_second_size = 300,
                .divider_color   = 0xFF334155,
                .divider_hover   = 0xFF0EA5E9,
                .divider_thickness = 4,
                .total_size      = 980,
            }));

        } else {
            // Resizable Panels (3 panes: sidebar | main | inspector)
            content->child(resizable_panels({
                .panels = {
                    {.child=makePanel("📂 Files",     0xFF1E293B, 0xFFCBD5E1), .initial_size=180, .min_size=100},
                    {.child=makePanel("📝 Editor",    0xFF0F172A, 0xFFF8FAFC), .initial_size=0,   .min_size=300},
                    {.child=makePanel("🔍 Inspector", 0xFF1E293B, 0xFFCBD5E1), .initial_size=220, .min_size=120},
                },
                .axis            = ResizablePanelsConfig::Axis::Horizontal,
                .divider_color   = 0xFF334155,
                .divider_hover   = 0xFF0EA5E9,
                .divider_thickness = 4,
                .total_size      = 980,
            }));
        }

        root->child(content);
        return root;
    }
};

std::unique_ptr<State> FeedbackLayoutsDemoApp::createState() {
    return std::make_unique<FeedbackLayoutsDemoState>();
}

int main(int argc, char** argv) {
    return vaura::runApp(std::make_shared<FeedbackLayoutsDemoApp>(), {
        .title  = "VAURA — Phase 5 & 6: Feedback + Layout Widgets",
        .width  = 1040,
        .height = 680,
        .vsync  = true,
    });
}
