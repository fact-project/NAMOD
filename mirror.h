//=================================
// include guard
#ifndef __MIRROR_H_INCLUDED__
#define __MIRROR_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "pointing_direction.h"
#include "programm_verbosity.h"
#include "simple_image.h"
#include "sccan_image.h"
#include "Vector3D.h"
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
	private:
	double tripod_orientation_z_in_rad;
	public:
	
	double tripod_radius_in_m;
	double pitch_of_bolt_in_m_per_revolution;
	
	bool 	flag_manipulation_distances_have_been_calculated;
	double manipulation_distance_of_first_tripod_leg_in_m;
	double manipulation_distance_of_second_tripod_leg_in_m;
	double manipulation_distance_of_third_tripod_leg_in_m;

	double manipulation_revolutions_of_first_tripod_leg_in_revs;
	double manipulation_revolutions_of_second_tripod_leg_in_revs;
	double manipulation_revolutions_of_third_tripod_leg_in_revs;

	Vector3D position_of_first_tripod_leg;
	Vector3D position_of_second_tripod_leg;
	Vector3D position_of_third_tripod_leg;	 

	// missalignment
	pointing_direction 	MirrorMisalignmentDirection;
	bool 				flag_misalignment_angles_have_been_calculated;
	
	// mask sccan_image
	bool 					flag_mask_has_been_initialized;
	std::vector<cv::Point> 	list_of_points_defining_mirror_polygon;
	sccan_image			image_with_mirror_mask;
	list_of_pixel_positions list_of_Points_inside_mirror_polygon;
	
	// configuration file
	std::string key_ID;
	std::string key_tripod_orientation_z_in_deg;
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
void draw_mirror_polygon(sccan_image *image_to_draw_poygons_in,bool highlight);
//======================================================================
void draw_mirror_tripod(sccan_image *image_to_draw_tripod_in,bool highlight);
//======================================================================
list_of_pixel_positions* get_pointer_to_list_of_Points_inside_mirror_polygon();
//======================================================================
sccan_image highlight_mirror_in_reflector_image
(sccan_image* pointer_to_reflector_image);
//======================================================================
std::string get_manipulation_instructions(pointing_direction
direction_of_star_relative_to_telescope_for_brighteset_mirror_response);
//======================================================================
void calculate_mirror_missalignment(pointing_direction
DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse);
//======================================================================
void calculate_bolt_manipulation_distances();
//======================================================================
void set_mirror_tripod(
double new_tripod_radius_in_m,
double new_pitch_of_mirror_bolt);
//======================================================================
void init_tripod();
//======================================================================
void calculate_bolt_manipulation_revolutions();
};
#endif // __MIRROR_H_INCLUDED__ 
