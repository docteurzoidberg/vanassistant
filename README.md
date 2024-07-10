# Prerequisties

```bash
sudo apt update
sudo apt install build-essential libglu1-mesa-dev libpng-dev libx11-dev libpng-dev
```

/!\ Warning: emscripten sdk must be installed and in path for WASM build
/!\ Cross compilation toolchain + rootfs must be setup to build for the raspberrypi

see []() about how to setup emsdk
see []() about how to setup cross compilation for pizero

# Build & run

## for X11/wsl2 using PGE engine

 ```bash
cd ~/vanasssistant
mkdir build.x11
cd build.x11
cmake -DPLATFORM=X11 ..
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
cmake -DPLATFORM=WASM ..
make 
```

To run locally in the web browser, you can use provided python script to serve the wasm files:

```bash
cd ~/vanassistant
python3 wasm/serve.py
```

Then open [http://localhost:8000/build.wasm/vanassistant.html](http://localhost:8000/build.wasm/vanassistant.html)

## for the PI Zero with direct framebuffer output

```bash
cd ~/vansssistant
mkdir build.pi
cd build.pi
cmake -DPLATFORM=PI ..
make
```

# Testing sam lib locally

```bash
cd samlibs/SAM-master
make
./say Hello world
```