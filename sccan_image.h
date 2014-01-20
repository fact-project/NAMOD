//=================================
// include guard
#ifndef __MYCLASS_H_INCLUDED__
#define __MYCLASS_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "global_variables.h"
#include "simple_image.h"
#include "intrinsic_camera_parameter.h"

//=================================
class sccan_image 				:public simple_image{
public:

	bool	image_initialized;
	double 	exposure_time_in_ms;

	std::string  image_name;
	std::string	file_name_extension_for_image;
	std::string	file_name_extension_for_info;
	
	uint	ID_of_ueye_camera_taken_this_image;
	
	double 	time_since_startup_in_seconds;
	// intrinsic parameters of camera and optics
	intrinsic_camera_parameter intrinsic_parameters;
	
	// configuration text file
	std::string name_of_image_info_block;
	std::string name_of_image_name_info;
	std::string name_of_image_file_name_info;
	std::string name_of_exposure_time_in_ms_info;
	std::string name_of_camera_ID_info;
	std::string name_of_time_since_startup_in_seconds;
//======================================================================
sccan_image();
//======================================================================
sccan_image(bool new_verbosity);
//======================================================================
void initialize(bool new_verbosity);
//======================================================================
void set_name(std::string new_image_name);
//======================================================================
void set_intrinsic_parameters(
intrinsic_camera_parameter new_intrinsic_parameters);
//======================================================================
std::string get_file_name();
//======================================================================
uint get_camera_ID_of_camera_taken_this_image();
//======================================================================
std::string get_name();
//======================================================================
bool export_image_to_disk();
//======================================================================
bool write_image_info(std::string image_configuration_file_name);
//======================================================================
double get_exposure_time_in_ms();
//======================================================================
void set_exposure_time_in_ms(double new_exposure_time_in_ms);
//======================================================================
void initialize_image_matrix(int rows, int cols);
//======================================================================
bool load_image(std::string name_of_image_to_load);
//======================================================================
void set_camera_ID(uint ueye_camera_ID);
//======================================================================
void disp_image();
//======================================================================
bool read_in_image_info(std::string file_name_of_image_info_to_load);
//======================================================================
std::string get_info_string();
//======================================================================
void disp();
//======================================================================
bool get_init_status();
//======================================================================
void set_time_stamp_in_seconds_since_startup();
//======================================================================
void operator=(sccan_image eq);
//======================================================================
void operator=(simple_image eq);
//======================================================================
std::vector<star_in_image> find_stars(double threshold_in_sigmas,
double radius_of_one_sigma_region_for_star_detection_in_degrees,
bool show_control_images);
//======================================================================
double mean_light_flux_in_bulbs_of_complex_image_region(
list_of_pixel_positions *pointer_to_pixels_of_complex_region);
//======================================================================
};
#endif // __MYCLASS_H_INCLUDED__ 
