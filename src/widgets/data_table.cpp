/// @file data_table.cpp
/// @brief DataTable widget implementation.

#include "vaura/widgets/data_table.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/container.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/icon.hpp"
#include "vaura/widgets/icons.hpp"
#include "vaura/widgets/checkbox.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <layout_engine/Yoga.h>
#include <numeric>

namespace vaura {

class DataTableWidget : public StatefulWidget {
public:
    DataTableConfig config;
    explicit DataTableWidget(DataTableConfig cfg) : config(std::move(cfg)) {}
    [[nodiscard]] std::string_view typeName() const override { return "DataTable"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class DataTableState : public State {
public:
    int hovered_row = -1;

    WidgetPtr build(BuildContext& ctx) override {
        auto* dw = static_cast<const DataTableWidget*>(widget());
        const auto& cfg = dw->config;

        // Calculate total table width
        float total_w = 0.0f;
        for (const auto& col : cfg.columns) total_w += col.width > 0 ? col.width : 120.0f;
        if (cfg.show_checkbox)    total_w += 48.0f;
        if (cfg.show_row_numbers) total_w += 48.0f;

        auto table_col = std::make_shared<FlexBox>();
        table_col->flexDirection(YGFlexDirectionColumn).width(total_w);

        // ── Header row ────────────────────────────────────────
        auto header_row = std::make_shared<FlexBox>();
        header_row->flexDirection(YGFlexDirectionRow)
                   .alignItems(YGAlignCenter)
                   .width(total_w)
                   .height(cfg.header_height)
                   .backgroundColor(cfg.header_bg);

        if (cfg.show_row_numbers) {
            auto num_hdr = std::make_shared<FlexBox>();
            num_hdr->width(48).height(cfg.header_height)
                    .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter)
                    .child(text("#", {.font_size = cfg.font_size, .color = cfg.header_text_color,
                                      .weight = TextStyle::SemiBold}));
            header_row->child(num_hdr);
        }
        if (cfg.show_checkbox) {
            auto chk_hdr = std::make_shared<FlexBox>();
            chk_hdr->width(48).height(cfg.header_height)
                    .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter);
            header_row->child(chk_hdr);
        }

        for (int ci = 0; ci < (int)cfg.columns.size(); ++ci) {
            const auto& col = cfg.columns[ci];
            float cw = col.width > 0 ? col.width : 120.0f;
            bool is_sorted = (cfg.sort_column_index == ci);

            auto hdr_cell = std::make_shared<FlexBox>();
            hdr_cell->flexDirection(YGFlexDirectionRow)
                     .alignItems(YGAlignCenter)
                     .width(cw)
                     .height(cfg.header_height)
                     .padding(YGEdgeLeft, 12)
                     .padding(YGEdgeRight, 8);

            auto lbl = text(col.label, {
                .font_size = cfg.font_size,
                .color     = is_sorted ? cfg.sort_icon_color : cfg.header_text_color,
                .weight    = TextStyle::SemiBold,
            });
            hdr_cell->child(lbl);

            if (col.sortable) {
                auto sort_icon_widget = icon(
                    is_sorted
                        ? (cfg.sort_ascending ? Icons::ArrowUp : Icons::ArrowDown)
                        : Icons::ArrowUpDown,
                    {.size = 14.0f, .color = is_sorted ? cfg.sort_icon_color : cfg.header_text_color});

                auto iw = std::make_shared<FlexBox>();
                iw->margin(YGEdgeLeft, 4).child(sort_icon_widget);
                hdr_cell->child(iw);

                int ci2 = ci;
                bool cur_asc = cfg.sort_ascending;
                bool is_sort = is_sorted;
                auto fn = cfg.on_sort;
                auto sortable_hdr = gesture_detector({
                    .child  = hdr_cell,
                    .on_tap = [fn, ci2, is_sort, cur_asc]() {
                        if (fn) fn(ci2, is_sort ? !cur_asc : true);
                    },
                });
                header_row->child(sortable_hdr);
            } else {
                header_row->child(hdr_cell);
            }
        }

        table_col->child(header_row);

        // ── Data rows ─────────────────────────────────────────
        for (int ri = 0; ri < (int)cfg.rows.size(); ++ri) {
            const auto& row = cfg.rows[ri];
            bool is_hov = (hovered_row == ri);

            Color row_bg = row.selected   ? cfg.selected_color
                         : is_hov         ? cfg.hover_color
                         : ri % 2 == 0    ? cfg.row_color
                         :                  cfg.alt_row_color;

            auto data_row = std::make_shared<FlexBox>();
            data_row->flexDirection(YGFlexDirectionRow)
                     .alignItems(YGAlignCenter)
                     .width(total_w)
                     .height(cfg.row_height)
                     .backgroundColor(row_bg);

            // Divider (top border)
            auto div_wrap = std::make_shared<FlexBox>();
            div_wrap->flexDirection(YGFlexDirectionColumn).width(total_w);

            if (ri > 0) {
                auto divider = std::make_shared<FlexBox>();
                divider->width(total_w).height(1).backgroundColor(cfg.border_color);
                div_wrap->child(divider);
            }

            if (cfg.show_row_numbers) {
                auto num_cell = std::make_shared<FlexBox>();
                num_cell->width(48).height(cfg.row_height)
                         .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter)
                         .child(text(std::to_string(ri + 1), {
                             .font_size = cfg.font_size - 1.0f,
                             .color     = 0xFF64748B,
                         }));
                data_row->child(num_cell);
            }
            if (cfg.show_checkbox) {
                auto chk_cell = std::make_shared<FlexBox>();
                chk_cell->width(48).height(cfg.row_height)
                          .justifyContent(YGJustifyCenter).alignItems(YGAlignCenter);
                int ri2 = ri;
                bool sel = row.selected;
                auto fn2 = cfg.on_row_select;
                chk_cell->child(checkbox({
                    .value      = sel,
                    .on_changed = [fn2, ri2](bool v) { if (fn2) fn2(ri2, v); },
                }));
                data_row->child(chk_cell);
            }

            for (int ci = 0; ci < (int)cfg.columns.size() && ci < (int)row.cells.size(); ++ci) {
                const auto& col = cfg.columns[ci];
                float cw = col.width > 0 ? col.width : 120.0f;

                auto cell_wrap = std::make_shared<FlexBox>();
                cell_wrap->width(cw).height(cfg.row_height)
                          .padding(YGEdgeLeft, 12).padding(YGEdgeRight, 8)
                          .alignItems(YGAlignCenter);
                if (row.cells[ci]) cell_wrap->child(row.cells[ci]);
                data_row->child(cell_wrap);
            }

            int ri3 = ri;
            auto row_fn = row.on_tap;
            auto row_gd = gesture_detector({
                .child          = data_row,
                .on_tap         = row_fn,
                .on_hover_enter = [this, ri3]() { setState([this, ri3]{ hovered_row = ri3; }); },
                .on_hover_exit  = [this]()      { setState([this]{ hovered_row = -1; }); },
            });

            div_wrap->child(row_gd);
            table_col->child(div_wrap);
        }

        // Wrap in container with border-radius
        auto outer = container({
            .color         = cfg.row_color,
            .border_radius = BorderRadius::circular(cfg.border_radius),
            .border        = Border{cfg.border_color, 1.0f},
            .clip_content  = true,
            .child         = table_col,
        });

        return outer;
    }
};

std::unique_ptr<State> DataTableWidget::createState() {
    return std::make_unique<DataTableState>();
}

std::shared_ptr<Widget> data_table(DataTableConfig config) {
    return std::make_shared<DataTableWidget>(std::move(config));
}

} // namespace vaura
