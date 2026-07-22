// widgets_demo/gesture_detector/main.cpp
#include "vaura/app/app.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/widgets/text.hpp"
#include "vaura/widgets/gesture_detector.hpp"
#include "vaura/state/state.hpp"
#include <string>

using namespace vaura;

class GestureDetectorDemoState : public State {
    std::string status = "Waiting for gesture...";
    Color box_color = 0xFF334155; // Slate 700
    bool is_hovered = false;

public:
    WidgetPtr build(BuildContext& ctx) override {
        return Column({
            .justifyContent = YGJustifyCenter,
            .alignItems = YGAlignCenter,
            .widthPercent = 100.0f,
            .heightPercent = 100.0f,
            .backgroundColor = 0xFF0F172A,
            .children = {
                Column({
                    .margin = std::pair{YGEdgeBottom, 40.0f},
                    .children = {
                        Text("Gesture Detector", {.font_size = 28.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                    }
                }),
                Column({
                    .children = {
                        GestureDetector({
                            .child = Column({
                                .justifyContent = YGJustifyCenter,
                                .alignItems = YGAlignCenter,
                                .width = 300.0f,
                                .height = 300.0f,
                                .backgroundColor = static_cast<uint32_t>(is_hovered ? 0xFF475569 : box_color),
                                .borderRadius = 24.0f,
                                .children = {
                                    Text(status, {.font_size = 18.0f, .color = 0xFFFFFFFF, .weight = TextStyle::Bold})
                                }
                            }),
                            .on_tap = [this]() {
                                setState([this]() { status = "Tapped!"; });
                            },
                            .on_tap_down = [this]() {
                                setState([this]() { box_color = 0xFF0EA5E9; status = "Pointer Down!"; });
                            },
                            .on_tap_up = [this]() {
                                setState([this]() { box_color = 0xFF10B981; status = "Pointer Up!"; });
                            },
                            .on_hover_enter = [this]() {
                                setState([this]() { is_hovered = true; status = "Hovering!"; });
                            },
                            .on_hover_exit = [this]() {
                                setState([this]() { is_hovered = false; status = "Waiting for gesture..."; box_color = 0xFF334155; });
                            },
                            .on_right_click = [this](Point p) {
                                setState([this]() { box_color = 0xFFEF4444; status = "Right Clicked!"; });
                            }
                        })
                    }
                })
            }
        });
    }
};

class GestureDetectorDemo : public StatefulWidget {
public:
    std::string_view typeName() const override { return "GestureDetectorDemo"; }
    std::unique_ptr<State> createState() override { return std::make_unique<GestureDetectorDemoState>(); }
};

int main(int argc, char** argv) {
    AppConfig config;
    config.title = "VAURA Gesture Detector Demo";
    config.width = 800;
    config.height = 600;
    return runApp(std::make_shared<GestureDetectorDemo>(), config);
}
