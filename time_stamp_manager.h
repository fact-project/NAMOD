//=================================
// include guard
#ifndef __TIME_STAMP_MANAGER_H_INCLUDED__
#define __TIME_STAMP_MANAGER_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>

#include <time.h>
#include "programm_verbosity.h"


//=================================
class time_stamp_manager 				:public programm_verbosity{
public:
time_t 	startup_time; 	
//======================================================================
time_stamp_manager();
//======================================================================
std::string get_startup_time_detail()const;
//======================================================================
std::string get_startup_time_stamp()const;
//======================================================================
std::string get_current_time_stamp()const;
//======================================================================
std::string get_time_stamp(time_t time_to_stamp)const;
//======================================================================
double get_number_of_seconds_since_startup()const;
};
#endif // __TIME_STAMP_MANAGER_H_INCLUDED__ 
