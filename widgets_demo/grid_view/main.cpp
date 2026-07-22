#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/widgets/grid_view.hpp"
#include "vaura/widgets/image_widget.hpp"

using namespace vaura;

class GridViewDemoApp : public StatelessWidget {
public:
    std::string_view typeName() const override { return "GridViewDemoApp"; }

    WidgetPtr build(BuildContext& ctx) override {
        // --- Scenario 1: Photo Gallery Grid ---
        std::vector<WidgetPtr> gallery_items;
        std::vector<std::string> images = {"assets/0.png", "assets/1.png", "assets/2.png", "assets/3.png", "assets/5.png", "assets/6.png"};
        for (int i = 0; i < 6; ++i) {
            gallery_items.push_back(ImageView({
                .path = images[i],
                .width_percent = 100.0f,
                .height_percent = 100.0f,
                .fit = BoxFit::Cover,
                .border_radius = 8.0f
            }));
        }
        auto gallery_grid = GridView({
            .crossAxisCount = 3,
            .mainAxisSpacing = 16.0f,
            .crossAxisSpacing = 16.0f,
            .childAspectRatio = 1.0f, // Perfect squares
            .children = gallery_items
        });

        // --- Scenario 2: Product Catalog Grid ---
        std::vector<WidgetPtr> catalog_items;
        std::vector<std::string> product_images = {"assets/7.png", "assets/8.png", "assets/9.png", "assets/12.png"};
        for (int i = 0; i < 4; ++i) {
                            auto add_btn = Column({
                                .padding = std::pair{YGEdgeHorizontal, 12.0f},
                                .backgroundColor = 0xFF111827,
                                .borderRadius = 4.0f,
                                .children = {
                                    Text("Add", { .font_size = 12, .color = 0xFFFFFFFF })
                                }
                            });
                            add_btn->padding(YGEdgeVertical, 6.0f);
                            
                            auto cell = Column({
                                .widthPercent = 100.0f,
                                .heightPercent = 100.0f,
                                .backgroundColor = 0xFFFFFFFF,
                                .borderRadius = 12.0f,
                                .children = {
                                    Column({
                                        .widthPercent = 100.0f,
                                        .flexGrow = 1.0f,
                                        .children = {
                                            ImageView({
                                                .path = product_images[i],
                                                .width_percent = 100.0f,
                                                .height_percent = 100.0f,
                                                .fit = BoxFit::Cover,
                                                .border_radius = 8.0f
                                            })
                                        }
                                    }),
                                    Text("Product " + std::to_string(i + 1), {
                                        .font_size = 18,
                                        .color = 0xFF111827,
                                        .weight = TextStyle::SemiBold
                                    }),
                                    Row({
                                        .justifyContent = YGJustifySpaceBetween,
                                        .margin = std::pair{YGEdgeTop, 8.0f},
                                        .children = {
                                            Text("$" + std::to_string(99 + i * 10) + ".99", {
                                                .font_size = 16,
                                                .color = 0xFF4F46E5,
                                                .weight = TextStyle::Bold
                                            }),
                                            add_btn
                                        }
                                    })
                                }
                            });
                            cell->padding(YGEdgeAll, 16.0f);
                            catalog_items.push_back(cell);
        }
        auto catalog_grid = GridView({
            .crossAxisCount = 2,
            .mainAxisSpacing = 24.0f,
            .crossAxisSpacing = 24.0f,
            .childAspectRatio = 0.7f, // Portrait cards
            .children = catalog_items
        });

        // --- Scenario 3: Dashboard Metrics Grid ---
        std::vector<WidgetPtr> metric_items;
        std::vector<std::string> labels = {"Total Users", "Revenue", "Active Sessions", "Bounce Rate"};
        std::vector<std::string> values = {"12.4K", "$45,200", "842", "24%"};
        for (int i = 0; i < 4; ++i) {
            auto cell = Column({
                .justifyContent = YGJustifyCenter,
                .widthPercent = 100.0f,
                .heightPercent = 100.0f,
                .backgroundColor = 0xFFFFFFFF,
                .borderRadius = 12.0f,
                .children = {
                    Text(labels[i], {
                        .font_size = 14,
                        .color = 0xFF6B7280,
                        .weight = TextStyle::Medium
                    }),
                    Text(values[i], {
                        .font_size = 28,
                        .color = 0xFF111827,
                        .weight = TextStyle::Bold
                    })
                }
            });
            cell->padding(YGEdgeAll, 20.0f);
            metric_items.push_back(cell);
        }
        auto metric_grid = GridView({
            .crossAxisCount = 4,
            .mainAxisSpacing = 16.0f,
            .crossAxisSpacing = 16.0f,
            .childAspectRatio = 2.0f, // Wide cards
            .children = metric_items
        });

        // Assemble Demo
        return Column({
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFFF3F4F6, // Gray 100
            .children = {
                ScrollView({
                    .child = Column({
                        .widthPercent = 100.0f,
                        .padding = std::pair{YGEdgeAll, 40.0f},
                        .children = {
                            Text("VAURA GridView Demo", {
                                .font_size = 32,
                                .color = 0xFF111827,
                                .weight = TextStyle::ExtraBold
                            }),
                            Column({ .height = 40.0f }),
                            
                            Text("Scenario 1: Photo Gallery (Aspect Ratio 1:1, 3 Columns)", {
                                .font_size = 18,
                                .color = 0xFF4B5563,
                                .weight = TextStyle::SemiBold
                            }),
                            Column({ .height = 16.0f }),
                            gallery_grid,
                            
                            Column({ .height = 60.0f }),
                            
                            Text("Scenario 2: Product Catalog (Aspect Ratio 0.7:1, 2 Columns)", {
                                .font_size = 18,
                                .color = 0xFF4B5563,
                                .weight = TextStyle::SemiBold
                            }),
                            Column({ .height = 16.0f }),
                            catalog_grid,
                            
                            Column({ .height = 60.0f }),

                            Text("Scenario 3: Dashboard Metrics (Aspect Ratio 2:1, 4 Columns)", {
                                .font_size = 18,
                                .color = 0xFF4B5563,
                                .weight = TextStyle::SemiBold
                            }),
                            Column({ .height = 16.0f }),
                            metric_grid
                        }
                    })
                })
            }
        });
    }
};

int main(int argc, char** argv) {
    AppConfig config{
        .title = "VAURA GridView Component",
        .width = 900,
        .height = 800,
        .vsync = true
    };
    return vaura::runApp(std::make_shared<GridViewDemoApp>(), config);
}
