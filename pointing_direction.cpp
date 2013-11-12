#include "pointing_direction.h"
//======================================================================
pointing_direction::pointing_direction(){
	direction_in_x_in_radiant = 0.0;
	direction_in_y_in_radiant = 0.0;	
}
//======================================================================
pointing_direction::pointing_direction(
const double dir_x_in_rad,const double dir_y_in_rad){
	direction_in_x_in_radiant = dir_x_in_rad;
	direction_in_y_in_radiant = dir_y_in_rad;
}
//======================================================================
std::string pointing_direction::get_x_tilt_prompt_in_deg_min_sec(){
	return get_angle_text_prompt_in_deg_arc_min_arc_sec
	(direction_in_x_in_radiant);
}
//======================================================================
std::string pointing_direction::get_y_tilt_prompt_in_deg_min_sec(){
	return get_angle_text_prompt_in_deg_arc_min_arc_sec
	(direction_in_y_in_radiant);
}
//======================================================================
std::string pointing_direction::get_x_tilt_prompt_in_deg(){
	std::stringstream x_tilt_prompt_in_deg;
	x_tilt_prompt_in_deg<<direction_in_x_in_radiant/(2*M_PI)*360;
	return x_tilt_prompt_in_deg.str();
}
//======================================================================
std::string pointing_direction::get_y_tilt_prompt_in_deg(){
	std::stringstream y_tilt_prompt_in_deg;
	y_tilt_prompt_in_deg<<direction_in_y_in_radiant/(2*M_PI)*360;
	return y_tilt_prompt_in_deg.str();
}
//======================================================================
std::string pointing_direction::
get_angle_text_prompt_in_deg_arc_min_arc_sec(double angle_in_rad){
	
	if(verbosity){
		std::cout<<"pointing_direction -> ";
		std::cout<<"get_angle_text_prompt_in_deg_arc_min_arc_sec() -> ";
		std::cout<<angle_in_rad<<"rad"<<std::endl;
	}

	double DEG = angle_in_rad/(2.0*M_PI)*360.0;
	double DEG_full, DEG_fraction;
	DEG_fraction = modf(DEG,&DEG_full);
	
	if(verbosity){
		std::cout<<"pointing_direction -> ";
		std::cout<<"get_angle_text_prompt_in_deg_arc_min_arc_sec() -> ";
		std::cout<<DEG<<"deg, full: "<<DEG_full<<"deg"<<std::endl;
	}

	double arc_Min = abs(DEG_fraction*60.0);
	double arc_Min_full, arc_Min_fraction;
	arc_Min_fraction = modf(arc_Min,&arc_Min_full);
	
	if(verbosity){
		std::cout<<"pointing_direction -> ";
		std::cout<<"get_angle_text_prompt_in_deg_arc_min_arc_sec() -> ";
		std::cout<<arc_Min<<"min, full: "<<arc_Min_full<<"min"<<std::endl;
	}
	
	double arc_sec = arc_Min_fraction*60.0;

	if(verbosity){
		std::cout<<"pointing_direction -> ";
		std::cout<<"get_angle_text_prompt_in_deg_arc_min_arc_sec() -> ";
		std::cout<<arc_sec<<"sec"<<std::endl;
	}
	
	std::stringstream out;
	out<<DEG_full<<"DEG "<<arc_Min_full<<"min ";
	out.precision(2);
	out<<arc_sec<<"sec";
	return out.str();
}
//======================================================================
pointing_direction pointing_direction::operator*(
const double ScaleFactor) const{
	pointing_direction temp;
	
	temp.direction_in_x_in_radiant = 
	ScaleFactor*direction_in_x_in_radiant;
	
	temp.direction_in_y_in_radiant = 
	ScaleFactor*direction_in_y_in_radiant;
	
	return temp;
}
