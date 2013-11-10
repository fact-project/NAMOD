//=================================
// include guard
#ifndef __QUICK_ALIGN_H_INCLUDED__
#define __QUICK_ALIGN_H_INCLUDED__

//=================================
// forward declared dependencies
//class sccan_point_pair_handler;
//=================================
// included dependencies
// open CV
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

#include "reflector.h"
#include "user_interaction.h"
#include "sccan_point_pair_handler.h"

//=================================
class quick_align 				:public user_interaction{
public:
reflector* pointer_to_reflector;
sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;

std::string window_name_of_alignment_image;
simple_image alignment_image;

int height_of_alignment_image;
//======================================================================
quick_align(
reflector* new_pointer_to_reflector,
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler);
//======================================================================
void start_alignment();
//======================================================================
void run_alignment();
//======================================================================
void display_help();
//======================================================================
void update_alignment_window();
//======================================================================
void acquire_sccan_point_for_quick_alignment();
//======================================================================
void update_alignment_window(sccan_point_pair current_sccan_point);
//======================================================================
std::string get_quick_alignment_prompt();
//======================================================================
void interaction();
//======================================================================
void toggle_verbosity();
//======================================================================
};

#endif // __QUICK_ALIGN_H_INCLUDED__
