#pragma once
/// @file accordion.hpp
/// @brief Accordion widget — collapsible section with an animated header.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>

namespace vaura {

struct AccordionConfig {
    std::string title;                  ///< The text title of the accordion header.
    WidgetPtr content = nullptr;        ///< The content to reveal when expanded.
    bool initially_expanded = false;    ///< Whether it starts expanded.
    
    Color background_color = 0xFF1E293B; ///< Background color of the header.
    Color text_color = 0xFFF8FAFC;       ///< Text color of the header.
    Color icon_color = 0xFF94A3B8;       ///< Chevron icon color.
};

/// Create an Accordion widget.
std::shared_ptr<Widget> accordion(AccordionConfig config);

} // namespace vaura
