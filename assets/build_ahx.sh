#!/bin/bash

# path
export MAIN_WORKING_DIR=/tmp/ahx

# clone this repo
git clone https://github.com/KangDroid/AutoHexUploader $MAIN_WORKING_DIR

cd $MAIN_WORKING_DIR
mkdir build
cd build
cmake ..
make -j12
make -j2 install
cd /
rm -rf $MAIN_WORKING_DIR