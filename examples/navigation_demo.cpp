/// @file navigation_demo.cpp
/// @brief Demo for Phase 2 navigation widgets:
///        Drawer, NavigationRail, Breadcrumb, FAB, Stepper

#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/drawer.hpp"
#include "vaura/widgets/navigation_rail.hpp"
#include "vaura/widgets/breadcrumb.hpp"
#include "vaura/widgets/fab.hpp"
#include "vaura/widgets/stepper.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"

using namespace vaura;

class NavigationDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "NavigationDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class NavigationDemoState : public State {
public:
    int  nav_index    = 0;
    bool drawer_open  = false;
    int  step_index   = 0;

    static constexpr const char* pages[] = { "Dashboard", "Analytics", "Projects", "Settings" };

    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100).heightPercent(100).backgroundColor(0xFF0F172A);

        // ── Navigation Rail (left sidebar) ────────────────────
        auto rail = NavigationRail({
            .items = {
                {.icon = Icon(Icons::House),      .label = "Dashboard"},
                {.icon = Icon(Icons::ChartBar),   .label = "Analytics"},
                {.icon = Icon(Icons::Folder),     .label = "Projects"},
                {.icon = Icon(Icons::GearAlt),    .label = "Settings"},
            },
            .selected_index          = nav_index,
            .on_changed              = [this](int i) { setState([this,i]{ nav_index=i; }); },
            .collapsed_width         = 72,
            .background_color        = 0xFF1E293B,
            .indicator_color         = 0x1A0EA5E9,
            .active_icon_color       = 0xFF0EA5E9,
            .inactive_icon_color     = 0xFF64748B,
            .active_label_color      = 0xFF0EA5E9,
            .inactive_label_color    = 0xFF64748B,
        });

        // ── Main content area ─────────────────────────────────
        auto main_col = std::make_shared<FlexBox>();
        main_col->direction(FlexDirection::Column).flexGrow(1.0f).padding(Edge::All, 24);

        // Breadcrumb
        std::vector<BreadcrumbItem> crumbs = {
            {.label = "Home",     .on_tap = [this]{ setState([this]{ nav_index=0; }); }},
            {.label = pages[nav_index]},
        };
        main_col->child(Breadcrumb({
            .items       = crumbs,
            .separator      = " / ",
            .active_color   = 0xFFF8FAFC,
            .inactive_color = 0xFF94A3B8,
        }));

        // Page title
        auto title_box = std::make_shared<FlexBox>();
        title_box->margin(Edge::Top, 20).margin(Edge::Bottom, 24);
        title_box->child(Text(pages[nav_index], {
            .font_size = 28, .color = 0xFFF8FAFC, .weight = TextStyle::Bold
        }));
        main_col->child(title_box);

        // Stepper (shown on Projects tab)
        if (nav_index == 2) {
            main_col->child(Text("Project Setup Wizard", {.font_size = 16, .color = 0xFF94A3B8}));
            auto sw = std::make_shared<FlexBox>();
            sw->margin(Edge::Top, 16).width(680);

            auto step_content = [](const char* msg) {
                auto f = std::make_shared<FlexBox>();
                f->padding(Edge::All, 12);
                f->child(Text(msg, {.font_size=14, .color=0xFFCBD5E1}));
                return std::static_pointer_cast<Widget>(f);
            };

            sw->child(Stepper({
                .steps = {
                    {.title="Project Type",   .subtitle="Choose template", .content=step_content("Choose between Web App, API, or Mobile.")},
                    {.title="Configuration",  .subtitle="Set parameters",  .content=step_content("Set project name, language, and dependencies.")},
                    {.title="Team",           .subtitle="Add members",     .content=step_content("Invite collaborators to your project.")},
                    {.title="Deploy",         .subtitle="Launch",          .content=step_content("Select target environment and deploy.")},
                },
                .current_step    = step_index,
                .on_step_changed = [this](int s){ setState([this,s]{ step_index=s; }); },
                .on_complete     = [this](){ setState([this]{ step_index=0; }); },
                .active_color    = 0xFF0EA5E9,
                .completed_color = 0xFF22C55E,
                .inactive_color  = 0xFF334155,
                .background_color= 0xFF1E293B,
                .text_color      = 0xFFF8FAFC,
                .subtitle_color  = 0xFF94A3B8,
                .connector_color = 0xFF475569,
            }));
            main_col->child(sw);
        } else {
            // Placeholder content
            auto card = Container({
                .color         = 0xFF1E293B,
                .width_percent = 100.0f,
                .padding       = EdgeInsets::all(24),
                .border_radius = BorderRadius::circular(12),
                .border        = Border{0xFF334155, 1},
                .child         = Text("Content for \"" + std::string(pages[nav_index]) + "\" page.",
                                      {.font_size=15, .color=0xFF94A3B8}),
            });
            main_col->child(card);
        }

        // ── FAB ───────────────────────────────────────────────
        auto fab_wrap = std::make_shared<FlexBox>();
        fab_wrap->positionType(PositionType::Absolute)
                 .position(Edge::Bottom, 24)
                 .position(Edge::Right, 24);
        fab_wrap->child(FAB({
            .icon      = Icon(Icons::Plus, {.size=22,.color=0xFFFFFFFF}),
            .label     = (nav_index == 0) ? "New" : "",
            .on_pressed= [this](){ setState([this]{ nav_index=2; step_index=0; }); },
            .background_color = 0xFF0EA5E9,
        }));

        // Force a RowReverse direction to see if this flips the layout on the user's system
        root->direction(FlexDirection::RowReverse);
        root->child(rail).child(main_col).child(fab_wrap);

        // ── Drawer overlay ────────────────────────────────────
        if (drawer_open) {
            auto drawer_col = std::make_shared<FlexBox>();
            drawer_col->direction(FlexDirection::Column).padding(Edge::All, 20);
            drawer_col->child(Text("Menu", {.font_size=22,.color=0xFFF8FAFC,.weight=TextStyle::Bold}));
            for (int i = 0; i < 4; ++i) {
                auto row = std::make_shared<FlexBox>();
                row->height(48).align(Align::Center);
                row->child(Text(pages[i], {.font_size=15,.color=0xFFCBD5E1}));
                int ci=i;
                drawer_col->child(GestureDetector({
                    .child  = row,
                    .on_tap = [this,ci](){ setState([this,ci]{ nav_index=ci; drawer_open=false; }); },
                }));
            }
            root->child(Drawer({
                .child     = drawer_col,
                .is_open   = drawer_open,
                .on_close  = [this](){ setState([this]{ drawer_open=false; }); },
                .width     = 260,
                .background_color = 0xFF1E293B,
            }));
        }

        return root;
    }
};

std::unique_ptr<State> NavigationDemoApp::createState() {
    return std::make_unique<NavigationDemoState>();
}

int main(int argc, char** argv) {
    return vaura::runApp(std::make_shared<NavigationDemoApp>(), {
        .title  = "VAURA — Phase 2: Navigation Widgets",
        .width  = 1024,
        .height = 700,
        .vsync  = true,
    });
}
