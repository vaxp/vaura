#pragma once
/// @file slider.hpp
/// @brief Slider widget implementation.

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>
#include <memory>

namespace vaura {

struct SliderConfig {
    float value = 0.0f;
    float min_value = 0.0f;
    float max_value = 1.0f;
    float thumb_radius = 8.0f;

    std::function<void(float)> on_changed;
    std::function<void(float)> on_change_start;
    std::function<void(float)> on_change_end;

    float width = 200.0f;
    Color active_color = 0xFF3B82F6;
    Color inactive_color = 0xFFCBD5E1;
    Color thumb_color = 0xFFFFFFFF;
    float track_height = 4.0f;
};

class SliderWidget : public StatefulWidget {
public:
    SliderConfig config;
    explicit SliderWidget(SliderConfig config) : config(std::move(config)) {}
    [[nodiscard]] std::string_view typeName() const override { return "SliderWidget"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

std::shared_ptr<Widget> slider(SliderConfig config);

} // namespace vaura
