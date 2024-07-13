# Prerequisties

```bash
sudo apt update
sudo apt install build-essential libglu1-mesa-dev libpng-dev libx11-dev libpng-dev
```

/!\ Warning: emscripten sdk must be installed in path for WASM build and a toolchain file prepared.

/!\ Warning: Cross compilation toolchain + rootfs must be setup and a toolchain file is required to build for the raspios

/!\ Warning: buildroot env must be setup and a target already built to have the toolchain cmake file.

see [https://emscripten.org/docs/getting_started/index.html](https://emscripten.org/docs/getting_started/index.html) about how to setup emsdk

see [https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler-CMake-Usage-Guide-with-rsynced-Raspberry-Pi-32-bit-OS](https://github.com/abhiTronix/raspberry-pi-cross-compilers/wiki/Cross-Compiler-CMake-Usage-Guide-with-rsynced-Raspberry-Pi-32-bit-OS) about how to setup cross compilation for the pi zero

see [TODO] to setup the buildroot directory

# Build & run

## for X11/wsl2 using PGE engine

 ```bash
cd ~/vanasssistant
mkdir build.x11
cd build.x11
cmake .. -DPLATFORM=X11 -BACKEND=PGE
make
```

To run:

```bash
cd build.x11
./vanassistant.x11
```

## for the WEB with wasm, using PGE Engine

```bash
cd ~/vanasssistant
mkdir build.wasm
cd build.wasm
cmake .. -DPLATFORM=WASM -BACKEND=FB -CMAKE_TOOLCHAIN_FILE=/home/drzoid/cmake/toolchains/emscripten.cmake
make 
```

To run locally in the web browser, you can use provided python script to serve the wasm files:

```bash
cd ~/vanassistant
python3 wasm/serve.py
```

Then open [http://localhost:8000/build.wasm/vanassistant.html](http://localhost:8000/build.wasm/vanassistant.html)

## for the PI Zero on RASPIOS with direct framebuffer output

```bash
cd ~/vansssistant
mkdir build.pi
cd build.pi
cmake .. -DPLATFORM=PIOS -BACKEND=FB -CMAKE_TOOLCHAIN_FILE=/home/drzoid/cmake/toolchains/crosspigcc.cmake
make
```

## for the PI Zero on BUILDROOT with direct framebuffer output

```bash
cd ~/vansssistant
mkdir build.buildroot
cd build.buildroot
cmake .. -DPLATFORM=BUILDROOT -BACKEND=FB -CMAKE_TOOLCHAIN_FILE=/home/drzoid/rpi-zero-minimal-buildroot/build_workdir/host/share/buildroot/toolchainfile.cmake
make
```

# Testing sam lib locally

```bash
cd samlibs/SAM-master
make
./say Hello world
```