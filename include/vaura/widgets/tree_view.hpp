#pragma once
/// @file tree_view.hpp
/// @brief TreeView widget — collapsible/expandable hierarchical list.
///
/// Usage:
///   tree_view({
///       .nodes = {
///           {"src", folder_icon, {
///               {"main.cpp", file_icon, {}, true, [](){ open("main.cpp"); }},
///               {"utils.cpp", file_icon, {}, true, [](){ open("utils.cpp"); }},
///           }, false},
///           {"include", folder_icon, {
///               {"types.hpp", file_icon, {}, true, [](){ }},
///           }, false},
///       },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <vector>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// TreeNode
// ════════════════════════════════════════════════════════════════

struct TreeNode {
    std::string              label;
    WidgetPtr                icon        = nullptr;  ///< Leading icon.
    std::vector<TreeNode>    children;               ///< Empty = leaf node.
    bool                     is_expanded = false;    ///< Controlled expansion state.
    std::function<void()>    on_tap;                 ///< Fired when the node is tapped.
    std::function<void(bool)> on_expand;             ///< Fired when expanded/collapsed.
};

// ════════════════════════════════════════════════════════════════
// TreeViewConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the TreeView widget.
struct TreeViewConfig {
    std::vector<TreeNode> nodes;

    float  indent             = 20.0f;    ///< Horizontal indent per nesting level.
    float  item_height        = 36.0f;
    Color  background_color   = 0xFF1E293B;
    Color  hover_color        = 0xFF334155;
    Color  selected_color     = 0xFF1E3A5F;
    Color  text_color         = 0xFFF8FAFC;
    Color  expand_icon_color  = 0xFF64748B;
    Color  connector_color    = 0xFF334155;  ///< Vertical connector line color.
    float  font_size          = 13.5f;
    bool   show_connectors    = true;        ///< Vertical tree lines.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a TreeView widget.
std::shared_ptr<Widget> tree_view(TreeViewConfig config);

} // namespace vaura
