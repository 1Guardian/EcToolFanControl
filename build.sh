#!/bin/bash

# Rough script to install ectool to control fan speed
# Script from: Breath Linux

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
g++ EcToolControl.cpp -o EcToolControl
cp EcToolControl /usr/bin/EcToolControl

#setting up the services and timers
cp chromebook-fan-control.sh /usr/bin/chromebook-fan-control.sh
chmod +x /usr/bin/chromebook-fan-control.sh
cp fan.service /etc/systemd/system/fan.service
cp fan.timer /etc/systemd/system/fan.timer
cp root-resume.service /etc/systemd/system/root-resume.service

systemctl start fan.timer && systemctl enable fan.timer
systemctl enable root-resume.service

echo "Ectool has been installed and the control utility along with the fan service has been set up! restart your computer now to start the service."
