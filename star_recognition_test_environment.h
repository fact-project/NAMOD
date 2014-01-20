//=================================
// include guard
#ifndef __STAR_RECOGNITION_TEST_ENVIRONMENT_H_INCLUDED__
#define __STAR_RECOGNITION_TEST_ENVIRONMENT_H_INCLUDED__

//=================================
// forward declared dependencies
//class sccan_point_pair_handler;
//=================================
// included dependencies
// open CV

#include "reflector.h"
#include "user_interaction.h"
#include "simple_image.h"
#include "sccan_image.h"
#include "intrinsic_camera_parameter.h"

//=================================
class star_recognition_test_environment:public user_interaction{
private:
	bool image_has_been_loaded;
	std::string	name_of_image;
	simple_image image_to_be_tested;
	
	intrinsic_camera_parameter parameters_of_image_to_be_tested;
	double test_mapping_in_degrees_per_pixel;
	double test_exposure_time_in_ms;
	
	double test_threshold_in_sigmas;
	double test_radius_of_one_sigma_region_for_star_detection_in_degrees;
	
public:
star_recognition_test_environment();
//=================================
void interaction();
std::string get_star_recognition_test_prompt();
//=================================
void test_star_recognition();
void load_image(std::string filename_of_image_to_be_tested);
void set_test_intrinsic_parameters();
void show_image();
};

#endif // __STAR_RECOGNITION_TEST_ENVIRONMENT_H_INCLUDED__
