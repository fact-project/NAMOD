//=================================
// include guard
#ifndef __SIMPLE_IMAGE_H_INCLUDED__
#define __SIMPLE_IMAGE_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "programm_verbosity.h"
#include "list_of_pixel_positions.h"
#include "star_in_image.h"

// open CV
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

//=================================
class simple_image				:public programm_verbosity{
public:	
	cv::Mat image_matrix;
	int 	image_format;
//======================================================================
simple_image();
//======================================================================
simple_image(bool new_verbosity);
//======================================================================
void disp_simple_image(std::string window_name);
//======================================================================
void disp_simple_image(std::string window_name,int time_to_display_in_ms);
//======================================================================
bool read_in_image(std::string file_name_of_image_to_load);
//======================================================================
bool save_simple_image(std::string file_name_of_image_to_save);
//======================================================================
int get_number_of_pixels();
//======================================================================
bool check_exposure_time
(	double lower_limit_for_brightest_pixel,
	double upper_limit_for_brightest_pixel,
	bool *expose_longer);
//======================================================================
void operator=(simple_image eq);
//======================================================================
// image processing 
//======================================================================
cv::Mat create_mean_image();
//======================================================================
cv::Mat create_median_image(double kernel_radius_one_sigma_in_pixels);
//======================================================================
double calculate_pixelvalue_standard_deviation();
//======================================================================
double calculate_variance(std::vector<double> *pointer_to_list_of_data);
//======================================================================
list_of_pixel_positions create_list_of_Points_of_non_zero_pixels();
//======================================================================
void get_brightest_pixel_of_image
(cv::Point *maximal_pixel_location,double *maximal_pixel_value);
//======================================================================
void get_relative_min_and_max_pixel_value(
double *min_relative_pixel_value,double *max_relative_pixel_value);
//======================================================================
simple_image get_overlay_with_stars(std::vector<star_in_image> list_of_stars,
cv::Scalar color_of_ring);
//======================================================================
simple_image get_overlay_with_stars
(list_of_pixel_positions list_of_star_positions,cv::Scalar color_of_ring);
//======================================================================
simple_image get_resized_image
(int height_of_alignment_image,double* scaling_factor_for_image);
//======================================================================
std::vector<list_of_pixel_positions> 
spatial_clustering_of_threshold_mask(bool show_controll_images);
//======================================================================
// image overlays
//======================================================================
void text_overlay(std::string text_to_overlay,
double relative_position_horizontal,
double relative_position_vertical,double relative_font_size);
//======================================================================
void text_overlay(std::string text_to_overlay,cv::Point text_position,
double relative_font_size);
//======================================================================
void horizontal_concatenate_with_delimiter_line(
simple_image* pointer_to_left_image,
simple_image* pointer_to_right_image);
//======================================================================
};
#endif // __SIMPLE_IMAGE_H_INCLUDED__ 
