#ifndef install
#define install

#include <iostream>
#include "directories.h"

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

#endif