/// @file data_display_demo.cpp
/// @brief Demo for Phase 4: Complex Data Display
///        DataTable, TreeView, ExpansionPanel

#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/data_table.hpp"
#include "vaura/widgets/tree_view.hpp"
#include "vaura/widgets/expansion_panel.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <set>

using namespace vaura;

class DataDisplayDemoApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "DataDisplayDemoApp"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class DataDisplayDemoState : public State {
public:
    int active_tab = 0;

    // DataTable state
    int sort_col_idx = -1;
    bool sort_asc = true;
    std::set<int> selected_rows;

    // TreeView state
    std::vector<TreeNode> tree_nodes;

    void initState() override {
        State::initState();
        // Initialize TreeView nodes
        tree_nodes = {
            {"📁 src", nullptr, {
                {"main.cpp", nullptr, {}, false, [](){}, [](bool){}},
                {"app.cpp",  nullptr, {}, false, [](){}, [](bool){}},
                {"📁 widgets", nullptr, {
                    {"button.cpp", nullptr, {}, false, [](){}, [](bool){}},
                    {"text.cpp",   nullptr, {}, false, [](){}, [](bool){}},
                }, false, [](){}, [](bool){}},
            }, true, [](){}, [](bool){}},
            {"📁 include", nullptr, {
                {"vaura.hpp", nullptr, {}, false, [](){}, [](bool){}},
            }, false, [](){}, [](bool){}},
            {"CMakeLists.txt", nullptr, {}, false, [](){}, [](bool){}},
            {"README.md",      nullptr, {}, false, [](){}, [](bool){}},
        };
        
        // Recursive hook to update state when nodes are expanded
        std::function<void(std::vector<TreeNode>&)> bind_tree = [&](std::vector<TreeNode>& nodes) {
            for (auto& node : nodes) {
                TreeNode* ptr = &node;
                ptr->on_expand = [this, ptr](bool ex) {
                    setState([ptr, ex]{ ptr->is_expanded = ex; });
                };
                if (!node.children.empty()) {
                    bind_tree(node.children);
                }
            }
        };
        bind_tree(tree_nodes);
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

        hdr->child(text("Phase 4  —  Complex Data Display", {
            .font_size=20,.color=0xFFF8FAFC,.weight=TextStyle::Bold,
        }));
        root->child(hdr);

        // ── Tab bar ───────────────────────────────────────────
        auto tabs_row = std::make_shared<FlexBox>();
        tabs_row->flexDirection(YGFlexDirectionRow)
                 .padding(YGEdgeHorizontal, 20)
                 .padding(YGEdgeVertical, 12)
                 .backgroundColor(0xFF0F172A);

        auto add_tab = [&](int idx, const char* label) {
            bool active = (active_tab == idx);
            auto btn = gesture_detector({
                .child = container({
                    .color         = active ? 0xFF0EA5E9 : 0xFF1E293B,
                    .padding       = EdgeInsets{10, 18, 10, 18},
                    .border_radius = BorderRadius::circular(8),
                    .child         = text(label, {
                        .font_size = 13,
                        .color     = active ? 0xFFFFFFFF : 0xFF94A3B8,
                        .weight    = active ? TextStyle::SemiBold : TextStyle::Regular,
                    }),
                }),
                .on_tap = [this,idx](){ setState([this,idx]{ active_tab = idx; }); },
            });
            auto bw = std::make_shared<FlexBox>(); bw->margin(YGEdgeRight,8).child(btn);
            tabs_row->child(bw);
        };
        add_tab(0, "DataTable");
        add_tab(1, "TreeView");
        add_tab(2, "ExpansionPanel");
        root->child(tabs_row);

        // ── Content Area ──────────────────────────────────────
        auto content = std::make_shared<FlexBox>();
        content->flexGrow(1).padding(YGEdgeAll, 24);

        if (active_tab == 0) {
            // DataTable
            std::vector<DataRow> dr_rows = {
                {{text("Alice", {.color=0xFFF8FAFC}), text("alice@example.com", {.color=0xFF94A3B8}), text("Admin", {.color=0xFF38BDF8})}, selected_rows.count(0) > 0, [](){}},
                {{text("Bob", {.color=0xFFF8FAFC}),   text("bob@example.com", {.color=0xFF94A3B8}),   text("User", {.color=0xFF64748B})},  selected_rows.count(1) > 0, [](){}},
                {{text("Charlie", {.color=0xFFF8FAFC}), text("charlie@example.com", {.color=0xFF94A3B8}), text("User", {.color=0xFF64748B})}, selected_rows.count(2) > 0, [](){}},
            };

            root->child(data_table({
                .columns = {
                    {.label="Name",       .sortable=true,  .width=160, .align=DataColumn::Align::Left},
                    {.label="Email",      .sortable=true,  .width=240, .align=DataColumn::Align::Left},
                    {.label="Role",       .sortable=true,  .width=120, .align=DataColumn::Align::Left},
                },
                .rows = dr_rows,
                .on_sort        = [this](int k, bool asc){
                    setState([this,k,asc]{ sort_col_idx=k; sort_asc=asc; });
                },
                .on_row_select = [this](int r, bool sel){
                    setState([this,r,sel]{
                        if(sel) selected_rows.insert(r); else selected_rows.erase(r);
                    });
                },
                .sort_column_index  = sort_col_idx,
                .sort_ascending     = sort_asc,
                .show_checkbox      = true,
                .show_row_numbers   = false,
                .header_bg          = 0xFF0F172A,
                .header_text_color  = 0xFF94A3B8,
                .row_color          = 0xFF1E293B,
                .alt_row_color      = 0xFF172030,
                .selected_color     = 0xFF0D2137,
                .hover_color        = 0xFF253347,
                .border_color       = 0xFF334155,
                .sort_icon_color    = 0xFF38BDF8,
                .text_color         = 0xFFF8FAFC,
                .row_height         = 52,
                .header_height      = 56,
                .font_size          = 13.5,
                .border_radius      = 12,
            }));
        } else if (active_tab == 1) {
            // TreeView
            auto tv_wrap = container({
                .color         = 0xFF1E293B,
                .width_percent = 100.0f,
                .padding       = EdgeInsets::all(16),
                .border_radius = BorderRadius::circular(12),
                .border        = Border{0xFF334155, 1},
                .child         = tree_view({
                    .nodes = tree_nodes,
                    .indent = 20.0f,
                    .item_height = 36.0f,
                    .background_color = 0xFF1E293B,
                    .hover_color = 0xFF334155,
                    .selected_color = 0xFF1E3A5F,
                    .text_color = 0xFFF8FAFC,
                    .expand_icon_color = 0xFF64748B,
                    .connector_color = 0xFF334155,
                    .font_size = 13.5f,
                    .show_connectors = true,
                }),
            });
            content->child(tv_wrap);
        } else if (active_tab == 2) {
            // ExpansionPanel
            auto ep_wrap = std::make_shared<FlexBox>();
            ep_wrap->width(600);
            
            auto make_content = [](const char* t) {
                auto w = std::make_shared<FlexBox>();
                w->padding(YGEdgeAll, 16);
                w->child(text(t, {.font_size=14, .color=0xFFCBD5E1}));
                return std::static_pointer_cast<Widget>(w);
            };

            static bool ep1_open = false;
            static bool ep2_open = false;

            ep_wrap->child(expansion_panel({
                .items = {
                    {"Account Settings", "Profile & Security", make_content("Update your email, password, and 2FA settings."), ep1_open},
                    {"Notifications",    "Email & Push",       make_content("Configure which alerts you receive."), ep2_open},
                },
                .on_expansion_changed = [this](int idx, bool open) {
                    setState([idx, open]{
                        if(idx == 0) ep1_open = open;
                        if(idx == 1) ep2_open = open;
                    });
                },
                .allow_multiple_expanded = true,
                .background_color        = 0xFF1E293B,
                .header_color            = 0xFF0F172A,
                .expanded_bg             = 0xFF162032,
                .border_color            = 0xFF334155,
                .title_color             = 0xFFF8FAFC,
                .subtitle_color          = 0xFF94A3B8,
                .icon_color              = 0xFF64748B,
                .active_icon_color       = 0xFF0EA5E9,
                .border_radius           = 12,
                .header_height           = 56,
                .title_font_size         = 15.0f,
                .subtitle_font_size      = 12.5f,
            }));
            content->child(ep_wrap);
        }

        root->child(content);
        return root;
    }
};

std::unique_ptr<State> DataDisplayDemoApp::createState() {
    return std::make_unique<DataDisplayDemoState>();
}

int main(int argc, char** argv) {
    return vaura::runApp(std::make_shared<DataDisplayDemoApp>(), {
        .title  = "VAURA — Phase 4: Data Display Demo",
        .width  = 1000,
        .height = 700,
        .vsync  = true,
    });
}
