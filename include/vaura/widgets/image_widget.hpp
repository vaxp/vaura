#pragma once
/// @file image_widget.hpp
/// @brief Image widget.
///
/// Loads and renders an image from the filesystem.
/// 
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/tree/render_object.hpp"
#include <string>
#include <optional>

namespace vaura {

enum class BoxFit {
    Fill,       ///< Stretch to fill bounds (distorts aspect ratio)
    Contain,    ///< Scale to fit inside bounds while preserving aspect ratio
    Cover       ///< Scale to fill bounds while preserving aspect ratio (clips overflow)
};

struct ImageConfig {
    std::string path;                                   ///< Filepath to the image
    std::optional<float> width;                         ///< Fixed width
    std::optional<float> height;                        ///< Fixed height
    std::optional<float> width_percent;                 ///< Percentage width
    std::optional<float> height_percent;                ///< Percentage height
    BoxFit fit = BoxFit::Contain;                       ///< How the image should be fitted
    float border_radius = 0.0f;                         ///< Optional rounded corners
};

class ImageWidget : public SingleChildRenderObjectWidget {
public:
    ImageConfig config;

    explicit ImageWidget(ImageConfig cfg)
        : config(std::move(cfg)) {}

    [[nodiscard]] std::string_view typeName() const override {
        return "Image";
    }

    [[nodiscard]] std::unique_ptr<RenderObject> createRenderObject(BuildContext& ctx) override;
    void updateRenderObject(BuildContext& ctx, RenderObject& renderObject) override;
};

inline WidgetPtr image(ImageConfig config) {
    return std::make_shared<ImageWidget>(std::move(config));
}

} // namespace vaura
