# Sapper

Classic minesweeper on C++ and SFML.

## Requirements

- CMake 3.24+
- Ninja
- Git
- C++17-capable compiler
- On Windows: Visual Studio with the `Desktop development with C++` workload if you want `build.bat` to bootstrap MSVC automatically

## Build

Windows:

```bat
build.bat
```

Bash:

```bash
./build.sh
```

If you start a script without a preset, it asks whether to build `release` or `debug`.

Examples:

- `build.bat release`
- `build.bat debug`
- `./build.sh release`
- `./build.sh debug`

The scripts download SFML `3.0.2` automatically if it is missing.

## Run

The executable is created in `build/<preset>/`.

Assets are copied next to the executable automatically.

## Controls

- Left mouse button: open a cell
- Right mouse button: place or remove a flag
- `1`: beginner, `9x9`, `10` mines
- `2`: intermediate, `16x16`, `40` mines
- `3`: expert, `16x30`, `99` mines
- `Esc`: exit the game
