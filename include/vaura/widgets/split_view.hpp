#pragma once
/// @file split_view.hpp
/// @brief SplitView widget — two panes separated by a draggable divider.
///
/// Usage:
///   split_view({
///       .first  = file_browser_panel,
///       .second = editor_panel,
///       .axis   = SplitViewConfig::Axis::Horizontal,
///       .initial_ratio = 0.3f,
///       .on_resize = [this](float r){ setState([this,r]{ ratio_ = r; }); },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// SplitViewConfig
// ════════════════════════════════════════════════════════════════

struct SplitViewConfig {
    WidgetPtr   first;                  ///< Left or Top pane.
    WidgetPtr   second;                 ///< Right or Bottom pane.

    enum class Axis { Horizontal, Vertical } axis = Axis::Horizontal;

    float  initial_ratio     = 0.5f;   ///< 0–1: proportion given to `first`.
    float  min_first_size    = 80.0f;  ///< Minimum px for the first pane.
    float  min_second_size   = 80.0f;  ///< Minimum px for the second pane.

    std::function<void(float)> on_resize; ///< Fired continuously with the new ratio.

    Color  divider_color     = 0xFF334155;
    Color  divider_hover     = 0xFF0EA5E9;  ///< Highlight when cursor is over divider.
    float  divider_thickness = 4.0f;
    float  total_size        = 800.0f;     ///< Total width or height (passed from parent).
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a SplitView widget.
std::shared_ptr<Widget> split_view(SplitViewConfig config);

} // namespace vaura
