//=================================
// include guard
#ifndef __VERBOSITY_HANDLER_H_INCLUDED__
#define __VERBOSITY_HANDLER_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include "user_interaction.h"
#include "ueye_camera.h"
#include "reflector.h"
#include "snapshot.h"
#include "sccan_point_pair_handler.h"
#include "quick_align.h"
#include "sccan_point_analysis.h"
//~ #include ".h"
//=================================
class verbosity_handler 		:public user_interaction{
	//this class is for toggling programm verbosity during runtime
	time_stamp_manager 	*pointer_to_time_stamp_manager;
	ueye_camera		*pointer_to_star_camera;
	ueye_camera		*pointer_to_reflector_camera;
	reflector		*pointer_to_reflector;
	snapshot		*pointer_to_snapshot;
	sccan_point_pair_handler	*pointer_to_sccan_point_pair_handler;
	quick_align		*pointer_to_quick_align;
	sccan_point_analysis	*pointer_to_sccan_point_analysis;
public:
//======================================================================
verbosity_handler(
	time_stamp_manager 	*new_pointer_to_time_stamp_manager,
	ueye_camera		*new_pointer_to_star_camera,
	ueye_camera		*new_pointer_to_reflector_camera,
	reflector		*new_pointer_to_reflector,
	snapshot		*new_pointer_to_snapshot,
	sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
	quick_align		*new_pointer_to_quick_align,
	sccan_point_analysis	*new_pointer_to_sccan_point_analysis
);
//======================================================================
std::string get_verbosity_menu_prompt();
//======================================================================
void interaction();
//======================================================================
void toggle_all_verbosity(bool new_verbosity);
//======================================================================
};

#endif // __VERBOSITY_HANDLER_H_INCLUDED__
