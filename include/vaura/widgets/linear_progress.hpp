#pragma once
/// @file linear_progress.hpp
/// @brief A linear progress indicator widget.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <memory>
#include <optional>

namespace vaura {

struct LinearProgressConfig {
    /// Progress value from 0.0 to 1.0. If nullopt, acts as an indeterminate progress bar.
    std::optional<float> value = std::nullopt;
    
    float thickness = 4.0f;               ///< Height of the progress bar
    float border_radius = 2.0f;           ///< Corner radius of the bar
    
    Color active_color = 0xFF3B82F6;      ///< Foreground (active) color (Blue)
    Color inactive_color = 0xFFE2E8F0;    ///< Background (track) color (Slate 200)
};

/// Create a linear progress indicator widget.
std::shared_ptr<Widget> linear_progress(LinearProgressConfig config = {});

/// Declarative syntax alias for LinearProgress
inline std::shared_ptr<Widget> LinearProgress(LinearProgressConfig config = {}) {
    return linear_progress(std::move(config));
}

} // namespace vaura
