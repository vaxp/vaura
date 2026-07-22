#pragma once
/// @file data_table.hpp
/// @brief DataTable widget — customizable table with sorting, selection, and alternating rows.
///
/// Usage:
///   data_table({
///       .columns = {
///           {"Name",  true,  160.0f},
///           {"Email", false, 220.0f},
///           {"Role",  true,  120.0f},
///       },
///       .rows = {
///           {{ text("Alice"), text("alice@x.com"), text("Admin") }, false, [](){} },
///           {{ text("Bob"),   text("bob@x.com"),   text("User")  }, false, [](){} },
///       },
///       .on_sort = [this](int col, bool asc){ /* sort data */ },
///   })
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <string>
#include <vector>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// DataColumn / DataRow
// ════════════════════════════════════════════════════════════════

/// @brief Defines a column in the DataTable.
struct DataColumn {
    std::string         label;               ///< Column header text.
    bool                sortable   = false;  ///< Whether clicking the header sorts by this column.
    float               width      = 120.0f; ///< Fixed column width. 0 = flex.

    enum class Align { Left, Center, Right } align = Align::Left;
};

/// @brief A single row in the DataTable.
struct DataRow {
    std::vector<WidgetPtr>  cells;           ///< One widget per column.
    bool                    selected = false;///< Highlights the row.
    std::function<void()>   on_tap;          ///< Row tap callback.
};

// ════════════════════════════════════════════════════════════════
// DataTableConfig
// ════════════════════════════════════════════════════════════════

/// @brief Configuration for the DataTable widget.
struct DataTableConfig {
    std::vector<DataColumn>        columns;
    std::vector<DataRow>           rows;

    std::function<void(int, bool)> on_sort;           ///< column_index, ascending
    std::function<void(int, bool)> on_row_select;     ///< row_index, selected

    int    sort_column_index  = -1;
    bool   sort_ascending     = true;
    bool   show_checkbox      = false;        ///< Leading checkbox per row.
    bool   show_row_numbers   = false;        ///< Leading row number column.

    Color  header_bg          = 0xFF0F172A;
    Color  header_text_color  = 0xFF94A3B8;
    Color  row_color          = 0xFF1E293B;
    Color  alt_row_color      = 0xFF172030;   ///< Alternating row background.
    Color  selected_color     = 0xFF0D2137;
    Color  hover_color        = 0xFF253347;
    Color  border_color       = 0xFF334155;
    Color  sort_icon_color    = 0xFF38BDF8;
    Color  text_color         = 0xFFF8FAFC;
    float  row_height         = 52.0f;
    float  header_height      = 56.0f;
    float  font_size          = 13.5f;
    float  border_radius      = 12.0f;        ///< Outer table corner radius.
};

// ════════════════════════════════════════════════════════════════
// DSL Helper
// ════════════════════════════════════════════════════════════════

/// Create a DataTable widget.
std::shared_ptr<Widget> data_table(DataTableConfig config);

/// Declarative syntax alias for DataTable
inline std::shared_ptr<Widget> DataTable(DataTableConfig config) {
    return data_table(std::move(config));
}

} // namespace vaura
