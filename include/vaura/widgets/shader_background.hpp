#pragma once
/// @file shader_background.hpp
/// @brief A widget that fills its layout bounds with a custom SkSL shader.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>

namespace vaura {

struct ShaderBackgroundConfig {
    std::string sksl_code;
};

class ShaderBackgroundWidget : public StatefulWidget {
public:
    ShaderBackgroundConfig config;

    explicit ShaderBackgroundWidget(ShaderBackgroundConfig cfg) : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override { return "ShaderBackground"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

} // namespace vaura
