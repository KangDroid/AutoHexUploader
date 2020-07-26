#!/bin/bash

export to_upload=/home/pi/store

# Date Format: YMDHM, args: printer_name
function dispatch {
    # afer using use_example_configs
    cd Marlin
    make -j2 clean
    make -j2 ARDUINO_INSTALL_DIR=/home/pi/arduino-1.8.13 AVR_TOOLS_PATH=/home/pi/arduino-1.8.13/hardware/tools/avr/bin/ USE_CCACHE_FOR_BUILD=0
    mv applet/Marlin.hex /tmp/CompiledHex.hex
    # if [ -d "$to_upload/$@" ]
    # then
    #     mv applet/Marlin.hex $to_upload/$@/$@_$(date "+%Y%m%d%H%M").hex
    # else
    #     mkdir $to_upload/$@
    #     mv applet/Marlin.hex $to_upload/$@/$@_$(date "+%Y%m%d%H%M").hex
    # fi
    
    cd $script_root_directory/Marlin
    restore_configs
}

# Basic root directory for this script
export script_root_directory=/home/pi

cd $script_root_directory

export PATH=$PATH:$script_root_directory/Marlin/buildroot/bin

# First, clone Git-Repo || exists? then just fetch
if [ -d "$script_root_directory/Marlin" ]
then
    cd $script_root_directory/Marlin
    git fetch kangdroid
    git reset --hard kangdroid/experimental
else
    git clone https://github.com/KangDroid/Marlin -b experimental
fi

# cd to work directory
cd $script_root_directory/Marlin

if [ $@ == "CoreM" ]; then
    # Core M
    use_example_configs Corem
    dispatch "CoreM"
elif [ $@ == "SlideFast" ]; then
    # SlideFast
    use_example_configs SlideFast
    dispatch "SlideFast"
elif [ $@ == "CoreM_Multi" ]; then
    # Mieum - for now, just.
    use_example_configs Mieum
    dispatch "Mieum_HomePV"
elif [ $@ == "Lugo" ]; then
    # uhhhh...
    # Mieum - for now, just.
    use_example_configs Mieum
    dispatch "Mieum_HomePV"
else
    echo "what"
fi