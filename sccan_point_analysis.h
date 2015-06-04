//=================================
// include guard
#ifndef __SCAAN_POINT_ANALYSIS_H_INCLUDED__
#define __SCAAN_POINT_ANALYSIS_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include <algorithm>
#include "user_interaction.h"
#include "sccan_analysis_point.h"
#include "sccan_point_pair_handler.h"
#include "reflector.h"
#include "pointing_direction.h"
// parallel computing
#include <omp.h>

// plotting
#include <mgl2/mgl.h>

//------------------------------------------------------------------------------
static bool compare_normalized_flux(
	const sccan_analysis_point *a, const sccan_analysis_point *b
) {
	return a->get_normalized_light_flux() <	b->get_normalized_light_flux();		
}
//------------------------------------------------------------------------------
static bool compare_only_mirror_flux(
	const sccan_analysis_point *a, const sccan_analysis_point *b
) {
	return a->get_mirror_light_flux() < b->get_mirror_light_flux();	
}
//------------------------------------------------------------------------------

//======================================================================
class sccan_point_analysis 		:public user_interaction{
private:
	bool 	sccan_point_analysis_verbosity;
	
	std::vector< std::vector<sccan_analysis_point*> > sccan_matrix;
	// dimension 1 sccan analysis points
	// dimension 2 mirrors

	pointing_direction global_offset;
	bool substract_global_offset = false;
	
	bool normalize_mirror_response;
	bool max_only_instead_of_CoG;
	
	//input
	sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;
	reflector				* pointer_to_reflector;
	
	// multi thread
	bool use_multithread;
public:	
	sccan_point_analysis(
		sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler,
		reflector* new_pointer_to_reflector
	);
	void fill_sccan_matrix();
	std::string get_sccan_matrix_prompt();
	void interaction();
	void toggle_verbosity();
	bool verbosity_status();
private:
	void calculate_global_offset();
	void display_sccan_matrix_prompt();
	std::string get_analysis_prompt();
	pointing_direction PointingDirectionOfStarAtMaxMirrorResponse(uint mirror_iterator);
	pointing_direction CentreOfGravityPointingDirectionOfMirrorResponse(uint mirror_iterator);
	void draw_mirror_response(uint mirror_iterator);
	uint get_number_of_mirrors();
	uint get_number_of_sccan_points();
	void run_anaysis();
	void analyse_single_mirror_and_get_instructions(
		uint MIit, 
		std::stringstream &instruction_table
	);
	void fill_sccan_point(int sccan_point_itterator);
};
#endif // __SCAAN_POINT_ANALYSIS_H_INCLUDED__
