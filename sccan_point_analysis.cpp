#include "sccan_point_analysis.h"
//------------------------------------------------------------------------------
sccan_point_analysis::sccan_point_analysis(
	sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler,
	reflector* new_pointer_to_reflector
) {
	
	sccan_point_analysis_verbosity = true;
	
	if(sccan_point_analysis_verbosity){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "sccan_point_analysis()";
		std::cout << std::endl;
	}
	
	menu_name = "sccan point analysis";
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	
	normalize_mirror_response = true;
	
	max_only_instead_of_CoG = true;
	
	use_multithread = true;
	
	pointer_to_reflector = new_pointer_to_reflector;

}	
//------------------------------------------------------------------------------
void sccan_point_analysis::fill_sccan_matrix(){
	
	sccan_matrix.clear();
	
	// update sccan point list of current directory
	pointer_to_sccan_point_pair_handler->
	update_list_of_runs_in_current_directory();
	//for all sccan points
	
	int sccan_point_itterator;
	
	if(use_multithread){
	
		#pragma omp parallel private(sccan_point_itterator) 
		#pragma omp for schedule(dynamic) 	
		for(sccan_point_itterator=0;
			sccan_point_itterator<
			pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
			sccan_point_itterator++
		) {
			fill_sccan_point(sccan_point_itterator);
		}
	}else{
		for(int sccan_point_itterator = 0;
			sccan_point_itterator<
			pointer_to_sccan_point_pair_handler->
			get_number_of_sccan_points_in_current_directory();
			sccan_point_itterator++
		) {
			fill_sccan_point(sccan_point_itterator);
		}	
	}
}
//------------------------------------------------------------------------------
void sccan_point_analysis::fill_sccan_point(int sccan_point_itterator){
		
	sccan_point_pair current_sccan_point
	(0,sccan_point_analysis_verbosity);
	
	if(current_sccan_point.load_sccan_point_pair(
	pointer_to_sccan_point_pair_handler->
	get_sccan_point_pair_name_at(sccan_point_itterator)
	)){
		if(sccan_point_analysis_verbosity){
			std::stringstream out;
			out << "sccan_point_analysis -> ";
			out << "fill_sccan_matrix() -> fill_sccan_point() -> ";
			out << "sccan point " << (sccan_point_itterator+1);
			out << "/" << 	
			pointer_to_sccan_point_pair_handler->
			get_number_of_sccan_points_in_current_directory();
			out << " loading successful.";
			out << "\n";
			std::cout  <<  out.str();
		}			
	}else{
		std::stringstream out;
		out << "sccan_point_analysis -> ";
		out << "fill_sccan_matrix() -> fill_sccan_point() -> ";
		out << "sccan point " << (sccan_point_itterator+1);
		out << "/" << 	
		pointer_to_sccan_point_pair_handler->
		get_number_of_sccan_points_in_current_directory();
		out << " loading failed.";
		out << "\n";
		std::cout << out.str();			
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
				std::stringstream out;
				out << "_____________________________________" << std::endl;
				out << "sccan_point_analysis -> ";
				out << "fill_sccan_matrix() -> fill_sccan_point() -> ";
				out << "sccan point " << (sccan_point_itterator+1);
				out << "/" << 	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				out << " mirror " << (mirror_itterator+1);
				out << "/" << 	
				pointer_to_reflector->
				get_number_of_mirrors_on_reflector();
				out << "\n";
				std::cout << out.str();
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
			std::stringstream out;
			out << "sccan_point_analysis -> ";
			out << "fill_sccan_matrix() -> fill_sccan_point() -> ";
			out << "sccan point " << (sccan_point_itterator+1);
			out << "/" << 	
			pointer_to_sccan_point_pair_handler->
			get_number_of_sccan_points_in_current_directory();
			out << " no single star found!" << std::endl;
			out << "\n";
			std::cout << out.str();
		}			
	}
}
//------------------------------------------------------------------------------
std::string sccan_point_analysis::get_sccan_matrix_prompt(){
	std::stringstream out;
	
	int number_of_sccan_analysis_points =  sccan_matrix.size();
	if(number_of_sccan_analysis_points<1){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "get_sccan_matrix_prompt() -> ";
		std::cout << "sccan matrix is empty." << std::endl;
	}else{
		int number_of_mirrors =  sccan_matrix.at(0).size();
		
		if(number_of_mirrors<1){
			std::cout << "sccan_point_analysis -> ";
			std::cout << "get_sccan_matrix_prompt() -> ";
			std::cout << "sccan matrix has no mirrors." << std::endl;
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
				out << sccan_matrix.
				at(sccan_point_itterator).
				at(mirror_itterator)->get_info_prompt();
				out << std::endl;
			}
		}
	}

	return out.str();
}
//------------------------------------------------------------------------------
void sccan_point_analysis::display_sccan_matrix_prompt(){
	std::cout << std::endl;
	std::cout << get_sccan_matrix_prompt();
}
//------------------------------------------------------------------------------
std::string sccan_point_analysis::get_analysis_prompt(){
	std::stringstream out;
	std::stringstream info;
	
	out << "| analysis" << std::endl;
	
	// sccan matrix
	info.str("");
	if(sccan_matrix.size()>0){
		info << sccan_matrix.size() << " sccan points times";
		info << " " << sccan_matrix.at(0).size() << " mirrors";
	}else{
		info << " empty";
	}
	out << make_nice_line_with_dots("| sccan matrix: ",info.str());	
	
	// reflector name
	out << make_nice_line_with_dots(
		"| reflector name",pointer_to_reflector->get_reflector_name()
	);
	
	// number of mirrors
	info.str("");
	info << pointer_to_reflector->get_number_of_mirrors_on_reflector();
	out  <<  make_nice_line_with_dots("| number of mirrors",info.str());
	
	// normalize_mirror_response
	info.str("");
	if(normalize_mirror_response)
		info << "TRUE";
	else
		info << "FALSE";
	out << make_nice_line_with_dots("| mirror response normalizing: ",info.str());	

	// method to estimate max response
	info.str("");
	if(max_only_instead_of_CoG)
		info << "maximum only";
	else
		info << "centre of gravity";
	out << make_nice_line_with_dots(
		"| method to estimate brightest response direction: ",
		info.str()
	);	


	// multithreading
	info.str("");
	if(use_multithread)
		info << "TRUE";
	else
		info << "FALSE";
	out << make_nice_line_with_dots("| multithreading ",info.str());

	// global offset
	info.str("");
	if(substract_global_offset)
		info << "TRUE";
	else
		info << "FALSE";
	out << make_nice_line_with_dots("| substract global offset ",info.str());
	out << make_nice_line_with_dots("| x global offset",global_offset.get_x_tilt_prompt_in_deg_min_sec());
	out << make_nice_line_with_dots("| y global offset",global_offset.get_y_tilt_prompt_in_deg_min_sec());

	// horizontal line 
	out << "|";
	out << get_line_of_specific_character("_",command_line_columns-1);
	out << std::endl;
	return out.str(); 
}
//------------------------------------------------------------------------------
void sccan_point_analysis::interaction(){
	
	std::string key_fill_sccan_point_matrix  ="f";
	add_control(key_fill_sccan_point_matrix,"fill sccan point matrix");

	std::string key_show_sccan_point_matrix  ="s";
	add_control(key_show_sccan_point_matrix,"show sccan point matrix");	

	std::string key_analyse_sccan_point_matrix  ="a";
	add_control(key_analyse_sccan_point_matrix,
	"analyse sccan point matrix");	
	
	std::string key_toggle_light_flux_normalizing  ="n";
	add_control(key_toggle_light_flux_normalizing,
	"toggle mirror response normalizing");	
	
	std::string key_toggle_max_response_estimation_method  ="method";
	add_control(key_toggle_max_response_estimation_method,
	"toggle max response estimation method");
	
	std::string key_draw_all_mirror_response_maps  ="draw";
	add_control(key_draw_all_mirror_response_maps,
	"draw all mirror response maps");

	std::string key_toggle_multithread  ="m";
	add_control(key_toggle_multithread,"toggle multithread");	
	
	std::string key_substract_global_direction_offset = "sub_off";
	add_control(key_substract_global_direction_offset," toggle sub. global direction offset");

	std::string key_user_wants_to_end_analysis  ="back";
	add_control(key_user_wants_to_end_analysis,"back to main menu");	
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_analyse = true;
	
	do{
		// prompt snapshot options to command line
		update_status_prompt(get_analysis_prompt());
		// get input of user via command line
		get_user_input();
		// interpret answer of user

		if(valid_user_input.compare(key_user_wants_to_end_analysis)==0)
			flag_user_wants_to_analyse = false;
		
		if(valid_user_input.compare(key_fill_sccan_point_matrix)==0)
			fill_sccan_matrix();
		
		if(valid_user_input.compare(key_show_sccan_point_matrix)==0)
			display_sccan_matrix_prompt();
		
		if(valid_user_input.compare(key_analyse_sccan_point_matrix)==0)
			run_anaysis();
		
		if(valid_user_input.compare(key_draw_all_mirror_response_maps)==0){
				
			for(uint mirror_it=0;
				mirror_it<get_number_of_mirrors();
				mirror_it++
			)
				draw_mirror_response(mirror_it);
		}
		
		if(valid_user_input.compare(key_toggle_light_flux_normalizing)==0)
			normalize_mirror_response = ! normalize_mirror_response;	
		
		if(valid_user_input.compare(key_toggle_multithread)==0)
			use_multithread =! use_multithread;	
		
		if(valid_user_input.compare(key_toggle_max_response_estimation_method)==0)
			max_only_instead_of_CoG =! max_only_instead_of_CoG;	
		
		if(valid_user_input.compare(key_substract_global_direction_offset)==0)
			substract_global_offset =! substract_global_offset;
		
	}while(flag_user_wants_to_analyse);
}
//------------------------------------------------------------------------------
void sccan_point_analysis::toggle_verbosity(){
	sccan_point_analysis_verbosity = !sccan_point_analysis_verbosity;
}
//------------------------------------------------------------------------------
bool sccan_point_analysis::verbosity_status(){
	return sccan_point_analysis_verbosity;
}
//------------------------------------------------------------------------------
pointing_direction sccan_point_analysis::PointingDirectionOfStarAtMaxMirrorResponse(
	uint mirror_iterator
) {
	
	std::vector<sccan_analysis_point*> list_of_sccan_points_of_specific_mirror;
	
	for(uint SPit=0; SPit<get_number_of_sccan_points(); SPit++) {
		list_of_sccan_points_of_specific_mirror.push_back(
			sccan_matrix.at(SPit).at(mirror_iterator)
		);
	}
	
	std::vector<sccan_analysis_point*>::iterator 
	iterator_to_brightest_sccan_point; 
	
	if(normalize_mirror_response) {
		iterator_to_brightest_sccan_point = max_element(
			list_of_sccan_points_of_specific_mirror.begin(),
			list_of_sccan_points_of_specific_mirror.end(),
			compare_normalized_flux
		);	
	}else {
		iterator_to_brightest_sccan_point = max_element(
			list_of_sccan_points_of_specific_mirror.begin(),
			list_of_sccan_points_of_specific_mirror.end(),
			compare_only_mirror_flux
		);
	}
	
	if(sccan_point_analysis_verbosity){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "PointingDirectionOfStarAtMaxMirrorResponse() -> ";
		std::cout << "mirror ID " << 
		sccan_matrix.at(0).at(mirror_iterator)->get_mirror_ID();
		std::cout << " -> ";
		std::cout << "light flux normalizing = ";
		if(normalize_mirror_response)
		{std::cout << "TRUE";}else{std::cout << "FALSE";}
		std::cout << " -> brightest star direction x:";
		std::cout << (*iterator_to_brightest_sccan_point)->
		get_star_position_relative_to_pointing_direction().
		get_x_tilt_prompt_in_deg_min_sec();
		std::cout << ", y:";
		std::cout << (*iterator_to_brightest_sccan_point)->
		get_star_position_relative_to_pointing_direction().
		get_y_tilt_prompt_in_deg_min_sec();
		std::cout << std::endl;
	}	
	
	pointing_direction brightest_response_direction = 
		(*iterator_to_brightest_sccan_point)->
		get_star_position_relative_to_pointing_direction();
	
	brightest_response_direction.set_sccan_run_number_of_highest_response(
		(*iterator_to_brightest_sccan_point)->get_sccan_run_number()
	);
	
	std::cout << "Brightest direction taken in sccan run number " << 
	(*iterator_to_brightest_sccan_point)->get_sccan_run_number() << std::endl;
	
	return brightest_response_direction;
}
//======================================================================
pointing_direction sccan_point_analysis::CentreOfGravityPointingDirectionOfMirrorResponse(uint mirror_iterator){
	
	std::stringstream CoG_protocol;
	
	pointing_direction CentreOfGravity;
	
	CoG_protocol  <<  "____Start_CoG_pointing_direction_calculation___\n";
	CoG_protocol  <<  "| CoG start:\n";
	CoG_protocol  <<  "| x dir: "  <<  CentreOfGravity.get_x_tilt_prompt_in_deg_min_sec()  <<  "\n";
	CoG_protocol  <<  "| y dir: "  <<  CentreOfGravity.get_y_tilt_prompt_in_deg_min_sec()  <<  "\n";
	
	double sum_of_all_weights_of_mirror_response = 0.0;
	
	CoG_protocol  <<  "| initial sum of all weights: "  <<  sum_of_all_weights_of_mirror_response  <<  "\n";
	
	for(uint SPit = 0; SPit<get_number_of_sccan_points(); SPit++){
		
		CoG_protocol  <<  "| \n";
		CoG_protocol  <<  "| __sccan analysis point_ "  <<  SPit  <<  "_of_"  <<  get_number_of_sccan_points()  <<  "___\n";
		
		double weight_of_mirror_response = 0.0;

		if(normalize_mirror_response)
		{
			weight_of_mirror_response = 
			sccan_matrix.at(SPit).at(mirror_iterator)->
			get_normalized_light_flux();
			
			CoG_protocol  <<  "| | weight: "  <<  weight_of_mirror_response  <<  "[1] (normalized)\n";
					
		}else
		{
			weight_of_mirror_response = 
			sccan_matrix.at(SPit).at(mirror_iterator)->
			get_mirror_light_flux();

			CoG_protocol  <<  "| | weight: "  <<  weight_of_mirror_response  <<  "[Bulbs] (not normalized)\n";			
		}
					
		sum_of_all_weights_of_mirror_response += 
		weight_of_mirror_response;	
		
		CoG_protocol  <<  "| | sum of all weights: "  <<  sum_of_all_weights_of_mirror_response  <<  "\n";
		
		CentreOfGravity = CentreOfGravity + 
		sccan_matrix.at(SPit).at(mirror_iterator)->
		get_star_position_relative_to_pointing_direction()*
		weight_of_mirror_response;
		
		CoG_protocol  <<  "| | Centre of Gravity (not normalized):\n";
		CoG_protocol  <<  "| | x dir: "  <<  CentreOfGravity.get_x_tilt_prompt_in_deg_min_sec()  <<  "\n";
		CoG_protocol  <<  "| | y dir: "  <<  CentreOfGravity.get_y_tilt_prompt_in_deg_min_sec()  <<  "\n";
		CoG_protocol  <<  "| |________________________________________\n";		
	}
	
	CentreOfGravity = 
	CentreOfGravity*(1.0/sum_of_all_weights_of_mirror_response);
	
	CoG_protocol  <<  "| Final Centre of Gravity :\n";
	CoG_protocol  <<  "| x dir: "  <<  CentreOfGravity.get_x_tilt_prompt_in_deg_min_sec()  <<  "\n";
	CoG_protocol  <<  "| y dir: "  <<  CentreOfGravity.get_y_tilt_prompt_in_deg_min_sec()  <<  "\n";

	if(sccan_point_analysis_verbosity){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "CentreOfGravityPointingDirectionOfMirrorResponse() -> ";
		std::cout << "mirror ID " << 
		sccan_matrix.at(0).at(mirror_iterator)->get_mirror_ID();
		std::cout << " -> \n";
		std::cout << CoG_protocol.str();
	}	
	
	return CentreOfGravity;
	
}
//======================================================================
void sccan_point_analysis::draw_mirror_response(uint mirror_iterator){
	
	if(get_number_of_sccan_points()<1){
		std::cout  <<  "sccan_point_analysis -> ";
		std::cout  <<  "draw_mirror_response() -> ";
		std::cout  <<  "no sccan points in sccan matrix !";
		std::cout  <<  std::endl;
		return;
	}
	
	if(get_number_of_mirrors()<1){
		std::cout  <<  "sccan_point_analysis -> ";
		std::cout  <<  "draw_mirror_response() -> ";
		std::cout  <<  "no mirrors in sccan matrix !";
		std::cout  <<  std::endl;
		return;
	}
	
	//==================================================================
	// filename
	//==================================================================
		
	std::string filename_extension;
	filename_extension = ".eps";
	
	std::stringstream plot_file_name;
	plot_file_name  <<  "mirror_ID"  << 
	sccan_matrix.at(0).at(mirror_iterator)->get_mirror_ID();
	plot_file_name  <<  "_response_map";
	plot_file_name  <<  filename_extension;

	std::stringstream csv_file_name;
	csv_file_name  <<  "mirror_ID"  << 
	sccan_matrix.at(0).at(mirror_iterator)->get_mirror_ID();
	csv_file_name  <<  "_response_map";
	csv_file_name  <<  ".csv";
	
	std::stringstream csv_table;
	
	if(sccan_point_analysis_verbosity){
		std::cout  <<  "sccan_point_analysis -> ";
		std::cout  <<  "draw_mirror_response() -> ";
		std::cout  <<  "filename = ";
		std::cout  <<  plot_file_name.str();
		std::cout  <<  std::endl;
	}

	//==================================================================
	// prepare data for plotting
	//==================================================================
	
	mglData rel_poi_dir_of_star_x_in_deg(sccan_matrix.size());
	mglData rel_poi_dir_of_star_y_in_deg(sccan_matrix.size());
	mglData mirror_response(sccan_matrix.size());
	
	csv_table  <<  "dir_X [deg],dir_Y [deg],";
	if(normalize_mirror_response)
	{csv_table  <<  "normalized_mirror_response [1]";}
	else
	{csv_table  <<  "mirror_response [bulbs]";}
	csv_table  <<  "\n";
	
	for(
	uint sccan_point_iterator=0;
	sccan_point_iterator<sccan_matrix.size();
	sccan_point_iterator++){
		
		rel_poi_dir_of_star_x_in_deg.a[ sccan_point_iterator ] = 
		(360.0/(2.0*M_PI)) *  
		sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
		get_star_position_relative_to_pointing_direction().
		direction_in_x_in_radiant;
	
		csv_table  <<  
		rel_poi_dir_of_star_x_in_deg.a[ sccan_point_iterator ]  <<  ",";
	
		rel_poi_dir_of_star_y_in_deg.a[ sccan_point_iterator ] = 
		(360.0/(2.0*M_PI)) * 
		sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
		get_star_position_relative_to_pointing_direction().
		direction_in_y_in_radiant;
	
		csv_table  <<  
		rel_poi_dir_of_star_y_in_deg.a[ sccan_point_iterator ]  <<  ",";
		
		if(normalize_mirror_response){
			mirror_response.a[ sccan_point_iterator ] = 
			sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
			get_normalized_light_flux();
		}else{
			mirror_response.a[ sccan_point_iterator ] = 
			sccan_matrix.at(sccan_point_iterator).at(mirror_iterator)->
			get_mirror_light_flux();		
		}

		csv_table  <<  mirror_response.a[ sccan_point_iterator ];
		csv_table  <<  "\n";
		
	}
	
	export_text( csv_file_name.str(),csv_table.str());
	
	//==================================================================	
	// fit 2D gaussian
	//==================================================================	
	mglGraph gr;
	
	//~ mreal ini[5] = {mirror_response.Maximal(),0,10,0,10};
	//~ mglData Ini(5,ini);
//~ 
	//~ mglData fit_result = gr.FitS(
	//~ rel_poi_dir_of_star_x_in_deg,
	//~ rel_poi_dir_of_star_y_in_deg,
	//~ mirror_response,
	//~ "a*(exp( -( ((x-u)*(x-u))/(2*v*v) + ((y-n)*(y-n))/(2*m*m) ) ))",
	//~ "auvmn",
	//~ Ini);
	//~ 
	//~ std::cout << "Fitting mirror norm " << fit_result[0] << std::endl;
	//~ std::cout << "Fit x pos " << fit_result[1] << " pm " << fit_result[2] << std::endl;
	//~ std::cout << "Fit y pos " << fit_result[3] << " pm " << fit_result[4] << std::endl;
	//~ 
	//~ mglData best_x = fit_result[1];
	//~ mglData best_y = fit_result[3];
	//~ mglData best_z = fit_result[0]*(
	//~ exp( -( 
	//~ (best_x[0]-fit_result[1])*(best_x[0]-fit_result[1])/(2.0*fit_result[2]) + 
	//~ (best_y[0]-fit_result[3])*(best_y[0]-fit_result[3])/(2.0*fit_result[4]) ) 
	//~ )
	//~ );
	//==================================================================	
	// calculate
	//==================================================================	

	pointing_direction best_dir;
	
	if(max_only_instead_of_CoG){
		best_dir = 
		PointingDirectionOfStarAtMaxMirrorResponse(mirror_iterator);
	}else{
		best_dir = 
		CentreOfGravityPointingDirectionOfMirrorResponse(mirror_iterator);
	}
	
	mglData bM_x(1);
	mglData bM_y(1);
	mglData bM_z(1);
		
	bM_x[0] = best_dir.get_x_tilt_in_deg();
	bM_y[0] = best_dir.get_y_tilt_in_deg();
	bM_z[0] = 1.5*mirror_response.Maximal();

	//~ std::cout << "mean x pos " << bM_x[0] << std::endl;
	//~ std::cout << "mean y pos " << bM_y[0] << std::endl;
	//~ std::cout << "mean response " << bM_z[0] << std::endl;
	
	//==================================================================
	// plotting response map
	//==================================================================
	std::stringstream plot_heading;
	plot_heading  <<  "Mirror ID ";
	plot_heading  <<  sccan_matrix.at(0).at(mirror_iterator)->
	get_mirror_ID();
	plot_heading  <<  " response map";
	
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
	
	if(normalize_mirror_response){
		gr.Label('z',"normalized mirror response [1]",0);
	}else{
		gr.Label('z',"mirror response [bulbs]",0);
	}
	gr.Box(); 
	gr.Light(true);
	
	//gr.ContV(xy_hist);
	//gr.ContF(xy_hist);
	//gr.Cont(xy_hist,"k");
	
	gr.Stem(
	rel_poi_dir_of_star_x_in_deg,
	rel_poi_dir_of_star_y_in_deg,
	mirror_response,
	"o");
	
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
	if(normalize_mirror_response){
		gr.Label('z',"normalized mirror response [1]",0);
	}else{
		gr.Label('z',"mirror response [bulbs]",0);
	}
	gr.Box(); 
	gr.Light(true);
	
	//gr.ContV(xy_hist);
	//gr.ContF(xy_hist);
	//gr.Cont(xy_hist,"k");

	gr.Stem(
	rel_poi_dir_of_star_x_in_deg,
	rel_poi_dir_of_star_y_in_deg,
	mirror_response,
	"o");
	
	gr.Stem(
	bM_x,
	bM_y,
	bM_z,
	"rx");

	gr.WriteFrame(plot_file_name.str().c_str());	// save it
}
//======================================================================
uint sccan_point_analysis::get_number_of_mirrors(){
	if(sccan_matrix.size()<1){
		return 0;
	}else{
		return sccan_matrix.at(0).size();
	}
}
//======================================================================
uint sccan_point_analysis::get_number_of_sccan_points(){
	return sccan_matrix.size();
}
//======================================================================
void sccan_point_analysis::run_anaysis(){
	
	if(sccan_point_analysis_verbosity){
		std::cout << "sccan_point_analysis -> ";
		std::cout << "analyse()";
		std::cout << " -> ";
		std::cout << std::endl;
	}
	
	std::stringstream instruction_table;
	
	calculate_global_offset();

	instruction_table << "global offset of telescope axis, i.e. star camera axis vs. mean of mirror missalignments: ";
	instruction_table << global_offset.get_print() << "\n";
	instruction_table << "global offset is substracted: ";
	if(substract_global_offset)
		instruction_table << "TRUE";
	else
		instruction_table << "FALSE";
	instruction_table << "\n";
	
	uint MIit;
	
	/*if(use_multithread){
		#pragma omp parallel shared(instruction_table) private(MIit) 
		#pragma omp for schedule(dynamic) 
		for(MIit = 0; MIit < get_number_of_mirrors(); MIit++){
			analyse_single_mirror_and_get_instructions(MIit,&instruction_table);
		}
	}else{*/
		for(uint MIit = 0; MIit < get_number_of_mirrors(); MIit++){
			analyse_single_mirror_and_get_instructions(MIit, instruction_table);
		}	
	//}
	
	// timestamp in filename
	std::stringstream instruction_table_filename;
	instruction_table_filename << "mirror_alignment_instruction_table_";
	instruction_table_filename << global_time_stamp_manager_instance.get_current_time_stamp();
	instruction_table_filename << ".txt";
	
	export_text( 
		instruction_table_filename.str(),
		instruction_table.str()
	);
}	
//======================================================================
void sccan_point_analysis::analyse_single_mirror_and_get_instructions(
	uint MIit, 
	std::stringstream &instruction_table
){	
	if(true){
		std::stringstream out;
		out  <<  "sccan_point_analysis -> ";
		out  <<  "analyse() -> ";
		out  <<  "instructions for Mirror ";
		out  <<  "[sccan_matrix/reflector list]ID  ";
		out  <<  sccan_matrix.at(0).at(MIit)->get_mirror_ID();
		out  <<  "/";
		out  <<  pointer_to_reflector -> 
		list_of_pointers_to_mirrors.at(MIit) -> get_mirror_ID();
		out  <<  "\n";
		std::cout  <<  out.str();
	}
	
	// draw controll plots
	draw_mirror_response(MIit);
	
	// get instructions
	pointing_direction telescope_orientation_for_best_mirror_resonse;
	
	if(max_only_instead_of_CoG){
		telescope_orientation_for_best_mirror_resonse =
			PointingDirectionOfStarAtMaxMirrorResponse(MIit);
	}else{
		telescope_orientation_for_best_mirror_resonse =
			CentreOfGravityPointingDirectionOfMirrorResponse(MIit);
	}

	if(substract_global_offset) {
		telescope_orientation_for_best_mirror_resonse =
			telescope_orientation_for_best_mirror_resonse - global_offset;
	}

	instruction_table << 
		pointer_to_reflector->list_of_pointers_to_mirrors.at(MIit)->
		get_manipulation_instructions(
			telescope_orientation_for_best_mirror_resonse
		);
}
//======================================================================
void sccan_point_analysis::calculate_global_offset() {

	std::cout << global_offset.get_print() << "\n";

	for(uint MIit = 0; MIit < get_number_of_mirrors(); MIit++){
		
		if(max_only_instead_of_CoG)
			global_offset = global_offset + PointingDirectionOfStarAtMaxMirrorResponse(MIit);
		else
			global_offset = global_offset + CentreOfGravityPointingDirectionOfMirrorResponse(MIit);
		
		std::cout << global_offset.get_print() << "\n";
	}

	if(get_number_of_mirrors()>=1)
		global_offset = global_offset/get_number_of_mirrors();

	std::cout << global_offset.get_print() << "\n";
}