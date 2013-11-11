//=================================
// include guard
#ifndef __SCAAN_POINT_ANALYSIS_H_INCLUDED__
#define __SCAAN_POINT_ANALYSIS_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "user_interaction.h"
#include "sccan_analysis_point.h"
#include "sccan_point_pair_handler.h"
#include "reflector.h"
#include "ueye_camera.h"
// matplotpp
//#include "Python.h"

// plotting
#include <mgl2/mgl.h>
//#include <mgl2/window.h>

//=================================
class sccan_point_analysis 		:public user_interaction{
	public:
	bool 	sccan_point_analysis_verbosity;
	
	std::vector< std::vector<sccan_analysis_point*> > sccan_matrix;
	// dimension 1 sccan analysis points
	// dimension 2 mirrors
	
	//input
	sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;
	reflector				* pointer_to_reflector;
	
	//pointer_to_star_camera
	ueye_camera *pointer_to_star_camera;
//======================================================================
sccan_point_analysis(
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler,
reflector* new_pointer_to_reflector,
ueye_camera *new_pointer_to_star_camera);
//======================================================================
void fill_sccan_matrix();
//======================================================================
std::string get_sccan_matrix_prompt();
//======================================================================
void display_sccan_matrix_prompt();
//======================================================================
std::string get_analysis_prompt();
//======================================================================
void interaction();
//======================================================================
void toggle_verbosity();
//======================================================================
bool verbosity_status();
//======================================================================
//void draw_telescope_pointing_directions_of_sccan_runs();
//======================================================================
//void draw_all_mirror_light_flux_response_maps();
//======================================================================
//void draw_normalized_light_flux_response_map(uint mirror_itterator);
//======================================================================
private:
//std::string get_tilt_position_matrix_for_python();
//======================================================================
//std::string get_light_flux_matrix_for_python(uint mirror_itterator);
//======================================================================
double calc_standart_deviation_using_weights(
std::vector<double> x,std::vector<double> weights);
//======================================================================
double calc_mean_using_wights(
std::vector<double> x,std::vector<double> weights);
//======================================================================
void
calculate_pointing_direction_of_telescope_for_max_mirror_response();
//======================================================================
pointing_direction calc_poining_when_mirror_is_brightest(
uint mirror_iterator);
//======================================================================
void draw_mirror_response(uint mirror_iterator);
//======================================================================
uint get_number_of_mirrors();
//======================================================================
uint get_number_of_sccan_points();
};
#endif // __SCAAN_POINT_ANALYSIS_H_INCLUDED__
