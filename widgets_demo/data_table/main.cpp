#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/data_table.hpp"
#include "vaura/state/state.hpp"
#include <algorithm>

using namespace vaura;

class DataTableDemoState : public State {
    int sort_col = 0;
    bool sort_asc = true;
    
    struct User {
        std::string name;
        std::string email;
        std::string role;
        bool active;
    };
    
    std::vector<User> data = {
        {"Alice Smith", "alice@example.com", "Admin", true},
        {"Bob Johnson", "bob@example.com", "Editor", false},
        {"Charlie Brown", "charlie@example.com", "Viewer", true},
        {"Diana Prince", "diana@example.com", "Admin", true},
        {"Eve Adams", "eve@example.com", "Editor", false}
    };
    
public:
    WidgetPtr build(BuildContext& ctx) override {
        auto root = std::make_shared<FlexBox>();
        root->flexDirection(YGFlexDirectionColumn)
            .justifyContent(YGJustifyCenter)
            .alignItems(YGAlignCenter)
            .padding(YGEdgeAll, 40.0f)
            .backgroundColor(0xFF0F172A)
            .widthPercent(100).heightPercent(100);

        auto title = std::make_shared<FlexBox>();
        title->margin(YGEdgeBottom, 20)
             .child(text("Users Table", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}));
        root->child(title);

        DataTableConfig cfg;
        cfg.columns = {
            {"Name", true, 180.0f},
            {"Email", true, 220.0f},
            {"Role", false, 120.0f},
            {"Status", false, 100.0f}
        };
        cfg.sort_column_index = sort_col;
        cfg.sort_ascending = sort_asc;
        
        cfg.on_sort = [this](int col, bool asc) {
            setState([this, col, asc]() {
                sort_col = col;
                sort_asc = asc;
                
                std::sort(data.begin(), data.end(), [this](const User& a, const User& b) {
                    bool res = false;
                    if (sort_col == 0) res = a.name < b.name;
                    else if (sort_col == 1) res = a.email < b.email;
                    
                    return sort_asc ? res : !res;
                });
            });
        };

        for (const auto& user : data) {
            DataRow row;
            row.cells.push_back(text(user.name, {.font_size = 14.0f, .color = 0xFFF8FAFC}));
            row.cells.push_back(text(user.email, {.font_size = 14.0f, .color = 0xFF94A3B8}));
            row.cells.push_back(text(user.role, {.font_size = 14.0f, .color = 0xFFCBD5E1}));
            
            auto status_box = std::make_shared<FlexBox>();
            status_box->width(80).height(24)
                      .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter)
                      .backgroundColor(user.active ? 0x3010B981 : 0x30EF4444)
                      .borderRadius(12.0f);
            status_box->child(text(user.active ? "Active" : "Inactive", {
                .font_size = 12.0f,
                .color = user.active ? 0xFF10B981 : 0xFFEF4444,
                .weight = TextStyle::Bold
            }));
            
            row.cells.push_back(status_box);
            row.on_tap = [](){}; // Just make it clickable for hover effects
            
            cfg.rows.push_back(row);
        }

        auto table_wrapper = std::make_shared<FlexBox>();
        table_wrapper->width(660).height(400);
        table_wrapper->child(data_table(cfg));
        
        root->child(table_wrapper);
        return root;
    }
};

class DataTableDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "DataTableDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<DataTableDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA DataTable Demo";
    config.width = 900;
    config.height = 600;

    return runApp(std::make_shared<DataTableDemo>(), config);
}
