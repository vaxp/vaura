project('vaura', 'cpp',
  version : '0.1.0',
  license : 'MIT',
  default_options : [
    'cpp_std=c++20',
    'warning_level=3',
    'werror=false',
    'optimization=2',
    'default_library=static',
  ],
  meson_version : '>= 1.1.0',
)

# ============================================================
# Compiler Configuration
# ============================================================
cpp = meson.get_compiler('cpp')

add_project_arguments(
  '-Wno-unused-parameter',
  '-Wno-missing-field-initializers',
  language : 'cpp',
)

# ============================================================
# Options
# ============================================================
build_tests    = get_option('build_tests')
build_examples = get_option('build_examples')

# ============================================================
# Dependencies
# ============================================================

# SDL: Try SDL3 first, fallback to SDL2
sdl_dep = dependency('sdl3', required : false)
if not sdl_dep.found()
  sdl_dep = dependency('SDL3', required : false)
endif
if sdl_dep.found()
  add_project_arguments('-DVAURA_HAS_SDL3=1', language : 'cpp')
else
  # Fallback to SDL2
  sdl_dep = dependency('sdl2', required : false)
  if not sdl_dep.found()
    sdl_dep = dependency('SDL2', required : false)
  endif
  if sdl_dep.found()
    add_project_arguments('-DVAURA_HAS_SDL2=1', language : 'cpp')
  endif
endif

# Anu layout engine (compiled directly)
anu_inc = include_directories('core')
anu_sources = files(
  'core/layout_engine/node/LayoutResults.cpp',
  'core/layout_engine/node/Node.cpp',
  'core/layout_engine/AnuValue.cpp',
  'core/layout_engine/AnuNode.cpp',
  'core/layout_engine/algorithm/PixelGrid.cpp',
  'core/layout_engine/algorithm/FlexLine.cpp',
  'core/layout_engine/algorithm/CalculateLayout.cpp',
  'core/layout_engine/algorithm/Cache.cpp',
  'core/layout_engine/algorithm/AbsoluteLayout.cpp',
  'core/layout_engine/algorithm/Baseline.cpp',
  'core/layout_engine/debug/Log.cpp',
  'core/layout_engine/debug/AssertFatal.cpp',
  'core/layout_engine/AnuNodeLayout.cpp',
  'core/layout_engine/AnuNodeStyle.cpp',
  'core/layout_engine/event/event.cpp',
  'core/layout_engine/AnuConfig.cpp',
  'core/layout_engine/AnuPixelGrid.cpp',
  'core/layout_engine/AnuEnums.cpp',
  'core/layout_engine/config/Config.cpp'
)

# Skia (prebuilt in core/skia)
skia_inc = include_directories('core/skia')
skia_dep = cpp.find_library('skia', dirs : [meson.current_source_dir() + '/core/skia/out/Release-x64'], required : false)
skparagraph_dep = cpp.find_library('skparagraph', dirs : [meson.current_source_dir() + '/core/skia/out/Release-x64'], required : false)
skshaper_dep = cpp.find_library('skshaper', dirs : [meson.current_source_dir() + '/core/skia/out/Release-x64'], required : false)
skunicode_dep = cpp.find_library('skunicode', dirs : [meson.current_source_dir() + '/core/skia/out/Release-x64'], required : false)

# Optional system Skia
if not skia_dep.found()
  skia_dep = dependency('skia', required : false)
  if not skia_dep.found()
    skia_dep = cpp.find_library('skia', required : false)
  endif
endif

# System libraries required by Skia
fontconfig_dep = dependency('fontconfig', required : false)
zlib_dep = dependency('zlib', required : false)
if not zlib_dep.found()
  zlib_dep = cpp.find_library('z', required : false)
endif

# OpenGL (for Skia backend)
gl_dep = dependency('gl', required : false)
egl_dep = dependency('egl', required : false)

# System deps
thread_dep = dependency('threads')
dl_dep = cpp.find_library('dl', required : false)
m_dep = cpp.find_library('m', required : false)

# ============================================================
# Collect all dependencies
# ============================================================
vaura_deps = [thread_dep]

if sdl_dep.found()
  vaura_deps += sdl_dep
endif
if skia_dep.found()
  vaura_deps += [skia_dep]
  if skparagraph_dep.found()
    vaura_deps += skparagraph_dep
  endif
  if skshaper_dep.found()
    vaura_deps += skshaper_dep
  endif
  if skunicode_dep.found()
    vaura_deps += skunicode_dep
  endif
  if fontconfig_dep.found()
    vaura_deps += fontconfig_dep
  endif
  if zlib_dep.found()
    vaura_deps += zlib_dep
  endif
  add_project_arguments('-DSK_GL', language : 'cpp')
endif
if gl_dep.found()
  vaura_deps += gl_dep
endif
if egl_dep.found()
  vaura_deps += egl_dep
endif
if dl_dep.found()
  vaura_deps += dl_dep
endif
if m_dep.found()
  vaura_deps += m_dep
endif

# ============================================================
# Feature detection — define macros based on what we found
# ============================================================
vaura_defines = ['-DVAURA_HAS_ANU=1']

if skia_dep.found()
  vaura_defines += '-DVAURA_HAS_SKIA=1'
endif

add_project_arguments(vaura_defines, language : 'cpp')

# ============================================================
# Include directories
# ============================================================
vaura_inc = include_directories('include', 'core/skia', 'core')

