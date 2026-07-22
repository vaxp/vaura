#pragma once
/// @file resizable_panels.hpp
/// @brief ResizablePanels widget — N panels separated by draggable dividers.
///
/// Generalization of SplitView that supports 2+ panels.
///
/// Usage:
///   resizable_panels({
///       .panels = {
///           {sidebar,       120.0f, 80.0f},   // initial=120, min=80
///           {editor,        0.0f,   200.0f},  // initial=auto, min=200
///           {inspector,     200.0f, 100.0f},
///       },
///       .axis = ResizablePanelsConfig::Axis::Horizontal,
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <vector>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// PanelConfig
// ════════════════════════════════════════════════════════════════

struct PanelConfig {
    WidgetPtr  child;
    float      initial_size = 0.0f;  ///< 0 = auto (equal share of remaining space).
    float      min_size     = 60.0f;
    float      max_size     = 0.0f;  ///< 0 = unlimited.
};

// ════════════════════════════════════════════════════════════════
// ResizablePanelsConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the ResizablePanels widget.
struct ResizablePanelsConfig {
    std::vector<PanelConfig> panels;

    enum class Axis { Horizontal, Vertical } axis = Axis::Horizontal;

    std::function<void(const std::vector<float>&)> on_resize; ///< Absolute sizes of all panels.

    Color  divider_color     = 0xFF334155;
    Color  divider_hover     = 0xFF0EA5E9;
    float  divider_thickness = 4.0f;
    float  total_size        = 1200.0f; ///< Total container size in the axis direction.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a ResizablePanels widget.
std::shared_ptr<Widget> resizable_panels(ResizablePanelsConfig config);

} // namespace vaura
