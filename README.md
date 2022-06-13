# DISCLAIMER
This program is in use on my machine (Acer 715-2W) with Ubuntu 22.04 running. The max fan speed may be innaccurate right now. Due to this, I do not recommend long term use at this moment, until I get enough free time to implement a max fan speed detection system

## Description
A fan controller which automates the installation of ectool and adds a simple service system and controller application for modification of fan curves and speeds.

## Overview of Parts
This program comes in 5 main parts: two services for starting ectool and setting fan speeds based on the temperature, and one timer for relaunching the service as needed. It also has a simple C++ program that allows for quick modification of the fan speeds based on the temperature reached, and it compiles ectool and installs it for use.

## Purpose
This program was made to run on devices running breath linux as the default coreboot firmware that comes with these devices typically seem to either be very aggressive with their fan curves, which leads to incredibly loud noises, or it simply doesn't run the fans fast enough to keep the cpu cool. This aims to make a simple way to run your fans and control their settings on breath devices. 

## Credit for parts
Google - Ectool and the coreboot firmware\n
Breath Linux Project - Script to install ectool quickly
<adding once I find the user again> - coming up with the service scripts for controlling the fan based on temperature.
