# Repository Guidelines

## Project Structure & Module Organization
`src/` contains the Qt/C++ application code. Key areas: `graph/` (core model/render logic), `widgets/` (UI components, including `knot_view/`), `dialogs/` (Qt dialogs and `.ui` files), and `io/` (XML/image import-export).  
`data/` stores runtime assets (icons, styles, plugins).  
`cmake/` contains reusable CMake modules/macros.  
`docs/` contains Doxygen sources and generated HTML.  
`package/` contains packaging metadata (deb/rpm/windows/snap).

## Build, Test, and Development Commands
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug`: configure a local debug build (C++17, ASan flags enabled in Debug).
- `cmake --build build -j`: compile the `icsograf` binary and copy `data/` into `build/data`.
- `./build/bin/icsograf`: run the app from the build tree.
- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Release`: configure an optimized build.
- `./qt_format.sh`: format all `src/**/*.cpp|hpp` with `clang-format`.

## Coding Style & Naming Conventions
Use `.clang-format` (Google base, 4-space indent, 100-column limit, no tabs, Qt-friendly macros).  
Run formatting before opening a PR.  
Follow existing naming patterns:
- Types/classes: `Pascal_Case` (for example `Edge_Style`, `Main_Window`).
- Functions/methods: mixed style already exists; prefer consistency with the surrounding file.
- Member fields: `m_` prefix (for example `m_view`).
- Header guards: `ALL_CAPS_HPP`.

## Testing Guidelines
There is no committed unit-test suite yet (`add_test(...)` is not wired in current CMake files).  
For each change, include manual verification steps in your PR (for example: open app, create/edit/export a knot, reload saved file, check plugin dialog behavior).  
If you add tests, use CTest integration (`enable_testing()` + `add_test(...)`) so they run via `ctest --test-dir build -V`.

## Commit & Pull Request Guidelines
Current history uses short, informal commit messages. Prefer clearer, imperative subjects moving forward, e.g. `graph: fix edge slide bounds`.  
Keep commits focused and scoped by subsystem (`graph`, `dialogs`, `io`, `widgets`).  
PRs should include:
- What changed and why.
- Linked issue (if any).
- Manual test evidence.
- UI screenshots/GIFs for visible dialog/view changes.

## Testing rules

- Any bug fix in edge classes must include a CTest in tests/
- Prefer small focused tests with assert()
- After modifying C++ code, run:
  - cmake --build build -j
  - ctest --test-dir build -V
- Summarize the failing test before changing implementation
