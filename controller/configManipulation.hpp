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
#include <cstdlib>
#include <sstream>


/**************************************************************************************************************************
void showHeader(std::string oldValues, std::string currentTemps, std::string currentSpeeds)

    Function used to show the informational header and clear the screen for ease of reading

**************************************************************************************************************************/
void showHeader(std::string oldValues, std::string currentTemps, std::string currentSpeeds){
    std::system("clear");
    std::cout << oldValues << std::endl << std::endl;

    //check to see if we need to display current speeds
    if (currentTemps != ""){
        std::cout << "CURRENTLY ENTERED TEMPERATURES:" << currentTemps << std::endl; 
    }
    if (currentSpeeds != ""){
        std::cout << "CURRENTLY ENTERED FAN SPEEDS:" << currentSpeeds << std::endl; 
    }
    if (currentTemps != "" || currentSpeeds != ""){
        std::cout << std::endl;
    }
}

/**************************************************************************************************************************
void safeCin(int* target)

    Function used to streamline the flow of cin statements, particularly in the case where a non-numerical value could be
    entered. 

**************************************************************************************************************************/
void safeCin(int* target){

    //take in value into string
    std::string input = "";
    (std::cin >> input).get();

    //check if input can become an int
    try{
        *target = stoi(input);
    }
    catch(...){
        *target = -1;
    }

}

/**************************************************************************************************************************
void ReadFanService(std::string location, std::ifstream& ifs, std::string* returnString)

    Function used to print the information related to the fan control service and how it will adjust the fans
    when in use.

**************************************************************************************************************************/
void ReadFanService(std::string location, std::ifstream& ifs, std::string* returnString){

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
    
    *returnString += "EcTool Fan Control Service Current Settings: \n";
    *returnString += "=============================================\n";
    *returnString += "Current Recognized Temperature Range:\n";
    *returnString += lines["TEMPS"] + "\n";
    *returnString += "Current Fan Speed Range (Corresponds to temperatures above):\n";
    *returnString += lines["FAN_SPEEDS"] + "\n";
    *returnString += "Current Maximum Fan Speed:\n";
    *returnString += lines["MAX_FAN_SPEED"] + "\n";

    ifs.close();
}

