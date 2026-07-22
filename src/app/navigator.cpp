/// @file navigator.cpp
/// @brief Navigator implementation — stack-based page navigation.
#include "vaura/app/navigator.hpp"

namespace vaura {

// ════════════════════════════════════════════════════════════════
// NavigatorState::build — renders the top page in the stack
// ════════════════════════════════════════════════════════════════

WidgetPtr NavigatorState::build(BuildContext& ctx) {
    if (stack_.empty()) return nullptr;
    return stack_.back().page;
}

// ════════════════════════════════════════════════════════════════
// NavigatorWidget::createState
// ════════════════════════════════════════════════════════════════

std::unique_ptr<State> NavigatorWidget::createState() {
    auto state = std::make_unique<NavigatorState>();
    state->push(initial_page);
    return state;
}

} // namespace vaura
