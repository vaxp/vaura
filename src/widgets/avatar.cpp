#include "vaura/widgets/avatar.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/image_widget.hpp"
#include <layout_engine/Yoga.h>

namespace vaura {

std::shared_ptr<Widget> avatar(AvatarConfig config) {
    std::shared_ptr<Widget> child = nullptr;

    if (!config.image_path.empty()) {
        child = image({
            .path = config.image_path,
            .width = config.size,
            .height = config.size,
            .fit = BoxFit::Cover
        });
    } else if (!config.initials.empty()) {
        child = text(config.initials, {
            .font_size = config.font_size,
            .color = config.text_color,
            .weight = TextStyle::SemiBold
        });
    }

    return container({
        .color = config.background_color,
        .width = config.size,
        .height = config.size,
        .border_radius = BorderRadius::circular(config.size / 2.0f),
        .clip_content = true,
        .alignment = Alignment::Center,
        .child = child
    });
}

} // namespace vaura
