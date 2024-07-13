#!/bin/sh
cwd=$(pwd)
cd /home/drzoid/rpi-zero-minimal-buildroot/build_workdir
make -j4
cp /home/drzoid/rpi-zero-minimal-buildroot/build_workdir/images/sdcard.img $cwd/vanassistant.sdcard.img