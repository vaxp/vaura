#pragma once
/// @file bottom_sheet.hpp
/// @brief BottomSheet widget — a panel that slides up from the bottom of the screen.
///
/// Usage:
///   bottom_sheet({
///       .child    = my_content_widget,
///       .is_open  = show_sheet_,
///       .on_close = [this]() { setState([this]{ show_sheet_ = false; }); },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// BottomSheetConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the BottomSheet widget.
struct BottomSheetConfig {
    WidgetPtr             child;                              ///< Content of the sheet.
    WidgetPtr             body;                              ///< The background body widget (shown behind).
    bool                  is_open              = false;      ///< Controls visibility externally.
    std::function<void()> on_close;                          ///< Called when the user dismisses the sheet.

    Color  background_color    = 0xFF1E293B;                 ///< Sheet panel background.
    Color  handle_color        = 0xFF475569;                 ///< Drag handle bar color.
    Color  backdrop_color      = 0x99000000;                 ///< Semi-transparent backdrop.
    float  border_radius       = 20.0f;                      ///< Top corner radius.
    float  max_height_percent  = 60.0f;                      ///< Max sheet height as % of parent.
    bool   show_handle         = true;                       ///< Show a small drag handle bar.
    bool   dismiss_on_backdrop = true;                       ///< Tap backdrop to dismiss.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a BottomSheet widget.
std::shared_ptr<Widget> bottom_sheet(BottomSheetConfig config);

/// Declarative syntax alias for BottomSheet
inline std::shared_ptr<Widget> BottomSheet(BottomSheetConfig config) {
    return bottom_sheet(std::move(config));
}

} // namespace vaura
