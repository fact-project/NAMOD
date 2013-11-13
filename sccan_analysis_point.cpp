#include "sccan_analysis_point.h"
//======================================================================
sccan_analysis_point::sccan_analysis_point(){
	verbosity = false;
	mirror_ID = 0;
	normalized_light_flux = 0.0;
	star_light_flux_in_bulbs = 0.0;
	mirror_light_flux_in_bulbs = 0.0;
}
//======================================================================
std::string sccan_analysis_point::get_info_prompt(){
	std::stringstream out;
	out<<" _sccan_analysis_point_mirror_ID_"<<mirror_ID;
	out<<"________"<<std::endl;
	out<<"| telescope tilt in x.......: ";
	out<<star_position_relative_to_pointing_direction.
	get_x_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| telescope tilt in y.......: ";
	out<<star_position_relative_to_pointing_direction.
	get_y_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| star flux.................: "<<star_light_flux_in_bulbs;
	out<<"[bulbs]"<<std::endl;
	out<<"| mirror flux...............: "<<mirror_light_flux_in_bulbs;	
	out<<"[bulbs]"<<std::endl;
	out<<"| mirror flux / star flux...: "<<normalized_light_flux;
	out<<"[1]";
	out<<std::endl;
	out<<"|________________________________________________"<<std::endl;
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
//~ void sccan_analysis_point::set_normalized_light_flux(double new_normalized_light_flux){
	//~ if(verbosity){
		//~ std::cout<<"sccan_analysis_point -> ";
		//~ std::cout<<"set_normalized_light_flux() -> ";
		//~ std::cout<<"new_normalized_light_flux ";
		//~ std::cout<<new_normalized_light_flux<<std::endl;
	//~ }	
	//~ normalized_light_flux = new_normalized_light_flux;
//~ }
//======================================================================
void sccan_analysis_point::set_light_flux(
double new_star_light_flux_in_bulbs,
double new_mirror_light_flux_in_bulbs){
	
	star_light_flux_in_bulbs = new_star_light_flux_in_bulbs;
	mirror_light_flux_in_bulbs = new_mirror_light_flux_in_bulbs;
	
	normalized_light_flux =	
	mirror_light_flux_in_bulbs/star_light_flux_in_bulbs;
	
	if(verbosity){
		std::cout << "sccan_analysis_point -> ";
		std::cout << "set_light_flux() -> ";
		std::cout << "star: ";
		std::cout << star_light_flux_in_bulbs <<"[bulbs], ";
		std::cout << "mirror: ";
		std::cout << mirror_light_flux_in_bulbs <<"[bulbs], ";
		std::cout << "mirror/star: ";
		std::cout << normalized_light_flux <<"[1]";
		std::cout << std::endl;
	}	
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
double sccan_analysis_point::get_mirror_light_flux()const{
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_mirror_light_flux() -> ";
		std::cout<<"mirror light flux ";
		std::cout<<mirror_light_flux_in_bulbs<<"[bulbs]"<<std::endl;
	}	
	return mirror_light_flux_in_bulbs;
}
//======================================================================
double sccan_analysis_point::get_star_light_flux()const{
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_star_light_flux() -> ";
		std::cout<<"star light flux ";
		std::cout<<star_light_flux_in_bulbs<<"[bulbs]"<<std::endl;
	}	
	return star_light_flux_in_bulbs;
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
