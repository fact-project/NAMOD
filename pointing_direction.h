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
	
	uint		number_of_sccan_point_run_for_brightest_response;
	
	pointing_direction();
	pointing_direction(const double dir_x_in_rad,const double dir_y_in_rad);
	pointing_direction(const double dir_x_in_rad,const double dir_y_in_rad,uint sccan_run_number);
	std::string get_x_tilt_prompt_in_deg_min_sec()const;
	std::string get_y_tilt_prompt_in_deg_min_sec()const;
	std::string get_x_tilt_prompt_in_deg()const;
	std::string get_y_tilt_prompt_in_deg()const;
	std::string get_print()const;
	double get_x_tilt_in_deg()const;
	double get_y_tilt_in_deg()const;
	uint get_sccan_run_number_of_highest_response()const;
	void set_sccan_run_number_of_highest_response(uint sccan_run_number);
	pointing_direction operator*(const double ScaleFactor)const;
	pointing_direction operator+(const pointing_direction dir_to_add) const;
	pointing_direction operator/(const double ScaleFactor)const;
	pointing_direction operator-(const pointing_direction dir_to_sub) const;
private:
	std::string get_angle_text_prompt_in_deg_arc_min_arc_sec(double angle_in_rad)const;
};
#endif // __POINTING_DIRECTION_H_INCLUDED__ 
