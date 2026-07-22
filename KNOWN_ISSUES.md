# Known Issues & Future Improvements

This document tracks known architectural limitations and widget bugs that require significant overhauls or fixes in the future.

## 1. Native Multi-Window Popups
Currently, overlays like `ContextMenu` and `Dropdown` are trapped inside the main application window because VAURA uses a single `SDL_Window` and a single Skia canvas per app. This is an architectural limitation (similar to early Flutter).
**Required Fix**:
- Enhance the `Platform` and `App` layers to support multi-window management.
- Allow the framework to request native, borderless, transparent `SDL_Window`s (e.g., `SDL_WINDOW_POPUP_MENU` or `SDL_WINDOW_TOOLTIP`) dynamically.
- Update `OverlayController` (or introduce a `NativeOverlayController`) so that popups draw onto these new floating windows, allowing them to escape the bounds of the main application window just like native Qt/GTK applications.
