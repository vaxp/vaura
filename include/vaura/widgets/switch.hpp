#pragma once
/// @file switch.hpp
/// @brief Switch widget — an animated boolean toggle.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>

namespace vaura {

struct SwitchConfig {
    bool value = false;
    std::function<void(bool)> on_changed;
    
    Color active_color = 0xFF4ADE80;   // Green
    Color inactive_color = 0xFF94A3B8; // Gray
    Color thumb_color = 0xFFFFFFFF;    // White
    
    float width = 44.0f;
    float height = 24.0f;
};

class SwitchWidget : public StatefulWidget {
public:
    SwitchConfig config;

    explicit SwitchWidget(SwitchConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "Switch"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

inline WidgetPtr toggle_switch(SwitchConfig config) {
    return std::make_shared<SwitchWidget>(std::move(config));
}

/// Declarative syntax alias for Switch
inline WidgetPtr Switch(SwitchConfig config) {
    return toggle_switch(std::move(config));
}

} // namespace vaura
