#pragma once
/// @file stepper.hpp
/// @brief Stepper widget — a wizard-like multi-step progress component.
///
/// Usage:
///   stepper({
///       .steps = {
///           {"Account",  "Fill in your details",  account_form,  false},
///           {"Plan",     "Choose a plan",          plan_form,     true},
///           {"Payment",  "Enter payment info",     payment_form,  false},
///       },
///       .current_step   = current_step_,
///       .on_step_changed = [this](int s){ setState([this, s]{ current_step_ = s; }); },
///       .on_complete    = [](){ /* submit */ },
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
// StepperStep
// ════════════════════════════════════════════════════════════════

/// @brief A single step in the Stepper.
struct StepperStep {
    std::string title;                ///< Step title shown in the header.
    std::string subtitle    = "";     ///< Optional sub-text below the title.
    WidgetPtr   content;              ///< Content shown when this step is active.
    bool        is_optional = false;  ///< Shows "Optional" badge.
};

// ════════════════════════════════════════════════════════════════
// StepperConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Stepper widget.
struct StepperConfig {
    std::vector<StepperStep> steps;                         ///< The ordered list of steps.
    int                      current_step   = 0;            ///< Index of the active step.
    std::function<void(int)> on_step_changed;               ///< Called when navigating steps (provides new index).
    std::function<void()>    on_complete;                   ///< Called when the last step's "Finish" is tapped.
    bool                     linear         = true;         ///< If true, steps must be completed in order.

    Color  active_color      = 0xFF0EA5E9;   ///< Active step circle & connector.
    Color  completed_color   = 0xFF22C55E;   ///< Completed step circle.
    Color  inactive_color    = 0xFF475569;   ///< Future step circle.
    Color  background_color  = 0xFF1E293B;   ///< Step content background.
    Color  text_color        = 0xFFF8FAFC;
    Color  subtitle_color    = 0xFF94A3B8;
    Color  connector_color   = 0xFF334155;   ///< Line between step circles.
    float  circle_size       = 32.0f;
    float  connector_height  = 40.0f;        ///< Height of the line between circles.
    float  font_size         = 14.0f;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Stepper widget.
std::shared_ptr<Widget> stepper(StepperConfig config);

} // namespace vaura
