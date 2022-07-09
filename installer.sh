#!/bin/bash

# Rough script to install ectool to control fan speed
# as well as install a fan controller/configurator as well
# as some scripts and services to allow the user to control
# the fan speeds apart from coreboot.
# Ectool Build Portion of Script from: Breath Linux

#MAX FANSPEED SET
#sudo ectool fanduty 100 && sudo ectool pwmgetfanrpm | grep -o '[0-9]*' | awk '$1>0'

if [ "$1" == "install" ]
then

    set -xe
    echo "Downloading dependencies..."
    git clone https://chromium.googlesource.com/chromiumos/platform/ec --depth 1
    #sed -i -e 's/<libusb.h>/<libusb-1.0\/libusb.h>/g' /tmp/file.txt
    sudo apt update; sudo apt install git libftdi-dev libusb-dev libusb-1.0-0-dev libncurses5-dev pkgconf g++ lm-sensors -y

    echo "Compiling ectool..."
    cd ec
    echo "What board do you have? (You can find this out by finding your 'Board Name' in https://mrchromebox.tech/#devices)"
    echo
    read BOARD
    make BOARD=$BOARD CROSS_COMPILE= HOST_CROSS_COMPILE= build/$BOARD/util/ectool || echo "Compiling error or invalid board supplied."

    #Building ectool
    cp build/$BOARD/util/ectool /usr/bin/ectool
    cd ..

    #configure the maximum fan speed
    echo "Setting fan(s) to 100% to get maximum rpm approximation for configuration file"
    sudo ectool fanduty 100 
    sleep 1
    MAX_RPM=`sudo ectool pwmgetfanrpm | grep -o '[0-9]*' | awk '$1>0'`
    sed -i "5s/.*/MAX_FAN_SPEED=$MAX_RPM/" ./scripts/chromebook-fan-control.sh

    #building the configuration utility
    g++ ./controller/EcToolControl.cpp -o ./EcToolControl
    cp EcToolControl /usr/bin/EcToolControl

    #setting up the services and timers
    cp ./scripts/chromebook-fan-control.sh /usr/bin/chromebook-fan-control.sh
    chmod +x /usr/bin/chromebook-fan-control.sh
    cp ./services/fan.service /etc/systemd/system/fan.service
    cp ./services/fan.timer /etc/systemd/system/fan.timer
    cp ./services/root-resume.service /etc/systemd/system/root-resume.service

    systemctl start fan.timer && systemctl enable fan.timer
    systemctl enable root-resume.service

    echo "Ectool has been installed and the control utility along with the fan service has been set up! restart your computer now to start the service."

elif [ "$1" == "clean" ]
then

    rm EcToolControl
    rm -rf ./ec

elif [ "$1" == "build-controller" ]
then

    g++ -std=c++11 ./controller/EcToolControl.cpp ./controller/configManipulation.hpp ./controller/installManipulation.hpp ./controller/directories.h -o ./EcToolControl

elif [ "$1" == "build-ec" ]
then

    set -xe
    echo "Downloading dependencies..."
    git clone https://chromium.googlesource.com/chromiumos/platform/ec --depth 1
    #sed -i -e 's/<libusb.h>/<libusb-1.0\/libusb.h>/g' /tmp/file.txt
    sudo apt update; sudo apt install git libftdi-dev libusb-dev libusb-1.0-0-dev libncurses5-dev pkgconf g++ lm-sensors -y

    echo "Compiling ectool..."
    cd ec
    echo "What board do you have? (You can find this out by finding your 'Board Name' in https://mrchromebox.tech/#devices)"
    echo
    read BOARD
    make BOARD=$BOARD CROSS_COMPILE= HOST_CROSS_COMPILE= build/$BOARD/util/ectool || echo "Compiling error or invalid board supplied."

else

    echo "This script is used to compile the Ectool and the Ectool fan controller as well as setup all necessary scripts for usage...."
    echo "Options:"
    echo "install -> Builds and installs the Ectool Fan Controller"
    echo "build-controller -> Builds the Ectool Fan Controller"
    echo "build-ec -> Builds Ectool"
    echo "clean -> Cleans the compiled Ectool Fan Controller and cleans the Ectool build directory"

fi

