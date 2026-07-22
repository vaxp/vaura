#pragma once
/// @file avatar.hpp
/// @brief Avatar widget — renders a rounded image or initials.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/rendering/color.hpp"
#include <string>

namespace vaura {

struct AvatarConfig {
    float size = 40.0f;                    ///< Diameter of the avatar
    Color background_color = 0xFFCBD5E1;   ///< Background color if no image
    std::string image_path = "";           ///< Optional image path
    std::string initials = "";             ///< Initials to show if no image
    Color text_color = 0xFFFFFFFF;         ///< Color of the initials
    float font_size = 16.0f;               ///< Font size of the initials
};

/// @brief Creates an avatar widget (rounded image or initials).
std::shared_ptr<Widget> avatar(AvatarConfig config);

} // namespace vaura
