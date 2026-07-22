#pragma once
/// @file toast.hpp
/// @brief Toast widget — non-actionable ephemeral popup message.
///
/// Simpler than Snackbar: no action button, just a brief text notification.
///
/// Usage:
///   toast({
///       .message     = "Saved successfully!",
///       .duration_ms = 2500,
///       .position    = ToastConfig::Position::Bottom,
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// ToastConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Toast widget.
struct ToastConfig {
    std::string            message;                          ///< The toast message text.
    std::function<void()>  on_dismissed;                     ///< Called after the toast disappears.

    int    duration_ms       = 2500;             ///< How long to show the toast.
    enum class Position { Top, Center, Bottom } position = Position::Bottom;

    Color  background_color  = 0xCC0F172A;       ///< Semi-transparent background.
    Color  text_color        = 0xFFFFFFFF;
    float  border_radius     = 24.0f;            ///< Pill shape.
    float  font_size         = 13.5f;
    float  padding_h         = 20.0f;            ///< Horizontal padding.
    float  padding_v         = 10.0f;            ///< Vertical padding.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Toast widget.
std::shared_ptr<Widget> toast(ToastConfig config);

} // namespace vaura
