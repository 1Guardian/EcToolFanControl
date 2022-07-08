#!/bin/bash

# Rough script to install ectool to control fan speed
# as well as install a fan controller/configurator as well
# as some scripts and services to allow the user to control
# the fan speeds apart from coreboot.
# Ectool Build Portion of Script from: Breath Linux

if [ "$1" == "build" ]
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

else
    echo "This script is used to compile the Ectool and the Ectool fan controller as well as setup all necessary scripts for usage...."
    echo "Options:"
    echo "build -> Builds and installs the Ectool Fan Controller"
    echo "clean -> Cleans the compiled Ectool Fan Controller Installer and cleans the Ectool build directory"
fi

