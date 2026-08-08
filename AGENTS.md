# Repository Guidelines

## Project Structure & Module Organization

`src/` contains the Qt/C++ application. Core graph models and rendering live in
`src/graph/`; interface code is in `src/widgets/` (notably `widgets/knot_view/`)
and `src/dialogs/`; import/export lives in `src/io/`. Database code is under
`src/database/`. Runtime icons, styles, and plugins belong in `data/`.
Reusable CMake helpers are in `cmake/`, packaging metadata in `package/`, and
Doxygen material in `docs/`. Focused CTest executables are in `tests/`.

## Build, Test, and Development Commands

Configure a Debug build (C++17 and AddressSanitizer enabled):

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
ctest --test-dir build -V
./build/bin/icsograf
```

Use `-DCMAKE_BUILD_TYPE=Release` for an optimized build. The build copies
`data/` into `build/data`; run the executable from the build tree so assets are
available. Format C++ sources before review with `./qt_format.sh`.

## Coding Style & Naming Conventions

Follow `.clang-format`: Google-derived C++ style, four-space indentation, no
tabs, and a 100-column limit. Function definitions use Allman braces. Use
existing naming patterns: classes/types use `Pascal_Case` (for example,
`Edge_Style`), member fields use `m_` (for example, `m_view`), and header guards
use `ALL_CAPS_HPP`. Match the surrounding method naming where styles differ.

## Testing Guidelines

Tests are CTest executables registered in `tests/CMakeLists.txt`, generally
named `test_<area>.cpp` and implemented with focused `assert()` checks. Add a
matching test when changing graph or edge behavior; edge fixes require a CTest
under `tests/`. Run the full build and `ctest --test-dir build -V` after C++
changes. For UI and export changes, also manually create/edit a knot, save and
reload it, and verify the affected dialog or exported file.

## Commit & Pull Request Guidelines

Recent history uses short informal messages; prefer clear imperative subjects
scoped to the subsystem, such as `graph: fix edge slide bounds`. Keep commits
focused. PRs should explain what changed and why, link the issue when relevant,
include build/test or manual verification evidence, and attach screenshots or a
GIF for visible UI changes.
