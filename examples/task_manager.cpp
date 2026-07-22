#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/text_field.hpp"
#include "vaura/widgets/button.hpp"
#include "vaura/widgets/checkbox.hpp"
#include "vaura/widgets/scroll_view.hpp"
#include <memory>
#include <vector>
#include <string>

using namespace vaura;

struct Task {
    std::string title;
    bool is_done;
};

class TaskManagerApp : public StatefulWidget {
public:
    [[nodiscard]] std::string_view typeName() const override { return "TaskManagerApp"; }
    std::unique_ptr<State> createState() override;
};

class TaskManagerAppState : public State {
private:
    std::vector<Task> tasks_;
    std::string new_task_title_;

public:
    void initState() override {
        State::initState();
        tasks_.push_back({"Learn VAURA Framework", false});
        tasks_.push_back({"Integrate Yoga Layout Engine", true});
        tasks_.push_back({"Build a Custom Backend", false});
    }

    WidgetPtr build(BuildContext& ctx) override {
        // Main container
        auto root = std::make_shared<FlexBox>();
        root->widthPercent(100)
            .heightPercent(100)
            .flexDirection(YGFlexDirectionColumn)
            .backgroundColor(0xFF0F172A); // Dark slate background

        // Header
        auto header = std::make_shared<FlexBox>();
        header->widthPercent(100)
              .padding(YGEdgeAll, 24)
              .backgroundColor(0xFF1E293B)
              .justifyContent(YGJustifyCenter)
              .alignItems(YGAlignCenter);

        auto title = text("Task Manager", {
            .font_size = 28,
            .color = 0xFFF8FAFC,
            .weight = TextStyle::Bold
        });
        header->child(title);

        // Task List Container
        auto list_container = std::make_shared<FlexBox>();
        list_container->widthPercent(100)
                      .padding(YGEdgeAll, 24)
                      .flexDirection(YGFlexDirectionColumn);

        if (tasks_.empty()) {
            auto empty_text = text("No tasks yet. Add one below!", {
                .font_size = 18,
                .color = 0xFF64748B,
                .weight = TextStyle::Regular
            });
            auto empty_container = std::make_shared<FlexBox>();
            empty_container->widthPercent(100)
                           .padding(YGEdgeAll, 32)
                           .justifyContent(YGJustifyCenter)
                           .alignItems(YGAlignCenter)
                           .child(empty_text);
            list_container->child(empty_container);
        }

        for (size_t i = 0; i < tasks_.size(); ++i) {
            auto& task = tasks_[i];
            
            auto task_row = std::make_shared<FlexBox>();
            task_row->widthPercent(100)
                    .padding(YGEdgeAll, 16)
                    .margin(YGEdgeBottom, 12)
                    .backgroundColor(0xFF1E293B)
                    .borderRadius(8)
                    .flexDirection(YGFlexDirectionRow)
                    .alignItems(YGAlignCenter);

            auto chk = checkbox({
                .value = task.is_done,
                .on_changed = [this, i](bool val) {
                    setState([this, i, val] {
                        tasks_[i].is_done = val;
                    });
                },
                .size = 28.0f,
                .active_color = 0xFF10B981, // Emerald green
                .inactive_color = 0xFF64748B
            });

            auto task_text_container = std::make_shared<FlexBox>();
            task_text_container->flexGrow(1.0f).margin(YGEdgeHorizontal, 5);
            
            auto task_text = text(task.title, {
                .font_size = 18,
                .color = task.is_done ? 0xFF64748B : 0xFFF8FAFC,
                .weight = task.is_done ? TextStyle::Regular : TextStyle::Medium
            });
            task_text_container->child(task_text);

            auto delete_btn = button({
                .child = text("Delete", {
                    .font_size = 14,
                    .color = 0xFFFFFFFF,
                    .weight = TextStyle::Bold
                }),
                .on_pressed = [this, i]() {
                    setState([this, i] {
                        tasks_.erase(tasks_.begin() + i);
                    });
                },
                .color = 0xFFEF4444, // Red
                .hover_color = 0xFFDC2626,
                .pressed_color = 0xFFB91C1C,
                .border_radius = 6,
                .padding = EdgeInsets(8, 16, 8, 16)
            });

            auto chk_wrapper = std::make_shared<FlexBox>();
            chk_wrapper->margin(YGEdgeRight, 10).child(chk);

            task_row->child(task_text_container).child(chk_wrapper).child(delete_btn);
            list_container->child(task_row);
        }

        auto scroll = scroll_view({
            .child = list_container,
            .show_scrollbar = true
        });

        // Footer / Input Area
        auto footer = std::make_shared<FlexBox>();
        footer->widthPercent(100)
              .padding(YGEdgeAll, 24)
              .backgroundColor(0xFF1E293B)
              .flexDirection(YGFlexDirectionRow)
              .alignItems(YGAlignCenter);

        auto input_field = text_field({
            .placeholder = "Add a new task...",
            .initial_text = new_task_title_,
            .on_changed = [this](std::string_view text) { new_task_title_ = text; },
            .on_submitted = [this](std::string_view text) {
                if (!new_task_title_.empty()) {
                    setState([this] {
                        tasks_.push_back({new_task_title_, false});
                        new_task_title_.clear();
                    });
                }
            },
            .bg_color = 0x00000000,
            .text_color = 0xFFF8FAFC,
            .placeholder_color = 0xFF64748B,
            .cursor_color = 0xFF38BDF8,
            .font_size = 16,
            .border_radius = 8,
            .width_percent = 100
        });

        auto input_container = std::make_shared<FlexBox>();
        input_container->flexGrow(1.0f)
                       .backgroundColor(0xFF0F172A)
                       .borderRadius(8)
                       .margin(YGEdgeRight, 16)
                       .child(input_field);

        auto add_btn = button({
            .child = text("Add Task", {
                .font_size = 16,
                .color = 0xFFFFFFFF,
                .weight = TextStyle::Bold
            }),
            .on_pressed = [this]() {
                if (!new_task_title_.empty()) {
                    setState([this] {
                        tasks_.push_back({new_task_title_, false});
                        new_task_title_.clear();
                    });
                }
            },
            .color = 0xFF3B82F6, // Blue
            .hover_color = 0xFF2563EB,
            .pressed_color = 0xFF1D4ED8,
            .border_radius = 8,
            .padding = EdgeInsets(16, 24, 16, 24)
        });

        footer->child(input_container).child(add_btn);

        root->child(header);
        root->child(scroll);
        root->child(footer);

        return root;
    }
};

std::unique_ptr<State> TaskManagerApp::createState() {
    return std::make_unique<TaskManagerAppState>();
}

int main(int argc, char** argv) {
    AppConfig config{
        .title = "Task Manager - VAURA",
        .width = 600,
        .height = 800,
        .resizable = true,
        .vsync = true,
        .msaa = true,
        .clear_color = 0xFF0F172A
    };

    return vaura::runApp(std::make_shared<TaskManagerApp>(), config);
}
