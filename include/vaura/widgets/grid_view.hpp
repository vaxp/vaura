#pragma once

#include "vaura/core/types.hpp"
#include "vaura/widgets/flexbox.hpp"
#include <vector>
#include <memory>
#include <cmath>

namespace vaura {

struct GridViewConfig {
    int crossAxisCount = 2;
    float mainAxisSpacing = 0.0f;
    float crossAxisSpacing = 0.0f;
    float childAspectRatio = 1.0f; // 0.0f means auto height

    float width = YGUndefined;
    float height = YGUndefined;
    float widthPercent = YGUndefined;
    float heightPercent = YGUndefined;
    float flexGrow = 0.0f;
    float flexShrink = 0.0f;
    std::optional<std::pair<YGEdge, float>> padding = std::nullopt;
    std::optional<std::pair<YGEdge, float>> margin = std::nullopt;
    Color backgroundColor = 0x00000000;
    
    std::vector<WidgetPtr> children;
};

class GridViewWidget : public StatelessWidget {
public:
    GridViewConfig config;

    explicit GridViewWidget(GridViewConfig cfg) : config(std::move(cfg)) {}

    std::string_view typeName() const override { return "GridView"; }

    WidgetPtr build(BuildContext& ctx) override {
        std::vector<WidgetPtr> grid_cells;
        
        float cellWidthPercent = 100.0f / static_cast<float>(config.crossAxisCount);
        
        for (const auto& child : config.children) {
            auto cell = Column({
                .widthPercent = cellWidthPercent,
                .padding = std::pair{YGEdgeAll, 0.0f}, // Replaced below manually to support multiple margins
                .children = { child }
            });
            
            // Set padding based on spacing
            cell->padding(YGEdgeHorizontal, config.crossAxisSpacing / 2.0f);
            cell->padding(YGEdgeVertical, config.mainAxisSpacing / 2.0f);
            
            if (config.childAspectRatio > 0.0f) {
                cell->aspectRatio(config.childAspectRatio);
            }
            
            grid_cells.push_back(cell);
        }
        
        auto row = Row({
            .flexWrap = YGWrapWrap,
            .width = config.width,
            .widthPercent = config.widthPercent,
            .height = config.height,
            .heightPercent = config.heightPercent,
            .flexGrow = config.flexGrow,
            .flexShrink = config.flexShrink,
            .padding = config.padding,
            .margin = config.margin,
            .backgroundColor = config.backgroundColor,
            .children = grid_cells
        });
        
        return row;
    }
};

inline WidgetPtr GridView(GridViewConfig config) {
    return std::make_shared<GridViewWidget>(std::move(config));
}

} // namespace vaura
