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
cmake ..
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
cmake .. -DPLATFORM=WASM -CMAKE_TOOLCHAIN_FILE=/home/drzoid/cmake/toolchains/emscripten.cmake
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
cmake .. -DPLATFORM=PIOS -CMAKE_TOOLCHAIN_FILE=/home/drzoid/cmake/toolchains/crosspigcc.cmake
make
```

To copy the output binary to the raspberry pi zero W on raspbian over ssh (wifi enabled):

First time, make sure to adapt scripts/deploy_pi.sh to fit your networks and ssh and paths

```bash
cd ~/vansssistant
nano scripts/deploy_pi.sh
```

Then run a build again so the script is copied to build's output dir

```bash
cd build.pios
make
```
Then run the make_img.sh script from the build folder itself

```bash
./deploy_pi.sh
```

## for the PI Zero on BUILDROOT with direct framebuffer output

```bash
cd ~/vansssistant
mkdir build.buildroot
cd build.buildroot
cmake .. -DPLATFORM=BUILDROOT -CMAKE_TOOLCHAIN_FILE=/home/drzoid/rpi-zero-minimal-buildroot/build_workdir/host/share/buildroot/toolchainfile.cmake
make
```

To copy the output binary to buildroot's overlayfs and build the img file:

First time, make sure to adapt scripts/make_img.sh BEFORE your run the build to setup correct paths

```bash
cd ~/vansssistant
nano scripts/make_img.sh
```

Then run a build again so the script is copied to build's output dir

```bash
cd build.buildroot
make
```
Then run the make_img.sh script from the build folder itself

```bash
./make_img.sh
```

# Tinker with sam lib locally

```bash
cd samlibs/SAM-master
make
./say Hello world
```

# Generate new font headers for drzengine

using fontconvert online: #TODO
(using adafruit's gfx font format)

# Generate raw sprite headers for drzengine

```bash
python3 scripts/make_sprite_header.py filename.png output.h
```
