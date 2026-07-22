#pragma once
/// @file divider.hpp
/// @brief A thin line that groups content in lists and layouts.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <memory>

namespace vaura {

struct DividerConfig {
    float thickness = 1.0f;              ///< Thickness of the line.
    Color color = 0xFFCBD5E1;            ///< Color of the divider line.
    float indent = 0.0f;                 ///< Empty space to the leading edge.
    float end_indent = 0.0f;             ///< Empty space to the trailing edge.
    bool vertical = false;               ///< If true, draws a vertical divider.
};

/// Create a Divider widget.
std::shared_ptr<Widget> divider(DividerConfig config = {});

} // namespace vaura
