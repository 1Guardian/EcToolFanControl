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
#include <regex.h>
#include <ctype.h>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <set>

/********************************************************************************
REPLACE LATER WITH REAL LOCATIONS FOR FILES
********************************************************************************/

// /usr/bin/chromebook-fan-control.sh
std::string chromebook_fan_control_script_location = "/usr/bin/chromebook-fan-control.sh";

// /etc/systemd/system/fan.service
std::string chromebook_fan_control_service_location = "/etc/systemd/system/fan.service";

// /etc/systemd/system/fan.timer
std::string chromebook_fan_control_timer_location = "/etc/systemd/system/fan.timer";

// /etc/systemd/system/root-resume.service
std::string chromebook_fan_control_resume_service_location = "/etc/systemd/system/root-resume.service";

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

/**************************************************************************************************************************
void locateFiles()

    Function used to print the location of all the service files and scripts

**************************************************************************************************************************/
void locateFiles(){
    
    std::cout << "EcTool Fan Control Service Configuration Tool: " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "fan.service: " << chromebook_fan_control_service_location << std::endl;
    std::cout << "fan.timer: " << chromebook_fan_control_timer_location << std::endl;
    std::cout << "root-resume.service: " << chromebook_fan_control_resume_service_location << std::endl;
    std::cout << "chromebook-fan-control.sh: " << chromebook_fan_control_script_location << std::endl;

    exit(0);
}

/**************************************************************************************************************************
void uninstall()

    Function used to remove all service files and uninstall the fan control service

**************************************************************************************************************************/
void uninstall(){
    
    std::cout << "Uninstalling......";
    remove(&chromebook_fan_control_service_location[0]);
    remove(&chromebook_fan_control_timer_location[0]);
    remove(&chromebook_fan_control_resume_service_location[0]);
    remove(&chromebook_fan_control_script_location[0]);
    remove("EcToolControl");
    std::cout << "Done!\n";

    exit(0);
}

/**************************************************************************************************************************
void ReadFanService()

    Function used to print the information related to the fan control service and how it will adjust the fans
    when in use.

**************************************************************************************************************************/
void ReadFanService(std::string location, std::ifstream& ifs){

    ifs.open(location);

    //get relevant lines
    std::vector<std::string> lines;
    std::string line;
    int i = 0;
    while (std::getline(ifs, line))
    {
        if (i == 2 || i == 3 || i == 4) {
            lines.push_back(line);
        }

        if (i == 4) break;

        i++;
    }
    
    std::cout << "EcTool Fan Control Service Current Settings: " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Current Recognized Temperature Range:" << std::endl;
    std::cout << lines[0] << std::endl;
    std::cout << "Current Fan Speed Range (Corresponds to temperatures above):" << std::endl;
    std::cout << lines[1] << std::endl;
    std::cout << "Current Maximum Fan Speed:" << std::endl;
    std::cout << lines[2] << std::endl;

    ifs.close();
}

/**************************************************************************************************************************
void setFanSpeeds()

    Function used to set the fan speeds and temperature limitations

**************************************************************************************************************************/
void setFanSpeeds(std::string location, std::ifstream& ifs){

    int range, i = 0, t = -1, p = -1, a = 0;
    char check = 'N';
    std::vector<int> temperatures;
    std::vector<int> fanSpeeds;

    //show old values
    std::cout << "Beginning Process to set new fan values:\n " << std::endl;
    std::cout << "Old Values:" << std::endl;
    ReadFanService(location, ifs);
    std::cout << std::endl;
    std::cout << "How many individual temperature thresholds would you like to set?" << std::endl;
    std::cin >> range;
    while(range < 3){
        std::cout << "You must set at least 3 ranges for the fan speeds" << std::endl;
        std::cout << "How many individual temperature thresholds would you like to set?" << std::endl;
        std::cin >> range;
    }

    //take in new values
    while(i < range){
        std::cout << "Enter a temperature to adjust the fan curve when reached:" << std::endl;
        std::cin >> a;
        while (a < p){
            std::cout << "Next temperature in the set must be larger than the previous" << std::endl;
            std::cout << "Enter a temperature to adjust the fan curve when reached:" << std::endl;
            std::cin >> a;
        }
        p = a;
        temperatures.push_back(a);

        std::cout << "Enter a fan speed to set the fan curve to when provided temperature threshold is reached:" << std::endl;
        std::cin >> a;
        while (a < t){
            std::cout << "Next fanspeed in the set must be larger than the previous" << std::endl;
            std::cout << "Enter a fan speed to set the fan curve to when provided temperature threshold is reached:" << std::endl;
            std::cin >> a;
        }
        t = a;
        fanSpeeds.push_back(a);
        i++;
    }

    //show entered values for confirmation
    std::cout << "\nYour new range of temperatures is: \n";
    std::string temps = "TEMPS=(";
    for(int i = 0; i < temperatures.size(); i++)
        temps += " " + std::to_string(temperatures[i]);
    temps += " )";
    std::cout << temps;

    std::cout << "\nYour new corresponding fan speeds are: \n";
    std::string speeds = "FAN_SPEEDS=(";
    for(int i = 0; i < fanSpeeds.size(); i++)
        speeds += " " + std::to_string(fanSpeeds[i]);
    speeds += " )";
    std::cout << speeds;

    std::cout << "\nIs this correct? (y/N): ";
    std::cin >> check;
    if(check == 'y' || check == 'Y'){

        //build new lines
        std::string line;
        std::string writeBack = "";
        int i = 0;
        ifs.open(location);
        while (std::getline(ifs, line))
        {
            if (i == 2) {
                writeBack += temps + "\n";
            }

            else if (i == 3){
                writeBack += speeds + "\n";
            }

            else{
                writeBack += line + "\n";
            }

            i++;
        }
        ifs.close();

        //write new values back to file
        std::fstream ofs;
        ofs.open(location, std::ios::out | std::ios::trunc);
        ofs << writeBack;
        ofs.close();

        std::cout << "Values Set" << std::endl;
    }
    else{
        std::cout << "Aborting and abandoning values." << std::endl;
    }

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
    while ((opt = getopt(argc, argv, ":hrsul")) != -1) {
        switch (opt) {

        case 'h':
            printHelp();
            break;

        case 'r':
            ReadFanService(chromebook_fan_control_script_location, ifs);
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
        }
    }

    return 0;
}