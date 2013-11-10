#include "sccan_point_analysis.h"
//======================================================================
sccan_point_analysis::sccan_point_analysis(
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler,
reflector* new_pointer_to_reflector){
	
	sccan_point_analysis_verbosity = true;
	
	if(sccan_point_analysis_verbosity){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"sccan_point_analysis()";
		std::cout<<std::endl;
	}
	
	menu_name = "sccan point analysis";
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	pointer_to_reflector = new_pointer_to_reflector;		
}	
//======================================================================
void sccan_point_analysis::fill_sccan_matrix(){
	
	sccan_matrix.clear();
	
	// update sccan point list of current directory
	pointer_to_sccan_point_pair_handler->
	update_list_of_runs_in_current_directory();

	//for all sccan points
	for(int sccan_point_itterator = 0;
	sccan_point_itterator<
	pointer_to_sccan_point_pair_handler->
	get_number_of_sccan_points_in_current_directory();
	sccan_point_itterator++)
	{
		sccan_point_pair current_sccan_point
		(0,sccan_point_analysis_verbosity);
		
		if(current_sccan_point.load_sccan_point_pair(
		pointer_to_sccan_point_pair_handler->
		get_sccan_point_pair_name_at(sccan_point_itterator)
		)){
			if(sccan_point_analysis_verbosity){
				std::cout<<"sccan_point_analysis -> ";
				std::cout<<"fill_sccan_matrix() -> ";
				std::cout<<"sccan point "<<(sccan_point_itterator+1);
				std::cout<<"/"<<	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				std::cout<<" loading successful.";
				std::cout<<std::endl;
			}			
		}else{
				std::cout<<"sccan_point_analysis -> ";
				std::cout<<"fill_sccan_matrix() -> ";
				std::cout<<"sccan point "<<(sccan_point_itterator+1);
				std::cout<<"/"<<	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				std::cout<<" loading failed.";
				std::cout<<std::endl;			
		}
		//==============================================================
		// check whether a single star was found or not 
		//==============================================================
		if(current_sccan_point.a_single_star_has_been_found()){ 
			// a single star in sccan point
			
			std::vector<sccan_analysis_point*> 
			list_of_sccan_analysis_points_for_specific_sccan_point;
			//==========================================================
			//for all mirrors on reflector
			//==========================================================
			for(int mirror_itterator = 0;
			mirror_itterator<
			pointer_to_reflector->get_number_of_mirrors_on_reflector();
			mirror_itterator++)
			{
				if(sccan_point_analysis_verbosity){
					std::cout<<"_____________________________________"<<std::endl;
					std::cout<<"sccan_point_analysis -> ";
					std::cout<<"fill_sccan_matrix() -> ";
					std::cout<<"sccan point "<<(sccan_point_itterator+1);
					std::cout<<"/"<<	
					pointer_to_sccan_point_pair_handler->
					get_number_of_sccan_points_in_current_directory();
					std::cout<<" mirror "<<(mirror_itterator+1);
					std::cout<<"/"<<	
					pointer_to_reflector->
					get_number_of_mirrors_on_reflector();
					std::cout<<std::endl;
				}
				
				sccan_analysis_point* 
				pointer_to_specific_sccan_analysis_point = 
				current_sccan_point.calculate_sccan_analysis_point(
				pointer_to_reflector->
				get_pointer_to_list_of_Points_inside_mirror_polygon
				(mirror_itterator),
				pointer_to_reflector->list_of_pointers_to_mirrors.
				at(mirror_itterator)->get_mirror_ID()
				);
				
				pointer_to_specific_sccan_analysis_point->
				toggle_verbosity(sccan_point_analysis_verbosity);
				
				list_of_sccan_analysis_points_for_specific_sccan_point.
				push_back(pointer_to_specific_sccan_analysis_point);
			}
			
			sccan_matrix.push_back
			(list_of_sccan_analysis_points_for_specific_sccan_point);
			
		}else{
			// no single star in sccan point
			if(sccan_point_analysis_verbosity){
				std::cout<<"sccan_point_analysis -> ";
				std::cout<<"fill_sccan_matrix() -> ";
				std::cout<<"sccan point "<<(sccan_point_itterator+1);
				std::cout<<"/"<<	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				std::cout<<" no single star found!"<<std::endl;
				std::cout<<std::endl;
			}			
		}
	}
}
//======================================================================
std::string sccan_point_analysis::get_sccan_matrix_prompt(){
	std::stringstream out;
	
	int number_of_sccan_analysis_points =  sccan_matrix.size();
	if(number_of_sccan_analysis_points<1){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"get_sccan_matrix_prompt() -> ";
		std::cout<<"sccan matrix is empty."<<std::endl;
	}else{
		int number_of_mirrors =  sccan_matrix.at(0).size();
		
		if(number_of_mirrors<1){
			std::cout<<"sccan_point_analysis -> ";
			std::cout<<"get_sccan_matrix_prompt() -> ";
			std::cout<<"sccan matrix has no mirrors."<<std::endl;
		}
		//==============================================================
		//for all mirrors on reflector
		//==============================================================
		for(
		int mirror_itterator = 0;
		mirror_itterator<number_of_mirrors;
		mirror_itterator++)
		{	
			//==========================================================
			//for all sccan points
			//==========================================================
			for(
			int sccan_point_itterator = 0;
			sccan_point_itterator<number_of_sccan_analysis_points;
			sccan_point_itterator++)
			{
				out<<sccan_matrix.
				at(sccan_point_itterator).
				at(mirror_itterator)->get_info_prompt();
				out<<std::endl;
			}
		}
	}

	return out.str();
}
//======================================================================
void sccan_point_analysis::display_sccan_matrix_prompt(){
	std::cout<<std::endl;
	std::cout<<get_sccan_matrix_prompt();
}
//======================================================================
std::string sccan_point_analysis::get_analysis_prompt(){
	std::stringstream out;
	std::stringstream info;
	
	out<<"| analysis"<<std::endl;
	
	// sccan matrix
	info.str("");
	if(sccan_matrix.size()>0){
		info<<sccan_matrix.size()<<" sccan points times";
		info<<" "<<sccan_matrix.at(0).size()<<" mirrors";
	}else{
		info<<" empty";
	}
	out<<make_nice_line_with_dots
	("| sccan matrix: ",info.str());	
	
	// reflector name
	out<<make_nice_line_with_dots
	("| reflector name",pointer_to_reflector->get_reflector_name());
	
	// number of mirrors
	info.str("");
	info<<pointer_to_reflector->get_number_of_mirrors_on_reflector();
	out<<make_nice_line_with_dots
	("| number of mirrors",info.str());
	
	// horizontal line 
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str(); 
}
//======================================================================
void sccan_point_analysis::interaction(){
	
	std::string key_fill_sccan_point_matrix  ="f";
	add_control(key_fill_sccan_point_matrix,
	"fill sccan point matrix");

	std::string key_show_sccan_point_matrix  ="s";
	add_control(key_show_sccan_point_matrix,
	"show sccan point matrix");	
	
	std::string key_draw_all_mirror_response_maps  ="draw";
	add_control(key_draw_all_mirror_response_maps,
	"draw all mirror response maps");
	
	std::string key_user_wants_to_end_analysis  ="back";
	add_control(key_user_wants_to_end_analysis,
	"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_analyse = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_analysis_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_end_analysis)
	==0){
		flag_user_wants_to_analyse = false;
	}
	//==================================================================
	if(valid_user_input.compare(key_fill_sccan_point_matrix)==0){
		fill_sccan_matrix();
	}
	//==================================================================
	if(valid_user_input.compare(key_show_sccan_point_matrix)==0){
		display_sccan_matrix_prompt();
		draw_telescope_pointing_directions_of_sccan_runs();
	}
	//==================================================================
	if(valid_user_input.compare(key_draw_all_mirror_response_maps)==0){
		draw_all_mirror_light_flux_response_maps();
	}
	//==================================================================
}while(flag_user_wants_to_analyse);
}
//======================================================================
void sccan_point_analysis::toggle_verbosity(){
	sccan_point_analysis_verbosity = !sccan_point_analysis_verbosity;
}
//======================================================================
bool sccan_point_analysis::verbosity_status(){
	return sccan_point_analysis_verbosity;
}
//======================================================================
void sccan_point_analysis::draw_telescope_pointing_directions_of_sccan_runs(){
   
	std::stringstream python_execution_string;

	python_execution_string<<"import pylab"<<std::endl;
	python_execution_string<<"pylab.plot(";
	python_execution_string<<get_tilt_position_matrix_for_python();
	python_execution_string<<",'x')"<<std::endl;
	python_execution_string<<"pylab.xlabel('x-tilt [DEG]')"<<std::endl;
	python_execution_string<<"pylab.ylabel('y-tilt [DEG]')"<<std::endl;
	python_execution_string<<"pylab.title(";
	python_execution_string<<"'pointing direction of sccan runs'";
	python_execution_string<<")"<<std::endl;
	python_execution_string<<"pylab.show()"<<std::endl;

	if(verbosity){	
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"draw_telescope_pointing_directions_of_sccan_runs() -> ";
		std::cout<<"plotting using python:"<<std::endl;
		std::cout<<"_________________________________________________"<<std::endl;
		std::cout<<python_execution_string.str();
		std::cout<<"|________________________________________________"<<std::endl;
	}
	
	PyRun_SimpleString(python_execution_string.str().c_str());
}
//======================================================================
void sccan_point_analysis::draw_all_mirror_light_flux_response_maps(){
	
	if(sccan_matrix.size()>0){
	
		if(sccan_matrix.at(0).size()==0){
			std::cout<<"sccan_point_analysis -> ";
			std::cout<<"draw_all_mirror_light_flux_response_maps() -> ";
			std::cout<<"there are no mirrors in sccan_matrix!"<<std::endl;			
		}else{
		
			for(uint mirror_itterator = 0;
			mirror_itterator<sccan_matrix.at(0).size(); 
			mirror_itterator++){
				
				if(verbosity){
					std::cout<<"sccan_point_analysis -> ";
					std::cout<<"draw_all_mirror_light_flux_response_maps()";
					std::cout<<" -> ";
					std::cout<<"draw response map "<< mirror_itterator+1;
					std::cout<<"of"<<sccan_matrix.at(0).size();
					std::cout<<" for mirror ID ";
					std::cout<<sccan_matrix.at(0).at(mirror_itterator)->
					get_mirror_ID();
					std::cout<<std::endl;
				}
				draw_normalized_light_flux_response_map
				(mirror_itterator);
			}
		}
	}else{
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"draw_all_mirror_light_flux_response_maps() -> ";
		std::cout<<"there are no sccan runs in sccan_matrix!"<<std::endl;
	}
}
//======================================================================
void sccan_point_analysis::draw_normalized_light_flux_response_map(
uint mirror_itterator){
	std::stringstream python_execution_string;

	python_execution_string<<"from mpl_toolkits.mplot3d import axes3d"<<std::endl;
	python_execution_string<<"import matplotlib.pyplot as plt"<<std::endl;
	python_execution_string<<"from matplotlib import cm"<<std::endl;
	python_execution_string<<"import numpy as np"<<std::endl;
	
	python_execution_string<<"fig = plt.figure()"<<std::endl;
	python_execution_string<<"ax = fig.add_subplot(111, projection='3d')"<<std::endl;
	python_execution_string<<"ax.plot_surface("<<get_tilt_position_matrix_for_python()<<","<<get_light_flux_matrix_for_python(mirror_itterator)<<", rstride=1, cstride=1, cmap=cm.coolwarm, linewidth=0, antialiased=False)"<<std::endl;
	python_execution_string<<"plt.show()"<<std::endl;
	
	if(verbosity){	
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"draw_telescope_pointing_directions_of_sccan_runs() -> ";
		std::cout<<"plotting using python:"<<std::endl;
		std::cout<<"_________________________________________________"<<std::endl;
		std::cout<<python_execution_string.str();
		std::cout<<"|________________________________________________"<<std::endl;
	}
	
	PyRun_SimpleString(python_execution_string.str().c_str());
}
//======================================================================
std::string sccan_point_analysis::get_tilt_position_matrix_for_python(){
	
	std::stringstream list_of_x_tilts_in_deg;
	std::stringstream list_of_y_tilts_in_deg;
	
	for(int pointing_position_itterator = 0;
	pointing_position_itterator<sccan_matrix.size();
	pointing_position_itterator++)
	{
		if(sccan_matrix.at(pointing_position_itterator).size()>0)
		{
			list_of_x_tilts_in_deg<<
			sccan_matrix.
			at(pointing_position_itterator).at(0)->
			get_star_position_relative_to_pointing_direction().
			get_x_tilt_prompt_in_deg();
			
			list_of_y_tilts_in_deg<<
			sccan_matrix.
			at(pointing_position_itterator).at(0)->
			get_star_position_relative_to_pointing_direction().
			get_y_tilt_prompt_in_deg();
				
			// insert delimiters	
			if(pointing_position_itterator<(sccan_matrix.size()-1))
			{
				list_of_x_tilts_in_deg<<",";
				list_of_y_tilts_in_deg<<",";
			}
		}
	}
	std::stringstream tilt_position_matrix_for_python;
	tilt_position_matrix_for_python<<"[";
	tilt_position_matrix_for_python<<list_of_x_tilts_in_deg.str();
	tilt_position_matrix_for_python<<"],[";
	tilt_position_matrix_for_python<<list_of_y_tilts_in_deg.str();
	tilt_position_matrix_for_python<<"]";
	return tilt_position_matrix_for_python.str();
}
//======================================================================
std::string sccan_point_analysis::get_light_flux_matrix_for_python(
uint mirror_itterator){
	

	if(verbosity){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"get_normalized_light_flux_of";
		std::cout<<"_specific_mirror_for_python -> ";
		std::cout<<"mirror ID "<<sccan_matrix.at(0).at(mirror_itterator)->
		get_mirror_ID();
		std::cout<<" at position ";
		std::cout<<mirror_itterator<<std::endl;
	}
	
	std::stringstream list_of_normalized_light_flux;
	
	for(int pointing_position_itterator = 0;
	pointing_position_itterator<sccan_matrix.size();
	pointing_position_itterator++)
	{
		list_of_normalized_light_flux<< 
		sccan_matrix.
		at(pointing_position_itterator).
		at(mirror_itterator)->
		get_normalized_light_flux();
		
		// insert delimiters	
		if(pointing_position_itterator<(sccan_matrix.size()-1))
		{
			list_of_normalized_light_flux<<",";
		}
	}
	
	std::stringstream list_of_normalized_light_flux_python_array_string;
	list_of_normalized_light_flux_python_array_string<<"[";
	list_of_normalized_light_flux_python_array_string<<
	list_of_normalized_light_flux.str();
	list_of_normalized_light_flux_python_array_string<<"]";
	
	return list_of_normalized_light_flux_python_array_string.str();
		
}
//======================================================================
