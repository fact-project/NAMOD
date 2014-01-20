//=================================
// include guard
#ifndef __MAIN_MENU_H_INCLUDED__
#define __MAIN_MENU_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "user_interaction.h"
#include "snapshot.h"
#include "reflector.h"
#include "sccan_point_pair_handler.h"
#include "quick_align.h"
#include "sccan_point_analysis.h"
#include "verbosity_handler.h"
#include "ueye_camera.h"
#include "star_recognition_test_environment.h"

//=================================
class main_menu 				:public user_interaction{
	public:
	snapshot *pointer_to_snapshot;
	reflector *pointer_to_reflector;
	sccan_point_pair_handler *pointer_to_sccan_point_pair_handler;
	quick_align* pointer_to_quick_align;
	sccan_point_analysis *pointer_to_sccan_point_analysis;
	verbosity_handler *pointer_to_verbosity_handler;
	ueye_camera *pointer_to_star_camera;
	ueye_camera *pointer_to_reflector_camera;
	star_recognition_test_environment *pointer_to_test_environment;
//======================================================================
main_menu(
snapshot *new_pointer_to_snapshot,
reflector *new_pointer_to_reflector,
sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
quick_align *new_pointer_to_quick_align,
sccan_point_analysis *new_pointer_to_sccan_point_analysis,
verbosity_handler *new_pointer_to_verbosity_handler,
ueye_camera *new_pointer_to_star_camera,
ueye_camera *new_pointer_to_reflector_camera,
star_recognition_test_environment *new_pointer_to_test_environment);
//======================================================================
std::string get_main_menu_status_prompt();
//======================================================================
void interaction();
//======================================================================
};                                                                      
#endif // __MAIN_MENU_H_INCLUDED__
