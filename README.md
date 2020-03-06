# NanoboyAdvance libretro core

This project is a port of [NanoboyAdvance](https://github.com/fleroviux/NanoboyAdvance), a Game Boy Advance emulator
written by [fleroviux](https://github.com/fleroviux), to [libretro](https://www.libretro.com/), the heart of
[RetroArch](https://www.retroarch.com/).

This port pursues the goal of keeping a clean and unmodified core implementation.

## Building

### Update dependencies

This project includes [NanoboyAdvance](https://github.com/fleroviux/NanoboyAdvance) as a Git submodule, therefore you
should always execute the following command after pulling new commits from this repository:
```
git submodule update --init --recursive
```

### Compilation

#### Windows (UI based)

1) Open this project as a CMake project in Visual Studio 2019
2) Select `x64-Release` as the current build configuration.
3) In the `Solution Explorer`, select the `CMake Targets View`
4) Right click on `nba_libretro` and click on `Build`
5) Your compiled binary can be found at `out\build\x64-Release\nba_libretro.dll`

#### Windows (Command line)

`TODO`

#### Linux

Run the following commands:

```
mkdir cmake-build-release
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
make nba_libretro
```
