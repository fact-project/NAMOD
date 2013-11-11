#include "sccan_point_analysis.h"
//======================================================================
sccan_point_analysis::
sccan_point_analysis(
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler,
reflector* new_pointer_to_reflector,
ueye_camera *new_pointer_to_star_camera){
	
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
	
	pointer_to_star_camera = new_pointer_to_star_camera;
}	
//======================================================================
void sccan_point_analysis::
fill_sccan_matrix(){
	
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
std::string sccan_point_analysis::
get_sccan_matrix_prompt(){
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
void sccan_point_analysis::
display_sccan_matrix_prompt(){
	std::cout<<std::endl;
	std::cout<<get_sccan_matrix_prompt();
}
//======================================================================
std::string sccan_point_analysis::
get_analysis_prompt(){
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
void sccan_point_analysis::
interaction(){
	
	std::string key_fill_sccan_point_matrix  ="f";
	add_control(key_fill_sccan_point_matrix,
	"fill sccan point matrix");

	std::string key_show_sccan_point_matrix  ="s";
	add_control(key_show_sccan_point_matrix,
	"show sccan point matrix");	

	std::string key_analyse_sccan_point_matrix  ="a";
	add_control(key_analyse_sccan_point_matrix,
	"analyse sccan point matrix");	
	
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
		//draw_telescope_pointing_directions_of_sccan_runs();
	}
	//==================================================================
	if(valid_user_input.compare(key_analyse_sccan_point_matrix)==0){
		calculate_pointing_direction_of_telescope_for_max_mirror_response();
	}
	//==================================================================
	if(valid_user_input.compare(key_draw_all_mirror_response_maps)==0){
			
		for(uint mirror_it=0;
		mirror_it<get_number_of_mirrors();
		mirror_it++){
			draw_mirror_response(mirror_it);
		}
	}
	//==================================================================
}while(flag_user_wants_to_analyse);
}
//======================================================================
void sccan_point_analysis::
toggle_verbosity(){
	sccan_point_analysis_verbosity = !sccan_point_analysis_verbosity;
}
//======================================================================
bool sccan_point_analysis::
verbosity_status(){
	return sccan_point_analysis_verbosity;
}
//======================================================================
/*
void sccan_point_analysis::
draw_telescope_pointing_directions_of_sccan_runs(){
   
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
}*/
//======================================================================
/*
void sccan_point_analysis::
draw_all_mirror_light_flux_response_maps(){
	
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
}*/
//======================================================================
/*
void sccan_point_analysis::
draw_normalized_light_flux_response_map(
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
}*/
//======================================================================
/*
std::string sccan_point_analysis::
get_tilt_position_matrix_for_python(){
	
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
}*/
//======================================================================
/*
std::string sccan_point_analysis::
get_light_flux_matrix_for_python(
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
		
}*/
//======================================================================
void sccan_point_analysis::
calculate_pointing_direction_of_telescope_for_max_mirror_response(){
	
	int number_of_sccan_analysis_points =  sccan_matrix.size();

	if(number_of_sccan_analysis_points<1){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"calc -> ";
		std::cout<<"sccan matrix is empty."<<std::endl;
		
	}else{
		
		int number_of_mirrors =  sccan_matrix.at(0).size();
		
		if(number_of_mirrors<1){
			std::cout<<"sccan_point_analysis -> ";
			std::cout<<"calc -> ";
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
			pointing_direction 
			telescope_pointing_when_mirror_is_brightest = 
			calc_poining_when_mirror_is_brightest(mirror_itterator);
		}

	}
}
//======================================================================
pointing_direction sccan_point_analysis::
calc_poining_when_mirror_is_brightest(
uint mirror_iterator){
	
	double telescope_x_direction_in_rad_wighted_with_flux = 0.0;
	double telescope_y_direction_in_rad_wighted_with_flux = 0.0;
	double sum_of_all_mirror_responses_in_bulbs = 0.0;

	//==========================================================
	//for all sccan points of this mirror
	//==========================================================
	
	std::vector<double> dir_x;
	std::vector<double> dir_y;	
	std::vector<double> weights;

	for(
	int sccan_point_itterator = 0;
	sccan_point_itterator<sccan_matrix.size();
	sccan_point_itterator++)
	{	
		//flux
		weights.push_back(	
			sccan_matrix.
			at(sccan_point_itterator).
			at(mirror_iterator)->
			get_normalized_light_flux()
		);
		
		// x
		dir_x.push_back(
			sccan_matrix.
			at(sccan_point_itterator).
			at(mirror_iterator)->
			get_star_position_relative_to_pointing_direction().
			direction_in_x_in_radiant
		);
		
		// y
		dir_y.push_back(
			sccan_matrix.
			at(sccan_point_itterator).
			at(mirror_iterator)->
			get_star_position_relative_to_pointing_direction().
			direction_in_y_in_radiant
		);
		
	}
	
	double mean_dir_x = calc_mean_using_wights(dir_x,weights);
	double mean_dir_y = calc_mean_using_wights(dir_y,weights);	
	double std_dev_dir_x = 
	calc_standart_deviation_using_weights(dir_x,weights);	
	double std_dev_dir_y = 
	calc_standart_deviation_using_weights(dir_y,weights);	
	
	std::cout<<"sccan_point_analysis -> ";
	std::cout<<"calc -> ";
	std::cout<<"mirror ID ";
	std::cout<<sccan_matrix.at(0).at(mirror_iterator)->
	get_mirror_ID()<<std::endl;
	std::cout<<"mean x dir: "<<mean_dir_x<<" pm std_dev "<<std_dev_dir_x<<std::endl;
	std::cout<<"mean y dir: "<<mean_dir_y<<" pm std_dev "<<std_dev_dir_y<<std::endl;
	
	pointing_direction telescope_pointing_when_mirror_is_brightest(
	mean_dir_x,mean_dir_y);
	
	return telescope_pointing_when_mirror_is_brightest;
}
//======================================================================
double sccan_point_analysis::
calc_mean_using_wights(
std::vector<double> x,std::vector<double> weights){

	if(x.size() != weights.size()){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"calc_mean_using_wights -> ";
		std::cout<<"screw you! list of values size("<<x.size()<<"), ";
		std::cout<<"list of wights size("<<weights.size()<<")!";
		return 0.0;
	}
	
	double weighted_x_sum = 0.0;
	double sum_of_weights = 0.0;
	for(uint iterator=0;iterator<x.size();iterator++){
		weighted_x_sum = 
		weighted_x_sum + x.at(iterator)*weights.at(iterator);
		
		sum_of_weights = 
		sum_of_weights + weights.at(iterator);
	}
	
	return weighted_x_sum/sum_of_weights;
}
//======================================================================
double sccan_point_analysis::
calc_standart_deviation_using_weights(
std::vector<double> x,std::vector<double> weights){
	
	if(x.size() != weights.size()){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"calc_mean_using_wights -> ";
		std::cout<<"screw you! list of values size("<<x.size()<<"), ";
		std::cout<<"list of wights size("<<weights.size()<<")!";
		return 0.0;
	}
	
	double one_over_size_minus_one = 1.0 / (x.size()-1.0);
	double weighted_mean = calc_mean_using_wights(x,weights);
	
	double sum = 0.0;
	for(uint iterator=0;iterator<x.size();iterator++){
		sum = sum + pow(x.at(iterator) - weighted_mean,2.0);
	}
	
	return sqrt(one_over_size_minus_one*sum);
}
//======================================================================
void sccan_point_analysis::
draw_mirror_response(uint mirror_iterator){
	
	if(get_number_of_sccan_points()<1){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "draw_mirror_response() -> ";
		std::cout << "no sccan points in sccan matrix !";
		std::cout << std::endl;
		return;
	}
	
	if(get_number_of_mirrors()<1){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "draw_mirror_response() -> ";
		std::cout << "no mirrors in sccan matrix !";
		std::cout << std::endl;
		return;
	}
	
	//==================================================================
	// filename
	//==================================================================
		
	std::string filename_extension;
	filename_extension = ".eps";
	
	std::stringstream plot_file_name;
	plot_file_name << "mirror_ID" <<
	sccan_matrix.at(0).at(mirror_iterator)->get_mirror_ID();
	plot_file_name << "_response_map";
	plot_file_name << filename_extension;
	
	if(sccan_point_analysis_verbosity){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "draw_mirror_response() -> ";
		std::cout << "filename = ";
		std::cout << plot_file_name.str();
		std::cout << std::endl;
	}

	//==================================================================
	// prepare data for plotting
	//==================================================================
	
	mglData rel_poi_dir_of_star_x_in_deg(sccan_matrix.size());
	mglData rel_poi_dir_of_star_y_in_deg(sccan_matrix.size());
	mglData mirror_response(sccan_matrix.size());
	
	for(
	uint sccan_point_iterator=0;
	sccan_point_iterator<sccan_matrix.size();
	sccan_point_iterator++){
	
		rel_poi_dir_of_star_x_in_deg.a[ sccan_point_iterator ] = 
		(360.0/(2.0*M_PI)) *  
		sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
		get_star_position_relative_to_pointing_direction().
		direction_in_x_in_radiant;
	
		rel_poi_dir_of_star_y_in_deg.a[ sccan_point_iterator ] = 
		(360.0/(2.0*M_PI)) * 
		sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
		get_star_position_relative_to_pointing_direction().
		direction_in_y_in_radiant;
		
		mirror_response.a[ sccan_point_iterator ] = 
		sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
		get_normalized_light_flux();
	}
	//==================================================================	
	// fit 2D gaussian
	//==================================================================	
	mglGraph gr;
	
	mreal ini[5] = {mirror_response.Maximal(),0,10,0,10};
	mglData Ini(5,ini);

	mglData fit_result = gr.FitS(
	rel_poi_dir_of_star_x_in_deg,
	rel_poi_dir_of_star_y_in_deg,
	mirror_response,
	"a*(exp( -( ((x-u)*(x-u))/(2*v*v) + ((y-n)*(y-n))/(2*m*m) ) ))",
	"auvmn",
	Ini);
	
	std::cout<<"Fitting mirror norm "<<fit_result[0]<<std::endl;
	std::cout<<"Fit x pos "<<fit_result[1]<<" pm "<<fit_result[2]<<std::endl;
	std::cout<<"Fit y pos "<<fit_result[3]<<" pm "<<fit_result[4]<<std::endl;
	
	mglData best_x = fit_result[1];
	mglData best_y = fit_result[3];
	mglData best_z = fit_result[0]*(
	exp( -( 
	(best_x[0]-fit_result[1])*(best_x[0]-fit_result[1])/(2.0*fit_result[2]) + 
	(best_y[0]-fit_result[3])*(best_y[0]-fit_result[3])/(2.0*fit_result[4]) ) 
	)
	);
	//==================================================================	
	// calculate
	//==================================================================	
	pointing_direction best_dir = 
	calc_poining_when_mirror_is_brightest(
	mirror_iterator);
	
	mglData bM_x(1);
	mglData bM_y(1);
	mglData bM_z(1);
		
	bM_x[0] = best_dir.direction_in_x_in_radiant*(360.0/(2.0*M_PI));
	bM_y[0] = best_dir.direction_in_y_in_radiant*(360.0/(2.0*M_PI));
	bM_z[0] = mirror_response.Maximal();

	std::cout<<"mean x pos "<<bM_x[0]<<std::endl;
	std::cout<<"mean y pos "<<bM_y[0]<<std::endl;
	std::cout<<"mean response "<<bM_z[0]<<std::endl;
	//==================================================================
	// plotting response map
	//==================================================================
	std::stringstream plot_heading;
	plot_heading << "Mirror ID ";
	plot_heading << sccan_matrix.at(0).at(mirror_iterator)->
	get_mirror_ID();
	plot_heading << " response map";
	
	double x_range = 5.0;
	double y_range = 5.0;
	
	mglData xy_hist = gr.Hist(
	rel_poi_dir_of_star_x_in_deg,
	rel_poi_dir_of_star_y_in_deg,
	mirror_response);	
	
	gr.Title(plot_heading.str().c_str());
	//====================================
	gr.SubPlot(1,2,0);	
	gr.Rotate(50,60);

	gr.SetRanges(
	rel_poi_dir_of_star_x_in_deg.Minimal()*1.1,
	rel_poi_dir_of_star_x_in_deg.Maximal()*1.1,
	rel_poi_dir_of_star_y_in_deg.Minimal()*1.1,
	rel_poi_dir_of_star_y_in_deg.Maximal()*1.1,
	0.0,mirror_response.Maximal());
	
	gr.Axis();
	gr.Label('x',"x direction [deg]",0);
	gr.Label('y',"y direction [deg]",0);
	gr.Label('z',"mirror response [1]",0);
	gr.Box(); 
	gr.Light(true);
	
	gr.ContV(xy_hist);
	gr.ContF(xy_hist);
	gr.Cont(xy_hist,"k");
	// best fit result
	//~ gr.Stem(
	//~ best_x,
	//~ best_y,
	//~ best_z,
	//~ "rx");
	gr.Stem(
	bM_x,
	bM_y,
	bM_z,
	"rx");
	
		
	
	//====================================
	gr.SubPlot(1,2,1);
	gr.Rotate(50,120);
	
	gr.SetRanges(
	rel_poi_dir_of_star_x_in_deg.Minimal()*1.1,
	rel_poi_dir_of_star_x_in_deg.Maximal()*1.1,
	rel_poi_dir_of_star_y_in_deg.Minimal()*1.1,
	rel_poi_dir_of_star_y_in_deg.Maximal()*1.1,
	0.0,mirror_response.Maximal());
	
	gr.Axis();
	gr.Label('x',"x direction [deg]",0);
	gr.Label('y',"y direction [deg]",0);
	gr.Label('z',"mirror response [1]",0);
	gr.Box(); 
	gr.Light(true);
	
	gr.ContV(xy_hist);
	gr.ContF(xy_hist);
	gr.Cont(xy_hist,"k");

	gr.WriteFrame(plot_file_name.str().c_str());	// save it
}
//======================================================================
uint sccan_point_analysis::
get_number_of_mirrors(){
	if(sccan_matrix.size()<1){
		return 0;
	}else{
		return sccan_matrix.at(0).size();
	}
}
//======================================================================
uint sccan_point_analysis::
get_number_of_sccan_points(){
	return sccan_matrix.size();
}
//======================================================================