# ============================================================
# Source files
# ============================================================
vaura_sources = files(
  # Core
  'src/core/memory.cpp',
  'src/core/string_utils.cpp',

  # App
  'src/app/app.cpp',
  'src/app/theme.cpp',
  'src/app/navigator.cpp',

  # Platform
  'src/platform/platform_sdl3.cpp',
  'src/platform/window.cpp',
  'src/platform/popup_window.cpp',
  'src/platform/input.cpp',

  # Rendering
  'src/rendering/canvas.cpp',
  'src/rendering/paint.cpp',
  'src/rendering/color.cpp',
  'src/rendering/path.cpp',
  'src/rendering/image.cpp',
  'src/rendering/font_manager.cpp',

  # Tree
  'src/tree/widget.cpp',
  'src/tree/element.cpp',
  'src/tree/render_object.cpp',
  'src/tree/reconciler.cpp',
  'src/tree/build_context.cpp',
  'src/tree/key.cpp',

  # State
  'src/state/state.cpp',
  'src/state/cubit.cpp',
  'src/state/bloc.cpp',
  'src/state/stream.cpp',
  'src/state/bloc_provider.cpp',
  'src/state/bloc_builder.cpp',
  'src/state/bloc_listener.cpp',

  # Widgets
  'src/widgets/flexbox.cpp',
  'src/widgets/text.cpp',
  'src/widgets/button.cpp',
  'src/widgets/gesture_detector.cpp',
  'src/widgets/image_widget.cpp',
  'src/widgets/scroll_view.cpp',
  'src/widgets/slider.cpp',
  'src/widgets/linear_progress.cpp',
  'src/widgets/badge.cpp',
  'src/widgets/avatar.cpp',
  'src/widgets/tooltip.cpp',
  'src/widgets/container.cpp',
  'src/widgets/stack.cpp',
  'src/widgets/circular_progress.cpp',
  'src/widgets/opacity.cpp',
  'src/widgets/transform.cpp',
  'src/widgets/shader_progress.cpp',
  'src/widgets/shader_background.cpp',
  'src/widgets/shader_button.cpp',
  'src/widgets/switch.cpp',
  'src/widgets/text_field.cpp',
  'src/widgets/icon.cpp',
  'src/widgets/checkbox.cpp',
  'src/widgets/radio.cpp',
  'src/widgets/card.cpp',
  'src/widgets/divider.cpp',
  'src/widgets/list_tile.cpp',
  'src/widgets/accordion.cpp',
  'src/widgets/tabs.cpp',

  # ── Composite Widgets (Phase 1 — Overlays / Popups / Menus) ──
  'src/widgets/dropdown.cpp',
  'src/widgets/context_menu.cpp',
  'src/widgets/popup_menu.cpp',
  'src/widgets/bottom_sheet.cpp',

  # ── Overlay System ────────────────────────────────────────────
  'src/overlay/overlay.cpp',
  'src/overlay/popup_window_controller.cpp',


  # ── Composite Widgets (Phase 2 — Navigation) ─────────────────
  'src/widgets/drawer.cpp',
  'src/widgets/navigation_rail.cpp',
  'src/widgets/breadcrumb.cpp',
  'src/widgets/fab.cpp',
  'src/widgets/stepper.cpp',

  # ── Composite Widgets (Phase 3 — Pickers) ────────────────────
  'src/widgets/calendar.cpp',
  'src/widgets/date_picker.cpp',
  'src/widgets/time_picker.cpp',
  'src/widgets/color_picker.cpp',
  'src/widgets/search_bar.cpp',

  # ── Composite Widgets (Phase 4 — Data Display) ───────────────
  'src/widgets/data_table.cpp',
  'src/widgets/tree_view.cpp',
  'src/widgets/expansion_panel.cpp',

  # ── Composite Widgets (Phase 5 — Feedback) ───────────────────
  'src/widgets/snackbar.cpp',
  'src/widgets/toast.cpp',

  # ── Composite Widgets (Phase 6 — Advanced Layouts) ───────────
  'src/widgets/split_view.cpp',
  'src/widgets/resizable_panels.cpp',

  # Animation
  'src/animation/animation_controller.cpp',
  'src/animation/tween.cpp',
  'src/animation/curves.cpp',

  # App
  'src/app/app.cpp',
  'src/app/navigator.cpp',
  'src/app/theme.cpp',
)

anu_lib = static_library('anucore',
  anu_sources,
  include_directories: anu_inc,
  override_options: ['werror=false', 'warning_level=0']
)

anu_dep = declare_dependency(
  include_directories: anu_inc,
  link_with: anu_lib
)

vaura_deps += anu_dep

# ============================================================
# Library target
# ============================================================
vaura_lib = library('vaura',
  vaura_sources,
  include_directories : vaura_inc,
  dependencies : vaura_deps,
  install : true,
)

vaura_dep = declare_dependency(
  include_directories : vaura_inc,
  link_with : vaura_lib,
  dependencies : vaura_deps,
)

# ============================================================
# Tests
# ============================================================
if build_tests
  subdir('tests')
endif

# ============================================================
# Examples
# ============================================================
if build_examples
  subdir('examples')
  subdir('widgets_demo')
endif

# ============================================================
# Install headers
# ============================================================
install_subdir('include/vaura', install_dir : get_option('includedir'))
