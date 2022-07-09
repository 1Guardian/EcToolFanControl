#ifndef config
#define config

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

/**************************************************************************************************************************
void ReadFanService()

    Function used to print the information related to the fan control service and how it will adjust the fans
    when in use.

**************************************************************************************************************************/
void ReadFanService(std::string location, std::ifstream& ifs){

    ifs.open(location);

    //get relevant lines
    std::map<std::string, std::string> lines;
    std::string line;
    while (std::getline(ifs, line))
    {
        if (line.find("TEMPS") != std::string::npos) {
            lines["TEMPS"] = line;
        }
        if (line.find("FAN_SPEEDS") != std::string::npos) {
            lines["FAN_SPEEDS"] = line;
        }
        if (line.find("MAX_FAN_SPEED") != std::string::npos) {
            lines["MAX_FAN_SPEED"] = line;
        }

        if (lines.size() >= 3) break;
    }
    
    std::cout << "EcTool Fan Control Service Current Settings: " << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << "Current Recognized Temperature Range:" << std::endl;
    std::cout << lines["TEMPS"] << std::endl;
    std::cout << "Current Fan Speed Range (Corresponds to temperatures above):" << std::endl;
    std::cout << lines["FAN_SPEEDS"] << std::endl;
    std::cout << "Current Maximum Fan Speed:" << std::endl;
    std::cout << lines["MAX_FAN_SPEED"] << std::endl;

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

    //offer option to use list notation
    std::cout << "Would you like to enter your values in a [l]ist (40, 45, 50, 60, 75, 80) all at once, or [i]ndividually one by one? (l/i):" << std::endl;
    char option = 'i';
    std::cin >> option;
    if (option != 'l'){

        std::cout << "\nEntering values individually\n" << std::endl;

        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "RULES: " << std::endl;
        std::cout << "• The entered temperature thresholds must be in increasing order" << std::endl;
        std::cout << "• You must enter at least 3 thresholds" << std::endl;
        std::cout << "• The final value must be below 90, as 90 will be automatically set to 100% fan speed" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;

        std::cout << "How many individual temperature thresholds would you like to set?" << std::endl;
        std::cin >> range;
        while(range < 3){
            std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "You must set at least 3 ranges for the fan speeds" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
            std::cout << "How many individual temperature thresholds would you like to set?" << std::endl;
            std::cin >> range;
        }

        //take in new values
        while(i < range){
            std::cout << "Enter a temperature to adjust the fan curve when reached:" << std::endl;
            std::cin >> a;
            while (a <= p){
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << "Next temperature in the set must be larger than the previous" << std::endl;
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
                std::cout << "Enter a temperature to adjust the fan curve when reached:" << std::endl;
                std::cin >> a;
            }
            p = a;
            temperatures.push_back(a);

            std::cout << "Enter a fan speed to set the fan curve to when provided temperature threshold is reached:" << std::endl;
            std::cin >> a;
            while (a <= t){
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << "Next fanspeed in the set must be larger than the previous" << std::endl;
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
                std::cout << "Enter a fan speed to set the fan curve to when provided temperature threshold is reached:" << std::endl;
                std::cin >> a;
            }
            t = a;
            fanSpeeds.push_back(a);
            i++;
        }
    }
    else {

        bool safe = false;

        std::cout << "\nEntering values in list order.....\n" << std::endl;

        //take in temperature threshold list
        while (!safe){

            safe = true;
        
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "RULES: " << std::endl;
            std::cout << "• The entered temperature thresholds must be in increasing order" << std::endl;
            std::cout << "• You must enter at least 3 thresholds" << std::endl;
            std::cout << "• The final value must be below 90, as 90 will be automatically set to 100% fan speed" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;            
            std::cout << "\nPlease enter a list of temperatures separated by whitespace at which you would like your fans to increase in speed" << std::endl;
            std::cout << "Example: 30 40 50 60 70 80" << std::endl;
            std::string tempRange = "";
            std::cin.ignore();
            std::getline(std::cin, tempRange);

            std::stringstream inputTemperatureStream(tempRange);
            std::string temper = "";
            std::vector<std::string> localTemperatures;
            while(inputTemperatureStream >> temper) localTemperatures.push_back(temper);

            int previous = -1;
            for (std::vector<std::string>::iterator it = localTemperatures.begin(); it != localTemperatures.end(); ++it) {
                if (previous > 0){
                    if (stoi(*it) <= previous || stoi(*it) >= 90){
                        safe = false;
                    }
                }
                previous = stoi(*it);
            }

            //keep 3 setting limit
            (temperatures.size() < 3) ? false : safe;
            
            if (!safe){
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl; 
                std::cout << "Entered Numbers must be in increasing order, and must be in a safe range (below 90 degrees)" << std::endl; 
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl; 
            }

            if (safe){
                //set values to equal global vector
                for (std::vector<std::string>::iterator it = localTemperatures.begin(); it != localTemperatures.end(); ++it) {
                    temperatures.push_back(stoi(*it));
                }
            }
        }

        //reset blocking boolean
        safe = false;

        //take in fan speed list
        while (!safe){

            safe = true;
        
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "RULES: The entered fan speeds must be in increasing order" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "\nPlease enter a list of fan speeds (rpm) separated by whitespace that corresponds to the temperature theshold limits you entered above" << std::endl;
            std::cout << "Example: 2400 2600 3000 3500 4500 5000" << std::endl;
            std::string fanRange = "";
            std::cin.ignore();
            std::getline(std::cin, fanRange);

            std::stringstream inputFanStream(fanRange);
            std::string speedFan = "";
            std::vector<std::string> checkFanSpeeds;
            while(inputFanStream >> speedFan) checkFanSpeeds.push_back(speedFan);

            int previous = -1;
            for (std::vector<std::string>::iterator it = checkFanSpeeds.begin(); it != checkFanSpeeds.end(); ++it) {
                if (previous > 0){
                    if (stoi(*it) <= previous){
                        safe = false;
                    }
                }
                previous = stoi(*it);
            }

            //check for same size
            //TODO 
            
            if (!safe){
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl; 
                std::cout << "Entered Numbers must be in increasing order, and must be below the maximum RPM of your cpu fan" << std::endl; 
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl; 
            }

            if (safe){
                //set values to equal global vector
                for (std::vector<std::string>::iterator it = checkFanSpeeds.begin(); it != checkFanSpeeds.end(); ++it) {
                    fanSpeeds.push_back(stoi(*it));
                }
            }
        }

        exit(0);


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
            if (line.find("TEMPS") != std::string::npos) {
                writeBack += temps + "\n";
            }

            else if (line.find("FAN_SPEEDS") != std::string::npos){
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

#endif