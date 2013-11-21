//=================================
// include guard
#ifndef __UEYE_CAMERA_H_INCLUDED__
#define __UEYE_CAMERA_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "programm_verbosity.h"
#include "intrinsic_camera_parameter.h"
#include "sccan_image.h"
#include <cmath>
//======================================================================
// UEYE camera by IDS imaging
// http://www.ids-imaging.de
// Manual:
// http://www.ids-imaging.de/frontend/files/uEyeManuals/Manual_ger/uEye_Handbuch/index.html
#include <ueye.h>
//======================================================================

//=================================
class ueye_camera 				:public programm_verbosity{
public:
	sccan_image latest_image;
	HIDS 		ueye_camera_handle;
	CAMINFO 	ueye_camera_info;
	SENSORINFO 	ueye_sensor_info;
	int 		ueye_camera_id;
	int 		ueye_camera_sensor_number_of_pixels_in_width;
	int 		ueye_camera_sensor_number_of_pixels_in_hight;
	int 		ueye_color_mode;
	int 		ueye_number_of_coulor_channels;
	int 		ueye_bits_per_coulor_channel;
	int			ueye_bits_per_pixel;
	double 		ueye_exposure_time_in_ms; 
	uint 		ueye_pixel_clock_min_in_MHz;
	uint 		ueye_pixel_clock_max_in_MHz;
	uint 		ueye_pixel_clock_increment_in_MHz;
	uint 		ueye_current_pixel_clock_in_MHz;
	uint		ueye_default_pixel_clock_in_MHz;
	double 		ueye_current_framerate_in_fps;
	double 		ueye_default_framerate_in_fps;
	bool 		initialization_succesfull;
	bool 		flag_long_time_exposure;
	std::stringstream out;
	
	//intrinsic_camera_parameter
	intrinsic_camera_parameter intrinsic;
//======================================================================
ueye_camera(int camera_ID_to_initialize,
intrinsic_camera_parameter new_intrinsic);
//======================================================================
void set_camera_ID(int camera_ID_to_initialize);
//======================================================================
uint get_camera_ID();
//======================================================================
~ueye_camera();
//======================================================================
bool initialize();
//======================================================================
bool acquire_image(double *pointer_to_desired_exposure_time_in_ms);
//======================================================================
bool acquire_image(double *pointer_to_desired_exposure_time_in_ms,
double desired_relative_maximal_camera_response);
//======================================================================
bool long_time_exposure(bool long_time_exposure);
//======================================================================
void display_camera_information();
//======================================================================
sccan_image get_latest_image();
//======================================================================
void export_latest_image(std::string filename_prefix);
//======================================================================
double get_current_exposure_time_in_ms();
//======================================================================
void disp_latest_image();
//======================================================================
bool camera_status();
//======================================================================
bool is_initialized();
//======================================================================
void toggle_verbosity();
//======================================================================
cv::Size get_sensor_size()const;
//======================================================================
};

#endif // __UEYE_CAMERA_H_INCLUDED__
