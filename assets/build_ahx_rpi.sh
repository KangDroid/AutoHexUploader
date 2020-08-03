#!/bin/bash

# path
export MAIN_WORKING_DIR=/tmp/ahx

# clone this repo
git clone https://github.com/KangDroid/AutoHexUploader $MAIN_WORKING_DIR

cd $MAIN_WORKING_DIR
mkdir build
cd build
cmake ..
make -j2
make -j2 install