#include "sccan_analysis_point.h"
//======================================================================
sccan_analysis_point::sccan_analysis_point(){
	verbosity = false;
	mirror_ID = 0;
	normalized_light_flux = 0.0;
}
//======================================================================
std::string sccan_analysis_point::get_info_prompt(){
	std::stringstream out;
	out<<" _sccan_analysis_point_mirror_ID_"<<mirror_ID;
	out<<"________"<<std::endl;
	out<<"| telescope tilt in x....: ";
	out<<star_position_relative_to_pointing_direction.
	get_x_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| telescope tilt in y....: ";
	out<<star_position_relative_to_pointing_direction.
	get_y_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| normalized light flux..: "<<normalized_light_flux;
	out<<" [mirror flux / star flux]";
	out<<std::endl;
	out<<"|_____________________________________________________"<<std::endl;
	return out.str();
}
//======================================================================
void sccan_analysis_point::set_mirror_ID(uint new_mirror_ID){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"set_mirror_ID() -> ";
		std::cout<<"new mirror ID of this sccan analysis point: ";
		std::cout<<new_mirror_ID<<std::endl;
	}
	mirror_ID = new_mirror_ID;
}
//======================================================================
uint sccan_analysis_point::get_mirror_ID(){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_mirror_ID() -> ";
		std::cout<<" mirror ID of this sccan analysis point: ";
		std::cout<<mirror_ID<<std::endl;
	}
	return mirror_ID;
}
//======================================================================
void sccan_analysis_point::set_normalized_light_flux(double new_normalized_light_flux){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"set_normalized_light_flux() -> ";
		std::cout<<"new_normalized_light_flux ";
		std::cout<<new_normalized_light_flux<<std::endl;
	}	
	normalized_light_flux = new_normalized_light_flux;
}
//======================================================================
double sccan_analysis_point::get_normalized_light_flux()const{
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_normalized_light_flux() -> ";
		std::cout<<"normalized_light_flux ";
		std::cout<<normalized_light_flux<<std::endl;
	}	
	return normalized_light_flux;
}
//======================================================================
void sccan_analysis_point::set_star_position_relative_to_pointing_direction
(pointing_direction new_star_position_relative_to_pointing_direction){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"set_star_position_relative_to_pointing_direction() -> ";
		std::cout<<std::endl;
	}	
	star_position_relative_to_pointing_direction = 
	new_star_position_relative_to_pointing_direction;
}
//======================================================================
pointing_direction sccan_analysis_point::get_star_position_relative_to_pointing_direction(){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_star_position_relative_to_pointing_direction() -> ";
		std::cout<<std::endl;
	}	
	return star_position_relative_to_pointing_direction;
}
//======================================================================
void sccan_analysis_point::toggle_verbosity(bool new_verbosity){
	
	verbosity = new_verbosity;
	
	star_position_relative_to_pointing_direction.
	toggle_verbosity(verbosity);
}
//======================================================================
