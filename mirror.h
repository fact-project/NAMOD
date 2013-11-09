//=================================
// include guard
#ifndef __MIRROR_H_INCLUDED__
#define __MIRROR_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "programm_verbosity.h"
#include "simple_image.h"

// include for configuration file
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

// open CV
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

//=================================
// the actual class
class mirror					:public programm_verbosity{
public:
	//bool 		verbosity_mirror;
	uint 		mirror_ID;
	
	// mirror mount orientation
	double tripod_orientation_z_in_rad;

	bool flag_manipulations_have_been_calculated;
	double manipulation_distance_of_first_tripod_leg_in_m;
	double manipulation_distance_of_second_tripod_leg_in_m;
	double manipulation_distance_of_third_tripod_leg_in_m;

	double manipulation_revolutions_of_first_tripod_leg_in_revs;
	double manipulation_revolutions_of_second_tripod_leg_in_revs;
	double manipulation_revolutions_of_third_tripod_leg_in_revs;

	// missalignment
	bool 	flag_misalignment_angles_have_been_calculated;
	double 	missalignment_angle_in_x_in_rad;
	double 	missalignment_angle_in_y_in_rad;
	
	// mask sccan_image
	bool 				flag_mask_has_been_initialized;
	std::vector<cv::Point> 	list_of_points_defining_mirror_polygon;
	simple_image		image_with_mirror_mask;
	list_of_pixel_positions 		list_of_Points_inside_mirror_polygon;
	
	// configuration file
	std::string 	key_ID;
	std::string 	key_tripod_orientation_z_in_rad;
	std::string	key_list_of_points_defining_mirror_polygon;
	std::string	key_polygon_point_pixel_coordinate_x;
	std::string	key_polygon_point_pixel_coordinate_y;	
//======================================================================
mirror(uint new_mirror_ID,bool new_verbosity);
//======================================================================
void set_mirror_ID(uint new_mirror_ID);
//======================================================================
uint get_mirror_ID();
//======================================================================
void initialize(uint new_mirror_ID,bool new_verbosity);
//======================================================================
std::string get_string();
//======================================================================
void disp();
//======================================================================
void export_configuration_setting(libconfig::Setting *Mirror_file_handle);
//======================================================================
bool import_configuration(libconfig::Setting *Mirror_file_handle);
//======================================================================
void initialize_list_of_points_inside_mirror_polygon
(cv::Size image_size_of_reflector_camera);
//======================================================================
void create_polygon_mask(cv::Size image_size_of_reflector_camera);
//======================================================================
void create_list_of_Points_inside_mirror_polygon();
//======================================================================
list_of_pixel_positions* get_pointer_to_list_of_Points_inside_mirror_polygon();
//======================================================================
simple_image highlight_mirror_in_reflector_image
(simple_image* pointer_to_reflector_image);
//======================================================================
};

#endif // __MIRROR_H_INCLUDED__ 