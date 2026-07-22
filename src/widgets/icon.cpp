#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/text.hpp"

namespace vaura {

namespace {
    // Converts a Unicode codepoint to a UTF-8 string.
    std::string codepointToUtf8(uint32_t cp) {
        std::string result;
        if (cp <= 0x7F) {
            result.push_back(static_cast<char>(cp));
        } else if (cp <= 0x7FF) {
            result.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp <= 0xFFFF) {
            result.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        } else if (cp <= 0x10FFFF) {
            result.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        return result;
    }
}

std::shared_ptr<Widget> icon(uint32_t codepoint, IconConfig config) {
    TextConfig text_config;
    text_config.font_size = config.size;
    text_config.color = config.color;
    text_config.font_family = config.font_family;
    text_config.align = TextStyle::Center;
    
    // An icon is essentially a specialized text widget rendering a single glyph.
    return text(codepointToUtf8(codepoint), std::move(text_config));
}

std::shared_ptr<Widget> icon(IconData icon_data, IconConfig config) {
    config.font_family = icon_data.font_family;
    return icon(icon_data.code_point, std::move(config));
}

} // namespace vaura
