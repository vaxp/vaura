/// @file tree_view.cpp
/// @brief TreeView widget implementation.

#include "vaura/widgets/tree_view.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/transform.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

// ── Recursive node builder ─────────────────────────────────────

static WidgetPtr buildNode(const TreeNode& node, const TreeViewConfig& cfg,
                            int depth, int& hovered_key, int node_key,
                            std::function<void(int)> on_hover) {

    bool is_leaf   = node.children.empty();
    bool is_hov    = (hovered_key == node_key);

    Color row_bg = is_hov ? cfg.hover_color : 0x00000000;

    // Indent spacer
    float indent_px = depth * cfg.indent;

    auto row = std::make_shared<FlexBox>();
    row->flexDirection(YGFlexDirectionRow)
        .alignItems(YGAlignCenter)
        .height(cfg.item_height)
        .backgroundColor(row_bg)
        .padding(YGEdgeLeft, indent_px + 4.0f)
        .padding(YGEdgeRight, 8);

    // Expand/collapse chevron
    auto chevron_wrap = std::make_shared<FlexBox>();
    chevron_wrap->width(20).height(20)
                 .justifyContent(YGJustifyCenter)
                 .alignItems(YGAlignCenter)
                 .margin(YGEdgeRight, 4);

    if (!is_leaf) {
        float rotation = node.is_expanded ? 90.0f : 0.0f;
        auto chev = icon(Icons::ChevronRight, {.size = 16.0f, .color = cfg.expand_icon_color});
        if (rotation > 0.5f) {
            chevron_wrap->child(rotate(rotation, chev));
        } else {
            chevron_wrap->child(chev);
        }
    }
    row->child(chevron_wrap);

    // Icon
    if (node.icon) {
        auto iw = std::make_shared<FlexBox>();
        iw->width(18).height(18)
           .margin(YGEdgeRight, 6)
           .justifyContent(YGJustifyCenter)
           .alignItems(YGAlignCenter)
           .child(node.icon);
        row->child(iw);
    }

    // Label
    row->child(text(node.label, {
        .font_size = cfg.font_size,
        .color     = cfg.text_color,
    }));

    int nk = node_key;
    auto tap_fn   = node.on_tap;
    auto expand_fn = node.on_expand;
    bool was_expanded = node.is_expanded;

    auto row_gd = gesture_detector({
        .child          = row,
        .on_tap         = [tap_fn, expand_fn, was_expanded, is_leaf]() {
            if (!is_leaf && expand_fn) expand_fn(!was_expanded);
            if (is_leaf && tap_fn) tap_fn();
        },
        .on_hover_enter = [on_hover, nk]() { on_hover(nk); },
        .on_hover_exit  = [on_hover]()     { on_hover(-1); },
    });

    auto node_col = std::make_shared<FlexBox>();
    node_col->flexDirection(YGFlexDirectionColumn);
    node_col->child(row_gd);

    // Children
    if (!is_leaf && node.is_expanded) {
        for (int i = 0; i < (int)node.children.size(); ++i) {
            auto child_widget = buildNode(node.children[i], cfg, depth + 1,
                                          hovered_key, nk * 100 + i + 1, on_hover);
            node_col->child(child_widget);
        }
    }

    return node_col;
}

// ── TreeView widget ────────────────────────────────────────────

class TreeViewWidget : public StatefulWidget {
public:
    TreeViewConfig config;
    explicit TreeViewWidget(TreeViewConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "TreeView"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class TreeViewState : public State {
public:
    int hovered_key = -1;

    WidgetPtr build(BuildContext& ctx) override {
        auto* tw = static_cast<const TreeViewWidget*>(widget());
        const auto& cfg = tw->config;

        auto root_col = std::make_shared<FlexBox>();
        root_col->flexDirection(YGFlexDirectionColumn)
                 .backgroundColor(cfg.background_color);

        for (int i = 0; i < (int)cfg.nodes.size(); ++i) {
            auto node_widget = buildNode(cfg.nodes[i], cfg, 0, hovered_key, i + 1,
                [this](int k) { setState([this, k]{ hovered_key = k; }); });
            root_col->child(node_widget);
        }

        return root_col;
    }
};

std::unique_ptr<State> TreeViewWidget::createState() {
    return std::make_unique<TreeViewState>();
}

std::shared_ptr<Widget> tree_view(TreeViewConfig config) {
    return std::make_shared<TreeViewWidget>(std::move(config));
}

} // namespace vaura
