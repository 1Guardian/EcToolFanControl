/******************************************************************************

Simple Front-End Editor for the EcToolFanControl Service. 

Invokation: ./EcToolControl <options>

options:   
        -r : Reads out the current fan speed settings
        -s : Sets new fan speed and timing settings
        -u : uninstall the EcToolFanControl Service
        -l : locate the service and it's files
        -h : help

*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <regex.h>
#include <ctype.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <set>
#include <sstream>
#include "configManipulation.hpp"
#include "installManipulation.hpp"

/**************************************************************************************************************************
void printHelp()

    Function used to print the general help message displayed on incorrect invokation or on request of help message

**************************************************************************************************************************/
void printHelp(bool wrongInvokation = false){
    
    if (wrongInvokation)
        std::cout << "Incorrect usage of command. Only one switch may be used at a time." << std::endl;

    std::cout << "EcTool Fan Control Service Configuration Tool: " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Execution Method: ./EcToolControl <switches> " << std::endl;
    std::cout << "Switch Options:" << std::endl;
    std::cout << "[-h] :- Displays this 'help' message" << std::endl;
    std::cout << "[-r] :- Reads out the current fan speed settings" << std::endl;
    std::cout << "[-s] :- Sets new fan speed and timing settings" << std::endl;
    std::cout << "[-l] :- Locates the Service and it's supporting files for manual editing" << std::endl;
    std::cout << "[-u] :- uninstall the EcToolFanControl Service" << std::endl;

    exit(0);
}

int main(int argc, char *argv[])
{   
    //string for taking input and stdin check boolean
    bool checkAtty = true;
    std::string execPrefix = "out";
    char choice = 'N';

    //variables connected to using files
    std::fstream tmpFile;
    //tmpFile.open("tmpFile.txt", std::ios::out | std::ios::app);
    std::string input = "";
    int line = 0;
    std::ifstream ifs;

    //Check for too many switches
    if(argc > 2){
        printHelp(true);
    }

    //Handle Command Line Switches
    int opt;
    std::string readValues = "";
    while ((opt = getopt(argc, argv, ":hrsul")) != -2) {
        switch (opt) {

        case 'h':
            printHelp();
            break;

        case 'r':
            ReadFanService(chromebook_fan_control_script_location, ifs, &readValues);
            std::cout << readValues;
            exit(0);

        case 's':
            setFanSpeeds(chromebook_fan_control_script_location, ifs);
            break;

        case 'u':
            std::cout << "Are You sure you want to uninstall the ectool fan control program and service? (N/y): ";
            std::cin >> choice;
            if(choice == 'y' || choice == 'Y')
                uninstall();
            else
                exit(0);
            break;

        case 'l':
            locateFiles();
            break;

        case '?':
            std::cout << "Invalid switch supplied. Terminating." << std::endl;
            exit(0);
            break;

        default:
            printHelp();
            break;
        }
    }

    return 0;
}
