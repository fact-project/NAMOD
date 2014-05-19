//=================================
// include guard
#ifndef __STAR_INIMAGE_H_INCLUDED__
#define __STAR_INIMAGE_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>
#include "pointing_direction.h"
#include "programm_verbosity.h"
//=================================
class star_in_image				:public programm_verbosity{
public: 
	double light_flux_of_star_in_bulbs;
	pointing_direction star_direction_relative_to_camera;
	cv::Point position_in_pixels;

//======================================================================
star_in_image(double new_light_flux_of_star_in_bulbs,
pointing_direction new_star_direction_relative_to_camera,
cv::Point new_position_in_pixels);
//======================================================================
star_in_image();
//======================================================================
double get_light_flux_of_star_in_bulbs()const;
//======================================================================
pointing_direction get_star_direction_relative_to_camera()const;
//======================================================================
cv::Point get_position_in_pixels()const;
//======================================================================
void set_sccan_run_number(uint run_number);
//======================================================================
};
#endif // __STAR_INIMAGE_H_INCLUDED__ 
