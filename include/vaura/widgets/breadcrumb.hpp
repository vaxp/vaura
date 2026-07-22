#pragma once
/// @file breadcrumb.hpp
/// @brief Breadcrumb widget — hierarchical navigation trail.
///
/// Usage:
///   breadcrumb({
///       .items = {
///           {"Home",     [](){ nav.go("/"); }},
///           {"Products", [](){ nav.go("/products"); }},
///           {"Phone",    nullptr},  // current page — not clickable
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
// BreadcrumbItem
// ════════════════════════════════════════════════════════════════

/// @brief A single crumb in the navigation trail.
struct BreadcrumbItem {
    std::string            label;             ///< Display text.
    std::function<void()>  on_tap = nullptr;  ///< nullptr → current page (not tappable).
    WidgetPtr              icon   = nullptr;  ///< Optional leading icon.
};

// ════════════════════════════════════════════════════════════════
// BreadcrumbConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the Breadcrumb widget.
struct BreadcrumbConfig {
    std::vector<BreadcrumbItem> items;

    std::string  separator         = "/";         ///< Text between crumbs.
    Color        active_color      = 0xFFF8FAFC;  ///< Color of the current (last) crumb.
    Color        inactive_color    = 0xFF64748B;  ///< Color of parent crumbs.
    Color        separator_color   = 0xFF475569;  ///< Separator text color.
    Color        hover_color       = 0xFF94A3B8;  ///< Hover color for clickable crumbs.
    float        font_size         = 14.0f;
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a Breadcrumb widget.
std::shared_ptr<Widget> breadcrumb(BreadcrumbConfig config);

} // namespace vaura
