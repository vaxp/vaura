#pragma once
/// @file navigator.hpp
/// @brief Navigator — stack-based page navigation.
///
/// Provides push/pop navigation between pages, similar to Flutter's Navigator.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/tree/widget.hpp"
#include "vaura/state/state.hpp"
#include <vector>
#include <functional>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// Route — a named page entry in the navigation stack
// ════════════════════════════════════════════════════════════════

struct Route {
    std::string name;
    WidgetPtr   page;
};

// ════════════════════════════════════════════════════════════════
// NavigatorState — manages the route stack
// ════════════════════════════════════════════════════════════════

class NavigatorWidget; // forward

class NavigatorState : public State {
public:
    /// Push a new page onto the navigation stack.
    void push(WidgetPtr page) {
        stack_.push_back({ "", std::move(page) });
        setState([]{});
    }

    /// Push a named route.
    void pushNamed(const std::string& name, WidgetPtr page) {
        stack_.push_back({ name, std::move(page) });
        setState([]{});
    }

    /// Pop the top page. Returns false if only one page remains.
    bool pop() {
        if (stack_.size() <= 1) return false;
        stack_.pop_back();
        setState([]{});
        return true;
    }

    /// Replace the top page with a new one.
    void pushReplacement(WidgetPtr page) {
        if (!stack_.empty()) stack_.pop_back();
        stack_.push_back({ "", std::move(page) });
        setState([]{});
    }

    /// Pop all routes and push a new one.
    void pushAndRemoveUntil(WidgetPtr page) {
        stack_.clear();
        stack_.push_back({ "", std::move(page) });
        setState([]{});
    }

    /// Check if we can pop (more than one page).
    [[nodiscard]] bool canPop() const { return stack_.size() > 1; }

    /// Number of pages in the stack.
    [[nodiscard]] size_t depth() const { return stack_.size(); }

    /// Get the current (top) route name.
    [[nodiscard]] const std::string& currentRouteName() const {
        static const std::string empty;
        return stack_.empty() ? empty : stack_.back().name;
    }

    WidgetPtr build(BuildContext& ctx) override;

private:
    std::vector<Route> stack_;
};

// ════════════════════════════════════════════════════════════════
// NavigatorWidget — the StatefulWidget wrapper
// ════════════════════════════════════════════════════════════════

class NavigatorWidget : public StatefulWidget {
public:
    WidgetPtr initial_page;

    explicit NavigatorWidget(WidgetPtr page)
        : initial_page(std::move(page)) {}

    [[nodiscard]] std::string_view typeName() const override {
        return "Navigator";
    }

    [[nodiscard]] std::unique_ptr<State> createState() override;
};

// ════════════════════════════════════════════════════════════════
// DSL helper
// ════════════════════════════════════════════════════════════════

/// @brief Create a Navigator with an initial page.
/// @code navigator(std::make_shared<HomePage>()) @endcode
inline WidgetPtr navigator(WidgetPtr initial_page) {
    return std::make_shared<NavigatorWidget>(std::move(initial_page));
}

} // namespace vaura
