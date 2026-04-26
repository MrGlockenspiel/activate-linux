# Repository Guidelines

## Project Structure & Module Organization
Core code lives in `src/`. Platform backends are split by directory: `src/x11/`, `src/wayland/`, and `src/gdi/`. Shared logic (options, logging, i18n, color, config) is in top-level `src/*.c` and headers in `src/*.h`.

Generated protocol/event sources use `*.cgen` and `*.hgen` files under backend folders; build rules generate matching `.c`/`.h` during compilation. Static assets and desktop integration files are in `res/`. Build artifacts are written to `obj/` and the platform-specific binary in the repository root.

## Build, Test, and Development Commands
- `make`: Default local build (Linux/macOS; backend set by `backends`, default `wayland x11`).
- `make clean && make`: Clean rebuild, matching CI behavior.
- `make test`: Runs the built binary as a smoke test.
- `make install` / `make uninstall`: Install or remove binary and man page.
- `backends=gdi make`: Build Windows GDI backend (used in Windows CI).
- `xmake`: Alternative build path (experimental, see `xmake.lua`).

## Coding Style & Naming Conventions
Use C99 (`gnu99`) and keep warnings clean under `-Wall -Wextra -Wpedantic`. Formatting follows Linux style (`_astylerc`: `style=linux`) with 4-space indentation and braces on control blocks/functions.

Naming patterns:
- Files/modules: snake_case (`cairo_draw_text.c`, `activate_linux.c`)
- Functions/variables: snake_case
- Macros/feature flags: UPPER_CASE (`WAYLAND`, `X11`, `LIBCONFIG`)

## Testing Guidelines
This repository relies on build validation and runtime smoke checks. Before opening a PR, run:
1. `make clean && make`
2. `CC=clang make clean && make`
3. `make test` (on a supported display/session)

If you touch backend-specific code, test at least one affected backend explicitly (for example `backends=wayland make`).

## Commit & Pull Request Guidelines
Git history favors short, imperative commit subjects (for example: `Fix lang detection error`, `Add Korean translation`). Keep subject lines concise and scoped to one change.

PRs should include:
- What changed and why
- Platforms/backends tested (`x11`, `wayland`, or `gdi`)
- Build/test commands executed
- Screenshots for UI-visible changes (watermark appearance, positioning, font rendering)

## Security & Configuration Tips
Do not commit machine-specific configs or secrets. Use `example.cfg` as the baseline for optional `libconfig` setups, and keep local overrides untracked.
