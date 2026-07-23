// widgets_demo/tree_view/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/tree_view.hpp"
#include "vaura/widgets/icon.hpp"

using namespace vaura;

class TreeViewDemo : public StatelessWidget {
public:
    std::string_view typeName() const override { return "TreeViewDemo"; }

    WidgetPtr build(BuildContext& ctx) override {
        auto doc_icon = Icon(Icons::Doc, {.size = 18.0f, .color = 0xFFAAAAAA});
        auto folder_icon = Icon(Icons::Folder, {.size = 18.0f, .color = 0xFF3B82F6});

        TreeViewConfig tree_cfg;
        tree_cfg.nodes.push_back({"src", folder_icon, {
            {"main.cpp", doc_icon, {}, false, nullptr, nullptr},
            {"app", folder_icon, {
                {"app.cpp", doc_icon, {}, false, nullptr, nullptr},
                {"app.hpp", doc_icon, {}, false, nullptr, nullptr}
            }, false, nullptr, nullptr}
        }, true, nullptr, nullptr});

        tree_cfg.nodes.push_back({"include", folder_icon, {
            {"vaura", folder_icon, {
                {"core", folder_icon, {}, false, nullptr, nullptr},
                {"widgets", folder_icon, {}, false, nullptr, nullptr}
            }, false, nullptr, nullptr}
        }, false, nullptr, nullptr});

        tree_cfg.nodes.push_back({"README.md", doc_icon, {}, false, nullptr, nullptr});
        tree_cfg.nodes.push_back({"CMakeLists.txt", doc_icon, {}, false, nullptr, nullptr});

        return Column({
            .justify = Justify::Center,
            .align = Align::Center,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Text("Tree View Demo", {.font_size = 32.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold}),
                Column({.height = 40.0f}),
                Column({
                    .width = 400.0f,
                    .height = 400.0f,
                    .padding = std::pair{Edge::All, 20.0f},
                    .backgroundColor = 0xFF1E293B,
                    .borderRadius = 12.0f,
                    .children = {
                        TreeView(tree_cfg)
                    }
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Tree View Demo";
    config.width = 800;
    config.height = 700;
    return runApp(std::make_shared<TreeViewDemo>(), config);
}
