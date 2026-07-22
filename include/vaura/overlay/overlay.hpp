#pragma once
/// @file overlay.hpp
/// @brief OverlayController — global overlay layer manager.
///
/// Overlays are full-screen layers that float above all other content.
/// They are the correct solution for:
///   - Dropdown menus
///   - Context menus
///   - Bottom sheets / modals
///   - Tooltips
///   - Snackbars
///
/// Usage:
///   auto id = OverlayController::instance().insert({ .content = myWidget });
///   // later:
///   OverlayController::instance().remove(id);
///
/// The App wraps the root widget in an OverlayStack which reads and
/// renders all active overlay entries above the normal content.
///
/// @copyright VAURA Framework — MIT License

#include "vaura/core/types.hpp"
#include "vaura/tree/widget.hpp"
#include <functional>
#include <vector>
#include <cstdint>

namespace vaura {

// ════════════════════════════════════════════════════════════════
// OverlayEntry — a single overlay layer
// ════════════════════════════════════════════════════════════════

using OverlayId = uint64_t;

/// @brief A single floating overlay entry.
///
/// Content is a widget rendered at the given position in screen space.
/// Use position = {0,0} and let the widget handle its own positioning
/// for full-screen overlays like Bottom Sheet backdrops.
struct OverlayEntry {
    WidgetPtr content;               ///< The widget to display as the overlay.
    bool      absorbs_pointer = true; ///< If true, blocks pointer events below.
};

// ════════════════════════════════════════════════════════════════
// OverlayController — global singleton
// ════════════════════════════════════════════════════════════════

/// @brief Global manager for floating overlay layers.
///
/// Widgets that need to float above all others (menus, sheets, tooltips)
/// register themselves here. The OverlayStack widget (which wraps the
/// entire app root) reads these entries and renders them in order.
class OverlayController {
public:
    /// Get the global singleton instance.
    static OverlayController& instance();

    // Non-copyable
    OverlayController(const OverlayController&) = delete;
    OverlayController& operator=(const OverlayController&) = delete;

    // ── Entry Management ────────────────────────────────────────

    /// Insert a new overlay entry. Returns its unique ID.
    OverlayId insert(OverlayEntry entry);

    /// Remove an overlay entry by ID.
    void remove(OverlayId id);

    /// Remove all overlay entries.
    void clear();

    /// @return true if there are active overlay entries.
    [[nodiscard]] bool hasEntries() const;

    /// @return the number of active overlay entries.
    [[nodiscard]] size_t entryCount() const;

    // ── Notification ────────────────────────────────────────────

    /// Set a callback invoked whenever the overlay list changes.
    /// The App uses this to trigger a rebuild of the OverlayStack.
    void setOnChanged(std::function<void()> callback);

    /// @return the list of active entries (in insertion order).
    [[nodiscard]] const std::vector<std::pair<OverlayId, OverlayEntry>>& entries() const;

    /// @return whether the overlay state changed since last frame.
    bool isDirty() const { return dirty_; }

    /// Clear the dirty flag (call after processing changes).
    void clearDirty() { dirty_ = false; }

private:
    OverlayController() = default;

    friend class OverlayStackState; // Allow OverlayStackState to read entries directly

    struct EntryRecord {
        OverlayId id;
        OverlayEntry entry;
    };

    std::vector<EntryRecord> entries_;
    OverlayId next_id_ = 1;
    bool dirty_ = false;
    std::function<void()> on_changed_;
};

// ════════════════════════════════════════════════════════════════
// OverlayStack — wraps app root, renders overlays on top
// ════════════════════════════════════════════════════════════════

/// @brief Convenience function: creates the OverlayStack wrapper.
///
/// Call in App::initWidgetTree to wrap the user's root widget.
/// The stack reads OverlayController::instance() each frame and
/// renders any active overlay entries above the main content.
///
/// @param root  The application's root widget.
/// @return A widget that renders root + all active overlays.
std::shared_ptr<Widget> overlay_stack(std::shared_ptr<Widget> root);

}  // namespace vaura
