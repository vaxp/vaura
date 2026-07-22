#pragma once
/// @file icon.hpp
/// @brief Icon widget — renders an icon font glyph.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/rendering/color.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/widgets/icons.hpp"
#include <string>

namespace vaura {

/// @brief Configuration for the Icon widget.
struct IconConfig {
    float size = 24.0f;                        ///< Size of the icon in logical pixels.
    Color color = Colors::Black;               ///< Color of the icon.
    std::string font_family = "CupertinoIcons"; ///< Font family to use for rendering.
};

/// @brief A widget that renders a single icon from a font.
/// @param codepoint The Unicode codepoint of the icon (e.g., 0xf448).
/// @param config Appearance configuration for the icon.
/// @return A configured icon widget.
std::shared_ptr<Widget> icon(uint32_t codepoint, IconConfig config = {});

/// @brief A widget that renders a single icon from predefined IconData.
/// @param icon_data The predefined IconData struct.
/// @param config Appearance configuration for the icon (font_family is overridden by icon_data).
/// @return A configured icon widget.
std::shared_ptr<Widget> icon(IconData icon_data, IconConfig config = {});

/// Declarative syntax alias for Icon (codepoint)
inline std::shared_ptr<Widget> Icon(uint32_t codepoint, IconConfig config = {}) {
    return icon(codepoint, std::move(config));
}

/// Declarative syntax alias for Icon (IconData)
inline std::shared_ptr<Widget> Icon(IconData icon_data, IconConfig config = {}) {
    return icon(icon_data, std::move(config));
}

} // namespace vaura
