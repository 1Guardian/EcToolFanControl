#ifndef directories
#define directories

#include <string>

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

#endif