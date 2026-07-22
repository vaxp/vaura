#pragma once
/// @file snackbar.hpp
/// @brief Snackbar widget — brief actionable message shown at the bottom of the screen.
///
/// The Snackbar auto-dismisses after `duration_ms`. It can have an optional action button.
///
/// Usage:
///   snackbar({
///       .message      = "File deleted",
///       .action_label = "Undo",
///       .on_action    = [](){ /* undo */ },
///       .duration_ms  = 4000,
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <functional>
#include <chrono>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// SnackbarConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Snackbar widget.
struct SnackbarConfig {
    std::string            message;                          ///< Main message text.
    std::string            action_label  = "";               ///< Action button label (empty = no button).
    std::function<void()>  on_action;                        ///< Action button callback.
    std::function<void()>  on_dismissed;                     ///< Called when auto-dismissed or closed.

    int    duration_ms      = 4000;          ///< Auto-dismiss delay (ms). 0 = no auto-dismiss.
    Color  background_color = 0xFF1E293B;
    Color  text_color       = 0xFFF8FAFC;
    Color  action_color     = 0xFF38BDF8;    ///< Action button text color.
    float  border_radius    = 8.0f;
    float  font_size        = 14.0f;
    float  max_width        = 480.0f;        ///< Max snackbar width (centered).
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Snackbar widget.
std::shared_ptr<Widget> snackbar(SnackbarConfig config);

} // namespace vaura