/**************************************************************************************************************************
void setFanSpeeds(std::string location, std::ifstream& ifs)

    Function used to set the fan speeds and temperature limitations

**************************************************************************************************************************/
void setFanSpeeds(std::string location, std::ifstream& ifs){

    int range, i = 0, t = -1, p = -1, a = 0;
    char check = 'N';
    std::vector<int> temperatures;
    std::vector<int> fanSpeeds;
    std::string previouslyReadValues = "";
    std::string enteredTemperatures = "";
    std::string enteredFanSpeeds = "";

    //show old values
    std::cout << "Beginning Process to set new fan values:\n " << std::endl;
    std::cout << "Old Values:" << std::endl;
    ReadFanService(location, ifs, &previouslyReadValues);
    std::cout << previouslyReadValues << std::endl;
    std::cout << std::endl;

    //offer option to use list notation
    std::cout << "Would you like to enter your values in a [l]ist (40, 45, 50, 60, 75, 80) all at once, or [i]ndividually one by one? (l/i):" << std::endl;
    char option = 'i';
    (std::cin >> option).get();
    if (option != 'l'){

        showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
        std::cout << "\nEntering values individually\n" << std::endl;

        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
        std::cout << "RULES: " << std::endl;
        std::cout << "• The entered temperature thresholds must be in increasing order" << std::endl;
        std::cout << "• You must enter at least 3 thresholds" << std::endl;
        std::cout << "• The final value must be below 90, as 90 will be automatically set to 100% fan speed" << std::endl;
        std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;

        std::cout << "How many individual temperature thresholds would you like to set?" << std::endl;
        safeCin(&range);
        while(range < 3){
            showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
            std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "You must set at least 3 ranges for the fan speeds" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
            std::cout << "How many individual temperature thresholds would you like to set?" << std::endl;
            safeCin(&range);
        }

        //take in new values
        while(i < range){
            showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
            std::cout << "Enter a temperature to adjust the fan curve when reached:" << std::endl;
            safeCin(&a);
            while (a <= p){
                showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << "Next temperature in the set must be larger than the previous" << std::endl;
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
                std::cout << "Enter a temperature to adjust the fan curve when reached:" << std::endl;
                safeCin(&a);
            }
            p = a;
            temperatures.push_back(a);
            enteredTemperatures += " " + std::to_string(a);

            showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
            std::cout << "Enter a fan speed to set the fan curve to when provided temperature threshold is reached:" << std::endl;
            safeCin(&a);
            while (a <= t){
                showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                std::cout << "Next fanspeed in the set must be larger than the previous" << std::endl;
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl;
                std::cout << "Enter a fan speed to set the fan curve to when provided temperature threshold is reached:" << std::endl;
                safeCin(&a);
            }
            t = a;
            fanSpeeds.push_back(a);
            enteredFanSpeeds += " " + std::to_string(a);
            i++;
        }
    }
    else {

        bool safe = false;

        showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
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
            std::getline(std::cin, tempRange);

            std::stringstream inputTemperatureStream(tempRange);
            std::string temper = "";
            std::vector<std::string> localTemperatures;
            while(inputTemperatureStream >> temper) localTemperatures.push_back(temper);

            int previous = -1;
            int current = -1;
            for (std::vector<std::string>::iterator it = localTemperatures.begin(); it != localTemperatures.end(); ++it) {
                if (previous > 0){

                    //expect bad values
                    try{
                        current = stoi(*it);
                    }
                    catch(...){
                        safe = false;
                        break;
                    }

                    //check for increase, and safe upper bound
                    if (current <= previous || current >= 90){
                        safe = false;
                    }
                }
                
                //set previous
                try{
                    previous = stoi(*it);
                }
                catch(...){
                    safe = false;
                    break;
                }
            }

            //keep 3 setting limit
            (temperatures.size() < 3) ? false : safe;
            
            if (!safe){
                showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl; 
                std::cout << "Entered Numbers must be in increasing order, and must be in a safe range (below 90 degrees)" << std::endl; 
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl; 
            }

            if (safe){
                //set values to equal global vector
                for (std::vector<std::string>::iterator it = localTemperatures.begin(); it != localTemperatures.end(); ++it) {
                    temperatures.push_back(stoi(*it));
                    enteredTemperatures += " " + *it;
                }
            }
        }

        //reset blocking boolean
        safe = false;

        //take in fan speed list
        while (!safe){

            safe = true;

            showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "RULES: The entered fan speeds must be in increasing order" << std::endl;
            std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
            std::cout << "\nPlease enter a list of fan speeds (rpm) separated by whitespace that corresponds to the temperature theshold limits you entered above" << std::endl;
            std::cout << "Example: 2400 2600 3000 3500 4500 5000" << std::endl;
            std::string fanRange = "";
            std::getline(std::cin, fanRange);
            std::cout << fanRange;

            std::stringstream inputFanStream(fanRange);
            std::string speedFan = "";
            std::vector<std::string> checkFanSpeeds;
            while(inputFanStream >> speedFan) checkFanSpeeds.push_back(speedFan);

            int previous = -1;
            int current = -1;
            for (std::vector<std::string>::iterator it = checkFanSpeeds.begin(); it != checkFanSpeeds.end(); ++it) {
                if (previous > 0){

                    //expect bad values
                    try{
                        current = stoi(*it);
                    }
                    catch(...){
                        safe = false;
                        break;
                    }

                    //check for increase
                    if (current <= previous){
                            safe = false;
                        }
                }

                //set previous
                try{
                    previous = stoi(*it);
                }
                catch(...){
                    safe = false;
                    break;
                }
            }

            //check for same size
            //TODO 
            
            if (!safe){
                showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
                std::cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl; 
                std::cout << "Entered Numbers must be in increasing order, and must be below the maximum RPM of your cpu fan" << std::endl; 
                std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n" << std::endl; 
            }

            if (safe){
                //set values to equal global vector
                for (std::vector<std::string>::iterator it = checkFanSpeeds.begin(); it != checkFanSpeeds.end(); ++it) {
                    fanSpeeds.push_back(stoi(*it));
                    enteredFanSpeeds += " " + *it;
                }
            }
        }
    }

    //show entered values for confirmation
    showHeader(previouslyReadValues, enteredTemperatures, enteredFanSpeeds);
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
    (std::cin >> check).get();
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
