#include "star_in_image.h"
//======================================================================
star_in_image::star_in_image(double new_light_flux_of_star_in_bulbs,
pointing_direction new_star_direction_relative_to_camera,
cv::Point new_position_in_pixels){
	
	light_flux_of_star_in_bulbs = 
	new_light_flux_of_star_in_bulbs;
	
	star_direction_relative_to_camera = 
	new_star_direction_relative_to_camera;
	
	position_in_pixels = 
	new_position_in_pixels;
}
//======================================================================
star_in_image::star_in_image(){
	light_flux_of_star_in_bulbs = 0.0;
}
//======================================================================
double star_in_image::get_light_flux_of_star_in_bulbs()const{
	return light_flux_of_star_in_bulbs;
}
//======================================================================
pointing_direction 
star_in_image::get_star_direction_relative_to_camera()const{
	return star_direction_relative_to_camera;
}
//======================================================================
cv::Point star_in_image::get_position_in_pixels()const{
	return position_in_pixels;
}
//======================================================================
void star_in_image::set_sccan_run_number(uint run_number){
	star_direction_relative_to_camera.
	set_sccan_run_number_of_highest_response(run_number);
}
