/// @file overlay.cpp
/// @brief OverlayController + OverlayStack implementation.

#include "vaura/overlay/overlay.hpp"
#include "vaura/widgets/flexbox.hpp"
#include "vaura/tree/widget.hpp"
#include "vaura/state/state.hpp"
#include "vaura/tree/build_context.hpp"
#include <cassert>
#include <algorithm>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// OverlayController
// ════════════════════════════════════════════════════════════════

OverlayController& OverlayController::instance() {
    static OverlayController controller;
    return controller;
}

OverlayId OverlayController::insert(OverlayEntry entry) {
    OverlayId id = next_id_++;
    entries_.push_back({id, std::move(entry)});
    dirty_ = true;
    if (on_changed_) on_changed_();
    return id;
}

void OverlayController::remove(OverlayId id) {
    auto it = std::find_if(entries_.begin(), entries_.end(),
                           [id](const EntryRecord& r) { return r.id == id; });
    if (it != entries_.end()) {
        entries_.erase(it);
        dirty_ = true;
        if (on_changed_) on_changed_();
    }
}

void OverlayController::clear() {
    if (!entries_.empty()) {
        entries_.clear();
        dirty_ = true;
        if (on_changed_) on_changed_();
    }
}

bool OverlayController::hasEntries() const {
    return !entries_.empty();
}

size_t OverlayController::entryCount() const {
    return entries_.size();
}

void OverlayController::setOnChanged(std::function<void()> callback) {
    on_changed_ = std::move(callback);
}

// ════════════════════════════════════════════════════════════════
// OverlayStack — StatefulWidget
// ════════════════════════════════════════════════════════════════

class OverlayStackWidget : public StatefulWidget {
public:
    std::shared_ptr<Widget> root;
    explicit OverlayStackWidget(std::shared_ptr<Widget> r) : root(std::move(r)) {}
    [[nodiscard]] std::string_view typeName() const override { return "OverlayStack"; }
    [[nodiscard]] std::unique_ptr<State> createState() override;
};

class OverlayStackState : public State {
public:
    void initState() override {
        State::initState();
        // Register for overlay change notifications — triggers rebuild
        OverlayController::instance().setOnChanged([this]() {
            setState([]{});
        });
    }

    void dispose() override {
        OverlayController::instance().setOnChanged(nullptr);
        State::dispose();
    }

    WidgetPtr build(BuildContext& ctx) override {
        auto* sw = static_cast<const OverlayStackWidget*>(widget());
        auto& overlay = OverlayController::instance();

        // Root content layer — always full screen
        auto root_layer = std::make_shared<FlexBox>();
        root_layer->widthPercent(100).heightPercent(100);
        root_layer->child(sw->root);

        // Stack: root_layer + overlay entries (each is full-screen absolute)
        auto stack = std::make_shared<FlexBox>();
        stack->widthPercent(100).heightPercent(100);
        stack->child(root_layer);

        // Iterate entries directly (friend access via OverlayStackState)
        for (const auto& record : overlay.entries_) {
            if (!record.entry.content) continue;

            auto overlay_layer = std::make_shared<FlexBox>();
            overlay_layer->positionType(YGPositionTypeAbsolute)
                          .position(YGEdgeAll, 0.0f)
                          .widthPercent(100)
                          .heightPercent(100);
            overlay_layer->child(record.entry.content);
            stack->child(overlay_layer);
        }

        return stack;
    }
};

std::unique_ptr<State> OverlayStackWidget::createState() {
    return std::make_unique<OverlayStackState>();
}

std::shared_ptr<Widget> overlay_stack(std::shared_ptr<Widget> root) {
    return std::make_shared<OverlayStackWidget>(std::move(root));
}

}  // namespace vaura
