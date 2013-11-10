//=================================
// include guard
#ifndef __SNAPSHOT_H_INCLUDED__
#define __SNAPSHOT_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "user_interaction.h"
#include "ueye_camera.h"
//=================================
class snapshot 					:public user_interaction{
public:
	bool 		snapshot_verbosity;
	uint 		snapshot_counter;
	bool 		flag_long_time_exposure;
	double 		snapshot_exposure_time_in_ms;
	std::string		snapshot_file_name;
	ueye_camera *pointer_to_snapshot_camera;
	std::vector<ueye_camera*> list_of_pointers_to_cameras;
//======================================================================
snapshot();
//======================================================================
snapshot(ueye_camera* ptr_to_snapshot_cam);
//======================================================================
void initialize();
//======================================================================
void add_camera(ueye_camera* ptr_to_snapshot_cam);
//======================================================================
void interaction();
//======================================================================
void create_snapshot_file_name();
//======================================================================
std::string get_snapshot_prompt();
//======================================================================
bool choose_camera(uint camera_ID_to_use);
//======================================================================
void toggle_verbosity();
//======================================================================
bool verbosity_status();
//======================================================================
void take_exposure_time_series(int number_of_exposure_itterations);
//======================================================================
void spectate_stars(double threshold_in_sigmas_for_star_detection,
double radius_in_deg_of_one_sigma_region_for_star_detection);
//======================================================================
};
#endif // __SNAPSHOT_H_INCLUDED__ 
