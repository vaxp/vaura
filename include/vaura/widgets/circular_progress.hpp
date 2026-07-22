#pragma once
/// @file circular_progress.hpp
/// @brief CircularProgressIndicator widget — a circular loading indicator.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <optional>

namespace vaura {

struct CircularProgressConfig {
    /// If nullopt, the indicator spins indefinitely (indeterminate).
    /// If provided (0.0 to 1.0), it shows a fixed progress value.
    std::optional<float> value = std::nullopt;
    
    Color color = 0xFF38BDF8; // Sky blue
    Color background_color = 0x00000000; // Transparent
    
    float stroke_width = 4.0f;
    float size = 40.0f;
};

class CircularProgressWidget : public StatefulWidget {
public:
    CircularProgressConfig config;

    explicit CircularProgressWidget(CircularProgressConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "CircularProgress"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

inline WidgetPtr circular_progress(CircularProgressConfig config = {}) {
    return std::make_shared<CircularProgressWidget>(std::move(config));
}

/// Declarative syntax alias for CircularProgress
inline WidgetPtr CircularProgress(CircularProgressConfig config = {}) {
    return circular_progress(std::move(config));
}

} // namespace vaura
