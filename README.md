# Sapper

Old SFML-based minesweeper. This repo has been cleaned up and moved off the old Visual Studio project files to `CMake` + `Ninja`.

## Current status

- `CMake` is now the source of truth for project configuration.
- SFML is pinned to `3.0.2`, which is the latest official release at the time of this migration.
- The legacy in-repo SFML dump and old MSBuild artifacts are gone.
- The game code is ported to SFML 3 with minimal gameplay changes, so the original logic and quirks are mostly intact.

## Prerequisites

- `CMake 3.24+`
- `Ninja`
- `Git`
- A C++17-capable compiler
- On Windows, install Visual Studio with the `Desktop development with C++` workload if you want `build.bat` to auto-bootstrap MSVC for you.

## Build

Use one of the root build scripts:

- Windows batch: `build.bat`
- Bash: `./build.sh`

Both scripts do the same thing:

- if `.deps/SFML-3.0.2` is missing, they clone SFML automatically
- on Windows, `build.bat` also tries to load the Visual Studio C++ toolchain automatically if no compiler is already available in `PATH`
- on Bash, `build.sh` checks `CXX`, `c++`, `clang++`, and `g++` before it even starts CMake
- if you do not pass a build preset in an interactive terminal, both scripts ask you to choose `release` or `debug`
- then they run `cmake --preset ...`
- then they run `cmake --build --preset ...`
- in non-interactive shells, pass the preset explicitly

Examples:

- Batch with prompt: `build.bat`
- Batch release: `build.bat release`
- Batch debug: `build.bat debug`
- Bash with prompt: `./build.sh`
- Bash release: `./build.sh release`
- Bash debug: `./build.sh debug`

The presets use `Ninja` and place build trees under `build/`.

## Notes

- Assets are copied next to the executable by the CMake target after a build.
- The old Visual Studio solution and vendored SFML bundle were intentionally removed to keep the repo clean and reproducible.
- Official SFML release: <https://github.com/SFML/SFML/releases/tag/3.0.2>
