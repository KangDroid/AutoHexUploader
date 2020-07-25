#!/bin/bash

cd /Users/kangdroid/Desktop/Marlin/Marlin
make -j12 clean
make -j16

mv applet/Marlin.hex /tmp/CompiledHex.hex