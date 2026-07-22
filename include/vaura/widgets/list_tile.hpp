#pragma once
/// @file list_tile.hpp
/// @brief A luxurious, highly reusable row element for lists and menus.
///
/// Features:
/// - Slots for leading, title, subtitle, and trailing widgets.
/// - Built-in hover and active states (if an on_tap callback is provided).
/// - Elegant default padding and typography layout.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>
#include <string>
#include <memory>

namespace vaura {

struct ListTileConfig {
    WidgetPtr leading = nullptr;        ///< A widget to display before the title.
    WidgetPtr title = nullptr;          ///< The primary content of the list tile.
    WidgetPtr subtitle = nullptr;       ///< Additional content displayed below the title.
    WidgetPtr trailing = nullptr;       ///< A widget to display after the title.
    
    std::function<void()> on_tap;       ///< Callback when the tile is tapped.
    
    EdgeInsets content_padding = {16, 12, 16, 12}; ///< Padding inside the tile.
    Color hover_color = 0x0A000000;     ///< Background color on hover (default light grey).
    Color pressed_color = 0x14000000;   ///< Background color when pressed.
};

/// Create a ListTile widget.
std::shared_ptr<Widget> list_tile(ListTileConfig config);

/// Convenience function to create a text-based ListTile quickly.
std::shared_ptr<Widget> list_tile(std::string title_text, std::string subtitle_text = "", WidgetPtr leading = nullptr, WidgetPtr trailing = nullptr, std::function<void()> on_tap = nullptr);

} // namespace vaura
