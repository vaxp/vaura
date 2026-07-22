#pragma once
/// @file card.hpp
/// @brief A luxurious Card widget with rounded corners and drop shadows.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <memory>

namespace vaura {

struct CardConfig {
    Color color = 0xFF1E293B;            ///< Background color of the card.
    float border_radius = 16.0f;         ///< Radius of the corners.
    float elevation = 8.0f;              ///< Size of the drop shadow.
    Color shadow_color = 0x80000000;     ///< Shadow color (default semi-transparent black).
    float shadow_dx = 0.0f;              ///< Shadow X offset.
    float shadow_dy = 4.0f;              ///< Shadow Y offset.
    EdgeInsets padding = {16, 16, 16, 16}; ///< Padding inside the card.
};

/// Create a luxurious Card widget with a drop shadow.
std::shared_ptr<Widget> card(CardConfig config, std::shared_ptr<Widget> child);

} // namespace vaura
