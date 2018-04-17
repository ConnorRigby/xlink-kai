#!/bin/bash
TARGETS="rpi rpi0 rpi2 rpi3"
rm -f *.img
for i in $TARGETS; do
    echo "Building for: $i"
    # MIX_TARGET=$i mix do deps.get, firmware, firmware.image
    MIX_TARGET=$i mix do deps.get
    # mv xlink.img xlink-$i-v$(cat VERSION).img
done
