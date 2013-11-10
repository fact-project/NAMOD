//=================================
// include guard
#ifndef __SCCAN_POINT_PAIR_HANDLER_H_INCLUDED__
#define __SCCAN_POINT_PAIR_HANDLER_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "user_interaction.h"
#include "sccan_point_pair.h"

//=================================
class sccan_point_pair_handler 	:public user_interaction{
	public:
	//std::vector<sccan_point_pair> list_of_sccan_point_pairs;
	
	double star_camera_last_good_exposure_time_in_ms;
	double reflector_camera_last_good_exposure_time_in_ms;
	
	ueye_camera 	*pointer_to_star_camera;
	ueye_camera 	*pointer_to_reflector_camera;
	bool			exposure_range_is_good;
	int 			sccan_run_number;
	//bool			verbosity_sccan_handler;

	double 	threshold_in_sigmas_for_star_detection;
	double 	star_recognition_one_sigma_radius_in_degrees;
	int		max_number_of_exposure_manipulations;
	
	// status
	simple_image 	star_position_indicator_image;
	simple_image	status_display;

	list_of_pixel_positions 	list_of_star_positions_of_all_points_in_directory;
	
	std::vector<std::string>	list_of_sccan_points_in_directory;
//======================================================================
sccan_point_pair_handler();
//======================================================================
void set_cameras(ueye_camera* star_camera,ueye_camera* reflector_camera);
//======================================================================
void acquire_sccan_points(int number_of_sccan_points_to_acquire);
//======================================================================
sccan_point_pair acquire_single_sccan_point(
bool *pointer_to_exposure_range_is_good);
//======================================================================
std::string get_prompt();
//======================================================================
void interaction();
//======================================================================
void update_list_of_runs_in_current_directory();
//======================================================================
void disp_list_of_sccan_points_in_current_directory();
//======================================================================
bool is_string_a_sccan_run_config(std::string line);
//======================================================================
void update_star_position_indicator_image();
//======================================================================
void display_star_position_indicator_image();
//======================================================================
void update_list_of_star_positions();
//======================================================================
void reset_exposure_times();
//======================================================================
void update_status_display
(sccan_point_pair *pointer_to_current_sccan_point);
//======================================================================
int get_number_of_sccan_points_in_current_directory();
//======================================================================
std::string get_sccan_point_pair_name_at(
int position_of_sccan_point_pair_name);
//======================================================================
void toggle_verbosity();
//======================================================================
private:
bool is_c(char c);
bool is_f(char c);
bool is_g(char c);
bool is_s(char c);
bool is_a(char c);
bool is_n(char c);
bool is_point(char c);
bool is_underscore(char c);
bool is_end_of_line(char c);
};

#endif // __SCCAN_POINT_PAIR_HANDLER_H_INCLUDED__
