//=================================
// include guard
#ifndef __SCAAN_ANALYSIS_POINT_H_INCLUDED__
#define __SCAAN_ANALYSIS_POINT_H_INCLUDED__

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include "pointing_direction.h"
#include "programm_verbosity.h"

//=================================
class sccan_analysis_point		:public programm_verbosity{
private:
	pointing_direction star_position_relative_to_pointing_direction;
	
	double 	normalized_light_flux;
	
	double star_light_flux_in_bulbs;
	double mirror_light_flux_in_bulbs;
	
	uint 	mirror_ID;

public:	
//======================================================================
sccan_analysis_point();
//======================================================================
std::string get_info_prompt();
//======================================================================
void set_mirror_ID(uint new_mirror_ID);
//======================================================================
uint get_mirror_ID();
//======================================================================
void set_light_flux(double new_star_light_flux_in_bulbs,
double new_mirror_light_flux_in_bulbs);
//======================================================================
double get_normalized_light_flux()const;
//======================================================================
double get_mirror_light_flux()const;
//======================================================================
double get_star_light_flux()const;
//======================================================================
void set_star_position_relative_to_pointing_direction
(pointing_direction new_star_position_relative_to_pointing_direction);
//======================================================================
pointing_direction get_star_position_relative_to_pointing_direction();
//======================================================================
void toggle_verbosity(bool new_verbosity);
//======================================================================
};
#endif // __SCAAN_ANALYSIS_POINT_H_INCLUDED__
