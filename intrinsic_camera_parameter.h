//=================================
// include guard
#ifndef __INTRINSIC_CAMERA_PARAMETER_H_INCLUDED__
#define __INTRINSIC_CAMERA_PARAMETER_H_INCLUDED__

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include <iostream>
//#include <stdlib.h>
#include <string>
#include <sstream>
//#include <math.h>
#include <vector>
//#include <list>
//#include <algorithm>

// include for configuration file
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

// open CV
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

#include "programm_verbosity.h"
#include "pointing_direction.h"
//=================================
class intrinsic_camera_parameter:public programm_verbosity{
	public:
	std::string name_of_camera;
	std::string name_of_optical_system;
	// camera_sensor_information
	cv::Size camera_sensor_size;
	
	double degrees_per_pixel;
	
	// radiometric parameter
	double radiometric_coefficient_for_log_exp_time;
	double radiometric_coefficient_for_log_camera_response;
	double radiometric_coefficient_constant;
	
	// config
	std::string filename_extension;
	std::string key_intrinsic_parameter;
	std::string key_name_of_camera;
	std::string key_name_of_optical_system;
	std::string key_sccan_point_name_prefix;
	std::string key_camera_sensor_width;
	std::string key_camera_sensor_height;
	std::string key_simple_pixel2angle;
	std::string key_radiometric_coefficient_for_log_exp_time;
	std::string key_radiometric_coefficient_for_log_camera_response;
	std::string key_radiometric_coefficient_constant;
//======================================================================
intrinsic_camera_parameter();
//======================================================================
void set_camera_sensor_size(int width,int height);
//======================================================================
void set_FoV_to_pixel_mapping(const double new_degrees_per_pixel);
//======================================================================
void set_names(std::string new_camera_name,
std::string new_name_of_optical_system);
//======================================================================
void set_coefficients_for_radiometric_correction_plane(	
double new_radiometric_coefficient_for_log_exp_time,
double new_radiometric_coefficient_for_log_camera_response,
double new_radiometric_coefficient_constant);
//======================================================================
std::string get_status_prompt();
//======================================================================
void disp();
//======================================================================
bool save_intrinsic_parameter(std::string sccan_point_name_prefix);
//======================================================================
bool load_intrinsic_camera_parameters(std::string sccan_point_name_prefix);
//======================================================================
pointing_direction calculate_pointing_direction(cv::Point position);
//======================================================================
double calculate_absolute_light_flux(
double exposure_time_in_ms,
double normalized_pixel_value);
//======================================================================
};
#endif // __INTRINSIC_CAMERA_PARAMETER_H_INCLUDED__
