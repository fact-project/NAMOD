//=================================
// include guard
#ifndef __SCCAN_POINT_PAIR_H_INCLUDED__
#define __SCCAN_POINT_PAIR_H_INCLUDED__

//=================================
// forward declared dependencies
//class ueye_camera;
//=================================
// included dependencies
#include "programm_verbosity.h"
#include <iostream>
//#include <stdlib.h>
#include <string>
#include <sstream>
//#include <math.h>
//#include <vector>
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

#include "ueye_camera.h"
#include "simple_image.h"
#include "sccan_image.h"
#include "sccan_analysis_point.h"


//=================================
class sccan_point_pair			:public programm_verbosity{
public:
bool 			star_detection_verbosity;
sccan_image 	reflector_image;
sccan_image 	star_image;
std::string			reflector_image_name_postfix;
std::string			star_image_name_postfix;
uint 			run_number;
std::string 			sccan_point_name_prefix;

// caemras
ueye_camera 	*pointer_to_star_camera;
ueye_camera 	*pointer_to_reflector_camera;

// star detection

star_in_image	sccan_star;
bool			single_star_has_been_found;

// configuration text file
std::string 			sccan_point_configuration_file_name;
std::string			file_name_extension_for_sccan_point_configuration_file;
std::string 			key_sccan_point;
std::string			key_sccan_point_name_prefix;
std::string 			key_run_startup_time;
std::string 			key_sccan_run_number;
std::string 			key_star_camera_ID;
std::string 			key_star_image_filename;
std::string 			key_star_exposure_time_in_ms;
std::string 			key_reflector_camera_ID;
std::string 			key_reflector_image_filename;
std::string 			key_reflector_exposure_time_in_ms;
std::string 		 	key_single_star_has_been_found;
std::string			key_star_position_in_pixels_x;
std::string			key_star_position_in_pixels_y;
std::string			key_light_flux_of_star_in_pixel_values_per_milli_second;
std::string			key_absolute_light_flux_in_standard_bulbs;
//======================================================================
sccan_point_pair(uint new_run_number,bool new_verbosity);
//======================================================================
void initialize(uint new_run_number,bool new_verbosity);
//======================================================================
void set_sccan_point_name_prefix(std::string new_file_name_prefix);
//======================================================================
void set_run_number(uint new_run_number);
//======================================================================
uint get_run_number();
//======================================================================
void create_sccan_point_name_prefix_using_current_timestamp();
//======================================================================
bool save_sccan_point_pair();
//======================================================================
bool load_sccan_point_pair(std::string sccan_point_name_prefix_to_load);
//======================================================================
bool load_sccan_point_configuration
(std::string name_of_sccan_point_pair_to_load);
//======================================================================
std::string get_sccan_point_pair_string();
//======================================================================
void disp();
//======================================================================
void set_cameras(ueye_camera* star_camera,ueye_camera* reflector_camera);
//======================================================================
bool acquire_sccan_images(
double *pointer_to_star_exposure_time_in_ms, 
double *pointer_to_reflector_exposure_time_in_ms,
int		max_number_of_exposure_itterations,
bool	*max_number_of_exposure_itterations_has_been_exceeded
);
//======================================================================
bool save_sccan_point_configuration();
//======================================================================
void find_bright_star_in_star_image
(double threshold_in_sigmas,
double radius_one_sigma_for_star_detection_in_degrees);
//======================================================================
void display_calibration_star_in_star_image(int time_to_display_in_ms);
//======================================================================
bool a_single_star_has_been_found();
//======================================================================
cv::Point get_point_of_star_in_star_image();
//======================================================================
simple_image get_resized_star_image_with_overlay
(int height_of_alignment_image);
//======================================================================
simple_image get_resized_reflector_image
(int height_of_alignment_image);
//======================================================================
void get_reflector_image_exposure_time_stamp
(simple_image* pointer_to_reflector_image_for_overlay);
//======================================================================
// analysis
//======================================================================
sccan_analysis_point* calculate_sccan_analysis_point(
list_of_pixel_positions* pointer_to_list_of_pixel_positions_of_specific_mirror,
uint mirror_ID
);
//======================================================================
};

#endif // __SCCAN_POINT_PAIR_H_INCLUDED__
