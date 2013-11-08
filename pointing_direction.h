//=================================
// include guard
#ifndef __POINTING_DIRECTION_H_INCLUDED__
#define __POINTING_DIRECTION_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <math.h>
#include "programm_verbosity.h"

//=================================
class pointing_direction 		:public programm_verbosity{
public:
	double 	direction_in_x_in_radiant;
	double 	direction_in_y_in_radiant;	
//======================================================================
pointing_direction();
//======================================================================
std::string get_x_tilt_prompt_in_deg_min_sec();
//======================================================================
std::string get_y_tilt_prompt_in_deg_min_sec();
//======================================================================
std::string get_x_tilt_prompt_in_deg();
//======================================================================
std::string get_y_tilt_prompt_in_deg();
//======================================================================
private:
std::string get_angle_text_prompt_in_deg_arc_min_arc_sec
(double angle_in_rad);
};
#endif // __POINTING_DIRECTION_H_INCLUDED__ 
