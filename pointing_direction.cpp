#include "pointing_direction.h"
//======================================================================
pointing_direction::pointing_direction(){
	direction_in_x_in_radiant = 0.0;
	direction_in_y_in_radiant = 0.0;	
	number_of_sccan_point_run_for_brightest_response = 0;
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

	bool sign_of_angle_is_positiv;
	if(angle_in_rad>0.0)
	{sign_of_angle_is_positiv = true;}
	else
	{sign_of_angle_is_positiv = false;}

	double DEG = angle_in_rad/(2.0*M_PI)*360.0;
	double DEG_full;
	double DEG_fraction = modf(DEG,&DEG_full);
	
	if(verbosity){
		std::cout<<"pointing_direction -> ";
		std::cout<<"get_angle_text_prompt_in_deg_arc_min_arc_sec() -> ";
		std::cout<<DEG<<"deg, full: "<<DEG_full<<"deg"<<std::endl;
	}

	double arc_Min = fabs(DEG_fraction*60.0);
	double arc_Min_full;
	double arc_Min_fraction = modf(arc_Min,&arc_Min_full);
	
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
	if(fabs(arc_sec)<0.001)
		arc_sec = 0.0;
	
	
	std::stringstream out;
	
	if(sign_of_angle_is_positiv){out<<"+";}
	
	out<<DEG_full<<"DEG "<<arc_Min_full<<"min ";
	out.precision(3);
	out<<arc_sec<<"sec";
	return out.str();
}
//======================================================================
pointing_direction pointing_direction::
operator*(const double ScaleFactor) const{
	pointing_direction temp;
	
	temp.direction_in_x_in_radiant = 
	ScaleFactor*direction_in_x_in_radiant;
	
	temp.direction_in_y_in_radiant = 
	ScaleFactor*direction_in_y_in_radiant;
	
	return temp;
}
//======================================================================
pointing_direction pointing_direction::
operator+(const pointing_direction dir_to_add) const{
	
	pointing_direction temp;
	
	temp.direction_in_x_in_radiant = 
	direction_in_x_in_radiant + dir_to_add.direction_in_x_in_radiant;
	
	temp.direction_in_y_in_radiant = 
	direction_in_y_in_radiant + dir_to_add.direction_in_y_in_radiant;
	
	return temp;
}
//======================================================================
double pointing_direction::
get_x_tilt_in_deg()const{
	return direction_in_x_in_radiant*360.0/(2.0*M_PI);
}
//======================================================================
double pointing_direction::
get_y_tilt_in_deg()const{
	return direction_in_y_in_radiant*360.0/(2.0*M_PI);
}
//======================================================================
uint pointing_direction::get_sccan_run_number_of_highest_response()const{
	return number_of_sccan_point_run_for_brightest_response;
}
//======================================================================
void pointing_direction::set_sccan_run_number_of_highest_response(uint sccan_run_number){
	number_of_sccan_point_run_for_brightest_response = sccan_run_number;
}
//======================================================================
pointing_direction::pointing_direction(
const double dir_x_in_rad,const double dir_y_in_rad,uint sccan_run_number){
	
	number_of_sccan_point_run_for_brightest_response = sccan_run_number;
	
	direction_in_x_in_radiant = dir_x_in_rad;
	direction_in_y_in_radiant = dir_y_in_rad;
}
