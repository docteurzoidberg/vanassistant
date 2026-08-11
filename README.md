# Prerequisites

```bash
sudo apt update
sudo apt install build-essential cmake libglu1-mesa-dev libpng-dev libx11-dev
```

Per-target extras:

| Target                | Extra requirement                                                        |
| --------------------- | ------------------------------------------------------------------------ |
| `LINUX_X11`           | the packages above                                                        |
| `LINUX_FB`            | nothing beyond `build-essential` + `cmake`                                |
| `WASM`                | emsdk installed and activated (`source ~/sdk/emsdk/emsdk_env.sh`)         |
| `PI_X11` / `PI_FB`    | crosspigcc toolchain — `~/sdk/crosspigcc/toolchain.cmake`                 |
| `BUILDROOT`           | Buildroot env built at least once, for its toolchain file                 |
| `WIN`                 | mingw-w64 (`sudo apt install g++-mingw-w64-x86-64`)                       |

- emsdk setup: <https://emscripten.org/docs/getting_started/index.html>
- Pi cross-compilation setup:
  <https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler-CMake-Usage-Guide-with-rsynced-Raspberry-Pi-32-bit-OS>
- Buildroot setup: see `.claude/skills/vanassistant-buildroot/SKILL.md`

# Build & run

The `PLATFORM` option selects both the target and the rendering backend.
It defaults to `LINUX_X11` when omitted.

| `PLATFORM`  | Backend | Output binary              |
| ----------- | ------- | -------------------------- |
| `LINUX_X11` | PGE     | `vanassistant.linux_x11`   |
| `LINUX_FB`  | PGEFB   | `vanassistant.linux_fb`    |
| `PI_X11`    | PGE     | `vanassistant.pi_x11`      |
| `PI_FB`     | PGEFB   | `vanassistant.pi_fb`       |
| `BUILDROOT` | PGEFB   | `vanassistant.buildroot`   |
| `WIN`       | PGE     | `vanassistant.win.exe`     |
| `WASM`      | PGE     | `vanassistant.html` + `.js` + `.wasm` |

The binary is named `vanassistant.<platform lowercased>`; WASM is the
exception and produces the three web files instead.

## for X11/wsl2 using PGE engine

```bash
cd ~/dev/vanassistant
mkdir build.x11
cd build.x11
cmake .. -DPLATFORM=LINUX_X11
make
```

To run:

```bash
cd build.x11
./vanassistant.linux_x11
```

## for the Linux framebuffer

```bash
cd ~/dev/vanassistant
mkdir build.fb
cd build.fb
cmake .. -DPLATFORM=LINUX_FB
make
```

Run it from a real TTY (not from a terminal emulator inside X):

```bash
./vanassistant.linux_fb
```

## for the WEB with wasm, using PGE Engine

`emcmake` injects the emscripten toolchain file itself, so there is no
`-DCMAKE_TOOLCHAIN_FILE` to pass:

```bash
source ~/sdk/emsdk/emsdk_env.sh
cd ~/dev/vanassistant
mkdir build.wasm
cd build.wasm
emcmake cmake .. -DPLATFORM=WASM
make
```

This produces `vanassistant.html` (from the `wasm/vanassistant_shell.html`
template), plus `vanassistant.js` and `vanassistant.wasm`.

To run it locally, use the provided python server:

```bash
cd ~/dev/vanassistant
python3 wasm/wasmserve.py
```

Then open <http://localhost:8000/build.wasm/vanassistant.html>

**Do not** substitute `python3 -m http.server`, and do not open the page over
`file://`. The build is linked with `-pthread`, so it needs `SharedArrayBuffer`,
which the browser only enables when the page is cross-origin isolated —
`wasmserve.py` is there to send the required `Cross-Origin-Opener-Policy` and
`Cross-Origin-Embedder-Policy` headers, and to serve `.wasm` as
`application/wasm`.

There is also a wrapper that picks the build directory, starts the server
detached and prints the URLs (including what is needed to reach it from
another device on the LAN under WSL2):

```bash
.claude/scripts/va_wasm_serve.sh start     # then: stop | status | url | lan
```

It serves the build directory itself, so the URL is
<http://localhost:8000/vanassistant.html> in that case.

## for the PI Zero on RASPIOS with direct framebuffer output

```bash
cd ~/dev/vanassistant
mkdir build.pifb
cd build.pifb
cmake .. -DPLATFORM=PI_FB -DCMAKE_TOOLCHAIN_FILE=/home/drzoid/sdk/crosspigcc/toolchain.cmake
make
```

Use `-DPLATFORM=PI_X11` instead for the X11 output on the same hardware.

To push the binary to the Pi Zero W over ssh and restart it there, the build
provides a `deploy` target (it expects an ssh host named `pizerodev`, and runs
the app on `/dev/ttyAMA0` at 115200 bauds):

```bash
cd build.pifb
make deploy
```

## for the PI Zero on BUILDROOT with direct framebuffer output

```bash
cd ~/dev/vanassistant
mkdir build.buildroot
cd build.buildroot
cmake .. -DPLATFORM=BUILDROOT -DCMAKE_TOOLCHAIN_FILE=/home/drzoid/sdk/buildroot/output/host/share/buildroot/toolchainfile.cmake
make
```

The binary is copied into Buildroot's `vanassistant_overlay` as a post-build
step. To then regenerate the SD card image (`vanassistant.sdcard.img` in the
build directory):

```bash
cd build.buildroot
make image
```

The whole embedded distribution flow (kernel, boot logo, image, SD flashing)
is wrapped by `.claude/scripts/va_buildroot.sh` — see
`.claude/skills/vanassistant-buildroot/SKILL.md`.

## for Windows, cross-compiled with mingw-w64

```bash
cd ~/dev/vanassistant
mkdir build.win
cd build.win
cmake .. -DPLATFORM=WIN -DCMAKE_TOOLCHAIN_FILE=../.claude/scripts/toolchains/mingw64.cmake
make
```

# Checking every target builds

To compile all the targets whose toolchain is available on this machine, each
in its own throwaway `build.check.<target>/` directory:

```bash
.claude/scripts/va_build_all.sh list     # toolchain status per target
.claude/scripts/va_build_all.sh quick    # LINUX_X11 only, fast smoke test
.claude/scripts/va_build_all.sh all      # everything available + summary
```

See `.claude/skills/vanassistant-build-all/SKILL.md`.

# Tools

Helper programs live in `tools/` and build together:

```bash
cd tools
cmake -S . -B build
cmake --build build -j$(nproc)
```

Binaries land back in `tools/`: `png2h`, `obj2h`, `font2h`, `serialgui`,
`genproject`, `objviewer.linux_x11`.

# Generate raw sprite headers for drzengine

```bash
python3 tools/make_sprite_header.py filename.png output.h
```

# Generate new font headers for drzengine

using fontconvert online: #TODO
(using adafruit's gfx font format)

# Tinker with sam lib locally

```bash
cd samlibs/SAM-master
make
./say Hello world
```
