#include "sccan_point_pair_handler.h"
//======================================================================
sccan_point_pair_handler::
sccan_point_pair_handler(){
	verbosity = false;
	
	list_of_star_positions_of_all_points_in_directory.
	toggle_verbosity(verbosity);
	
	star_position_indicator_image.
	toggle_verbosity(verbosity);
	
	status_display.toggle_verbosity(verbosity);

	if(verbosity){
		std::cout<<"sccan_point_pair_handler()"<<std::endl;
	}
	star_camera_last_good_exposure_time_in_ms = 10.0;
	reflector_camera_last_good_exposure_time_in_ms = 10.0;
	
	star_camera_desired_max_relative_camera_response = 0.9;	
	reflector_camera_desired_max_relative_camera_response = 0.9;

	pointer_to_star_camera = NULL;
	pointer_to_reflector_camera = NULL;

	menu_name = "sccan point handler";
	update_list_of_runs_in_current_directory();
	
	sccan_run_number = list_of_sccan_points_in_directory.size()+1;
	
	threshold_in_sigmas_for_star_detection = 8.0;
	star_recognition_one_sigma_radius_in_degrees = 0.05;
	max_number_of_exposure_manipulations = 25;
	
	exposure_range_is_good = true;
	
	update_list_of_star_positions();
}
//======================================================================
void sccan_point_pair_handler::
set_cameras(ueye_camera* star_camera,ueye_camera* reflector_camera){
	pointer_to_star_camera = star_camera;
	pointer_to_reflector_camera = reflector_camera;

	// only neccessary to figure out star camera resolution 
	double exposure = 10;
	sccan_image image_to_figure_out_image_size_of_star_camera;
	
	pointer_to_star_camera->acquire_image(&exposure);

	image_to_figure_out_image_size_of_star_camera = 
	pointer_to_star_camera->get_latest_image();
	
	// now overwrite image taken with star camera to zeros
	star_position_indicator_image.image_matrix = cv::Mat::zeros(
	image_to_figure_out_image_size_of_star_camera.image_matrix.rows,
	image_to_figure_out_image_size_of_star_camera.image_matrix.cols,
	CV_8UC3);
}
//======================================================================
void sccan_point_pair_handler::
acquire_sccan_points(int number_of_sccan_points_to_acquire){
	//==================================================================
	// check number of sccan points
	//==================================================================
	if(number_of_sccan_points_to_acquire<1){
		std::cout<<"sccan_point_pair_handler -> acquire_sccan_points() -> ";
		std::cout<<"number of sccan images to qcquire must not be below 1!";
		std::cout<<std::endl;
		return;
	}

	//==================================================================
	// check cameras
	//==================================================================
	if(pointer_to_star_camera==NULL||pointer_to_reflector_camera==NULL){
		std::cout<<"sccan_point_pair_handler -> acquire_sccan_points() -> ";
		std::cout<<"either one or both cameras have not been set yet!";
		std::cout<<std::endl;
		return;		
	}
	
	exposure_range_is_good = true;
	int sccan_point_itterator = 0;
	
	//==================================================================
	// create star_position display
	//==================================================================
	update_star_position_indicator_image();
	std::string name_of_star_position_display = "star position display";
	std::string name_of_status_display = "status display";
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"acquire_sccan_points -> open status display ";
		std::cout<<name_of_star_position_display<<std::endl;
	}
	// Create a window for star position display.
	cv::namedWindow(name_of_star_position_display.c_str(),
	CV_WINDOW_NORMAL);
	//CV_WINDOW_AUTOSIZE
	
	cv::namedWindow(name_of_status_display.c_str(),
	CV_WINDOW_AUTOSIZE);
	//CV_WINDOW_NORMAL
	
	cv::imshow(
	name_of_star_position_display.c_str(),
	star_position_indicator_image.image_matrix
	);
	//cv::waitKey(1);
	
	do{
		//==============================================================
		//SCCAN run number
		//==============================================================
		sccan_point_itterator++;
		
		if(verbosity){
		std::cout<<"sccan_point_pair_handler -> acquire_sccan_points() -> ";
		std::cout<<"take sccan point number "<<sccan_run_number;
		std::cout<<std::endl;
		}

		sccan_point_pair current_sccan_point = 
		acquire_single_sccan_point(&exposure_range_is_good);
		
		update_status_display(&current_sccan_point);
		
		cv::imshow(
		name_of_status_display.c_str(),
		status_display.image_matrix
		);
		
		cv::imshow(
		name_of_star_position_display.c_str(),
		star_position_indicator_image.image_matrix
		);
		
		cv::waitKey(1);		
		
		if(exposure_range_is_good){
			// save sccan_point
			current_sccan_point.save_sccan_point_pair();
			
			// if a star was found, add it to list of stars
			if(current_sccan_point.a_single_star_has_been_found()){
				list_of_star_positions_of_all_points_in_directory.
				push_back
				(current_sccan_point.get_point_of_star_in_star_image());
				update_star_position_indicator_image();
				
				if(verbosity){
					std::cout<<"sccan_point_pair_handler -> ";
					std::cout<<"acquire_sccan_points -> ";
					std::cout<<"update status dispaly ";
					std::cout<<name_of_star_position_display<<std::endl;
				}
			}
			
			if(verbosity){		
				std::cout<<"sccan_point_pair_handler -> ";
				std::cout<<"acquire_sccan_points() -> ";
				std::cout<<"sccan_point number "<<sccan_run_number;
				std::cout<<" has been acquired.";
				std::cout<<std::endl;
				std::cout<<"sccan_point_pair_handler -> exposure times: ";
				std::cout<<"star ";
				std::cout<<star_camera_last_good_exposure_time_in_ms;
				std::cout<<"ms ";
				std::cout<<"reflector ";
				std::cout<<reflector_camera_last_good_exposure_time_in_ms;
				std::cout<<"ms";
				std::cout<<std::endl;
			}
		}else{
			std::cout<<"sccan_point_pair_handler -> ";
			std::cout<<"acquire_sccan_points() -> ";
			std::cout<<"exposure range is bad!";
			std::cout<<std::endl;			
		}
		
	}while( 
	(sccan_point_itterator<number_of_sccan_points_to_acquire) &&
	exposure_range_is_good);
	//==================================================================
	// close star_position display
	//==================================================================	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"acquire_sccan_points -> close status display ";
		std::cout<<name_of_star_position_display<<std::endl;
	}
	// close window
	cv::destroyWindow(name_of_star_position_display.c_str());  
	cv::destroyWindow(name_of_status_display.c_str());  
}
//======================================================================
sccan_point_pair sccan_point_pair_handler::
acquire_single_sccan_point(
bool *pointer_to_exposure_range_is_good){
		
		sccan_point_pair current_sccan_point
		(sccan_run_number,verbosity);
		
		current_sccan_point.
		set_cameras(pointer_to_star_camera,pointer_to_reflector_camera);
		
		
		bool max_number_of_exposure_manipulations_has_been_exceeded 
		= false;
		
		// acquire sccan point
		current_sccan_point.acquire_sccan_images(
		&star_camera_last_good_exposure_time_in_ms,
		&reflector_camera_last_good_exposure_time_in_ms,
		star_camera_desired_max_relative_camera_response,	
		reflector_camera_desired_max_relative_camera_response,		
		max_number_of_exposure_manipulations,
		&max_number_of_exposure_manipulations_has_been_exceeded
		);
		
		if(max_number_of_exposure_manipulations_has_been_exceeded){
			std::cout<<"sccan_point_pair_handler -> ";
			std::cout<<"acquire_sccan_points() -> ";
			std::cout<<"desired exposure time can not be reached!"<<std::endl;
			*pointer_to_exposure_range_is_good = false;
		}else{
			sccan_run_number++;
		}
		
		// find bright star
		current_sccan_point.find_bright_star_in_star_image(
		threshold_in_sigmas_for_star_detection,
		star_recognition_one_sigma_radius_in_degrees
		);
		
	return current_sccan_point;
}
//======================================================================
std::string sccan_point_pair_handler::
get_prompt(){
	std::stringstream out, info;
	// sccan run number
	info.str("");
	info<<sccan_run_number;
	out<<make_nice_line_with_dots
	("| sccan run number",info.str());
	
	out<<"|"<<std::endl;
		
	// star camera exposure time
	info.str("");
	info<<star_camera_last_good_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| star camera exposure time",info.str());
	
	// star camera desired response
	info.str("");
	info<<star_camera_desired_max_relative_camera_response<<"[1]";
	out<<make_nice_line_with_dots
	("| star camera desired max rel. response",info.str());
	
	out<<"|"<<std::endl;
	
	// reflector camera exposure time
	info.str("");
	info<<reflector_camera_last_good_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| reflector camera exposure time",info.str());
	
	// reflector camera desired response
	info.str("");
	info<<reflector_camera_desired_max_relative_camera_response<<"[1]";
	out<<make_nice_line_with_dots
	("| reflector camera desired max rel. response",info.str());

	out<<"|"<<std::endl;
	
	// exposure range is good
	info.str("");
	if(exposure_range_is_good){info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| exposure range is good",info.str());
	
	out<<"|"<<std::endl;
	
	// number of sccan_points in current directory
	info.str("");
	info<<list_of_sccan_points_in_directory.size();
	out<<make_nice_line_with_dots
	("| number of sccan_points in current directory",info.str());
	
	out<<"|"<<std::endl;
	
	// threshold in sigmas for star detection
	info.str("");
	info<<threshold_in_sigmas_for_star_detection;
	out<<make_nice_line_with_dots
	("| threshold in sigmas for star detection",info.str());
	
	// star recognition radius in pixels
	info.str("");
	info<<star_recognition_one_sigma_radius_in_degrees<<"deg";
	out<<make_nice_line_with_dots
	("| star recognition radius one sigma",info.str());
	
	// max number of exposure itterations
	info.str("");
	info<<max_number_of_exposure_manipulations;
	out<<make_nice_line_with_dots
	("| max number of exposure itterations",info.str());
	
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
void sccan_point_pair_handler::
interaction(){
	
	std::string key_user_wants_to_take_more_sccan_points  ="m";
	std::string key_show_list_of_sccan_points_in_current_directory ="s";
	std::string key_reset_exposure_times ="r";
	std::string key_show_star_position_indicator_image ="i";
	std::string key_modify_sigma_threshold ="sigma";
	std::string key_modify_recognition_radius ="radius";
	std::string key_user_wants_to_end_taking_sccan_points  ="back";
	std::string key_modify_star_cam_desired_max_rel_respone  ="star_cr";	
	std::string key_modify_refl_cam_desired_max_rel_respone  ="refl_cr";
		
	add_control(key_user_wants_to_take_more_sccan_points,
	"aquire >N< additional sccan points");
	
	add_control
	(key_show_list_of_sccan_points_in_current_directory,
	"show list of sccan_points in current directory");

	add_control
	(key_show_star_position_indicator_image,
	"show star position indicator image");

	add_control
	(key_reset_exposure_times,
	"reset exposure times of cameras");
	
	add_control
	(key_modify_sigma_threshold,
	"modify sigma threshold");
	
	add_control
	(key_modify_recognition_radius,
	"modify recognition radius");	
	
	add_control
	(key_user_wants_to_end_taking_sccan_points,
	"back to main menu");

	add_control
	(key_modify_star_cam_desired_max_rel_respone,
	"modify star camera max desired rel. respone");

	add_control
	(key_modify_refl_cam_desired_max_rel_respone,
	"modify reflector camera max desired rel. respone");
	
	std::string user_input;
	
	std::stringstream out;
	
	bool flag_user_wants_to_acquire_sccan_points = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_list_of_runs_in_current_directory();
	update_status_prompt(get_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare
	(key_user_wants_to_end_taking_sccan_points)==0){
		flag_user_wants_to_acquire_sccan_points = false;
	}
	//==================================================================
	if(valid_user_input.compare
	(key_user_wants_to_take_more_sccan_points)==0){
		int number_of_sccan_points_to_be_acquired = ask_user_for_integer
		("Enter number of sccan points to be acquired",0,1e4);
		acquire_sccan_points(number_of_sccan_points_to_be_acquired);
	}
	//==================================================================
	if(valid_user_input.compare(
	key_show_list_of_sccan_points_in_current_directory)==0){
		disp_list_of_sccan_points_in_current_directory();
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_sigma_threshold)==0){
		threshold_in_sigmas_for_star_detection =
		ask_user_for_non_zero_float(
		"Enter threshold for star detection in sigmas of image noise",
		0.0,100.0);
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_recognition_radius)==0){
		star_recognition_one_sigma_radius_in_degrees =
		ask_user_for_non_zero_float
		("Enter new star recognition radius",0.0,180.0);
	}
	//==================================================================
	if(
	valid_user_input.compare(key_show_star_position_indicator_image)==0)
	{
		display_star_position_indicator_image();
	}
	//==================================================================
	if(valid_user_input.compare(key_reset_exposure_times)==0){
		reset_exposure_times();
	}	
	//==================================================================
	if(valid_user_input.compare(
	key_modify_star_cam_desired_max_rel_respone)==0){
		star_camera_desired_max_relative_camera_response = 
		ask_user_for_non_zero_float(
		"enter new relative star camera response (0,1)",0.01,0.99);
	}
	//==================================================================
	if(valid_user_input.compare(
	key_modify_refl_cam_desired_max_rel_respone)==0){
		reflector_camera_desired_max_relative_camera_response = 
		ask_user_for_non_zero_float(
		"enter new relative reflector camera response (0,1)",0.01,0.99);
	}
	//==================================================================
}while(flag_user_wants_to_acquire_sccan_points);	
}
//======================================================================
void sccan_point_pair_handler::
update_list_of_runs_in_current_directory(){
	
	list_of_sccan_points_in_directory.clear();
	// create list of all possible files in directory
	std::vector<std::string> list_of_all_files_in_directory = 
	get_list_of_files_in_directory("");
	
	for(int file_itterator=0;
	file_itterator<list_of_all_files_in_directory.size();
	file_itterator++){
	
		if(
		is_string_a_sccan_run_config(
		list_of_all_files_in_directory.at(file_itterator)
		)
		){
			// erase the file ending
			std::string filename_to_add =
			list_of_all_files_in_directory.at(file_itterator).
			substr(
			0,
			list_of_all_files_in_directory.at(file_itterator).
			find(".",0)
			);
			
			if(verbosity){
				std::cout<<"sccan_point_pair_handler -> ";
				std::cout<<"update_list_of_runs_in_current_directory() -> ";
				std::cout<<"sccan point: "<<filename_to_add<<std::endl;
			}
			list_of_sccan_points_in_directory.push_back
			(filename_to_add);
		}
	}
}
//======================================================================
void sccan_point_pair_handler::
disp_list_of_sccan_points_in_current_directory(){
	std::stringstream out;
	out<<"list of "<<list_of_sccan_points_in_directory.size();
	out<<" sccan_points in current directory:"<<std::endl;

	for(int sccan_point_itterator=0;
	sccan_point_itterator<list_of_sccan_points_in_directory.size();
	sccan_point_itterator++){
	out<<list_of_sccan_points_in_directory.at(sccan_point_itterator);
	out<<std::endl;
	}
	std::cout<<out.str();
}
//======================================================================
bool sccan_point_pair_handler::
is_string_a_sccan_run_config(std::string line){
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"is_string_a_sccan_run_config() -> line: ";
		std::cout<<line<<std::endl;
	}
	
	// use DEAs for the std::string control
	// find 

	enum TokenT{ 
		digit,
		point,
		underscore,
		char_c,
		char_f,
		char_g,
		char_s,
		char_a,
		char_n,
		ERR
	};
		   
	int GetState[][23] ={
	//dig point under	c	f	g	s	a	n
	{  1,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//0		d
	{  2,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//1		d
	{  3,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//2		d
	{  4,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//3		d
	{  5,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//4		d
	{  6,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//5		d
	{  7,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//6		d
	{  8,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//7		d
	{  -1,	-1,  9,		-1,	-1,	-1, -1,	-1,	-1 },	//8		_
	{  10,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//9		d
	{  11,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//10	d
	{  12,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//11	d
	{  13,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//12	d
	{  -1,	-1,	14,		-1,	-1,	-1, -1,	-1,	-1 },	//13	_	
	{  -1,	-1,	-1,		-1,	-1,	-1, 15,	-1,	-1 },	//14	s
	{  -1,	-1,	-1,		16,	-1,	-1, -1,	-1,	-1 },	//15	c
	{  -1,	-1,	-1,		17,	-1,	-1, -1,	-1,	-1 },	//16	c
	{  -1,	-1,	-1,		-1,	-1,	-1, -1,	18,	-1 },	//17	a
	{  -1,	-1,	-1,		-1,	-1,	-1, -1,	-1,	19 },	//18	n
	{  -1,	-1,	20,		-1,	-1,	-1, -1,	-1,	-1 },	//19	_	
	{  20,	21,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//20	d	
	{  -1,	-1,	-1,		22,	-1,	-1, -1,	-1,	-1 },	//21	.
	{  -1,	-1,	-1,		-1,	23,	-1, -1,	-1,	-1 },	//22	c
	{  -1,	-1,	-1,		-1,	-1,	24, -1,	-1,	-1 },	//23	f
													//24	g
	};
	//for( int i=0; i<11;i++){std::cout<<"("<<i<<")"<<GetState[26][i]<<", ";}
	//std::cout<<std::endl;
	std::string ID;
	std::string tilt_x;
	std::string tilt_y;
	std::string tilt_z;
	std::string str_vec2D_x;
	std::string str_vec2D_y;
	int int_vec2D_count = 0;
	
	
	int state = 0;
	int char_number = 0;
	//std::cout<<"str laenge: "<<line.size()<<std::endl;
	while(state != -1 && char_number<line.size()){
		TokenT token = ERR;
		char s = line.at( char_number);
		if(is_c(s)) token = char_c;
		if(is_f(s)) token = char_f;
		if(is_g(s)) token = char_g;
		if(is_s(s)) token = char_s;
		if(is_a(s)) token = char_a;
		if(is_n(s)) token = char_n;
		if(isdigit(s)) token = digit; 
		//std::cout<<char(s)<<"==digit"<<std::endl;}
		if(is_point(s)) token = point;
		if(is_underscore(s)) token = underscore;
		//std::cout<<"check"<<char_number<<": ";
		//std::cout<<char(s)<<" state: "<<state<<" token: "<<token<<std::endl;
		state = (token == ERR) ? :GetState[state][token];
		char_number ++; 	
	
	}
	if(state==24){return true;}else{return false;};
}
//======================================================================
void sccan_point_pair_handler::
update_star_position_indicator_image(){
	//overwrite image to zeros
	star_position_indicator_image.image_matrix = cv::Mat::zeros(
	star_position_indicator_image.image_matrix.rows,
	star_position_indicator_image.image_matrix.cols,
	CV_8UC3);
	
	star_position_indicator_image = 
	star_position_indicator_image.get_overlay_with_stars(
	list_of_star_positions_of_all_points_in_directory,
	cv::Scalar(255,128,128)); //BGR
}
//======================================================================
void sccan_point_pair_handler::
display_star_position_indicator_image(){
	update_star_position_indicator_image();
	star_position_indicator_image.disp_simple_image
	("star position indicator image");
}
//======================================================================
void sccan_point_pair_handler::
update_list_of_star_positions(){
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_list_of_star_positions() -> ";
		std::cout<<"start";
		std::cout<<std::endl;
	}
	
	update_list_of_runs_in_current_directory();
	
	list_of_star_positions_of_all_points_in_directory.clear();
	
	for(int sccan_point_itterator = 0;
	sccan_point_itterator<list_of_sccan_points_in_directory.size();
	sccan_point_itterator++){
		
		sccan_point_pair sccan_point_to_check(0,verbosity);
		sccan_point_to_check.load_sccan_point_configuration(
		list_of_sccan_points_in_directory.at(sccan_point_itterator)
		);
		
		if(sccan_point_to_check.a_single_star_has_been_found()){
			list_of_star_positions_of_all_points_in_directory.
			push_back(
			sccan_point_to_check.get_point_of_star_in_star_image()
			);
		}
	}
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_list_of_star_positions() -> ";
		std::cout<<list_of_star_positions_of_all_points_in_directory.size();
		std::cout<<" star positions found in ";
		std::cout<<list_of_sccan_points_in_directory.size();
		std::cout<<" sccan_points in current directory.";
		std::cout<<std::endl;
	}
	
}
//======================================================================
void sccan_point_pair_handler::
reset_exposure_times(){
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"reset_exposure_times() -> ";
		std::cout<<"set both exposure times to 10.0ms";
		std::cout<<std::endl;
	}
	star_camera_last_good_exposure_time_in_ms = 100.0;
	reflector_camera_last_good_exposure_time_in_ms = 100.0;
}
//======================================================================
void sccan_point_pair_handler::
update_status_display
(sccan_point_pair *pointer_to_current_sccan_point){
	
	int number_of_lines_of_status_display_for_sccan_point_taking = 480;
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"start.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get star camera image
	//==================================================================
	
	simple_image resized_star_image_with_overlay = 
	pointer_to_current_sccan_point->get_resized_star_image_with_overlay
	(number_of_lines_of_status_display_for_sccan_point_taking);
	
	if(pointer_to_current_sccan_point->a_single_star_has_been_found()){
		// overlay with star direction
		std::stringstream star_direction_prompt;
		
		star_direction_prompt<<"x ";
		star_direction_prompt<<pointer_to_current_sccan_point->
		sccan_star.star_direction_relative_to_camera.
		get_x_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.1,0.9,0.25);
		
		star_direction_prompt.str("");
		star_direction_prompt<<"y ";
		star_direction_prompt<<pointer_to_current_sccan_point->
		sccan_star.star_direction_relative_to_camera.
		get_y_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.5,0.9,0.25);
	}

	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"got resized star image with overlay.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get reflector camera image
	//==================================================================

	simple_image resized_reflector_image_with_overlay =
	pointer_to_current_sccan_point->
	get_resized_reflector_image(
	number_of_lines_of_status_display_for_sccan_point_taking);
	
	pointer_to_current_sccan_point->
	get_reflector_image_exposure_time_stamp(
	&resized_reflector_image_with_overlay);
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"got resized reflector image with overlay.";
		std::cout<<std::endl;
	}	
	
	//==================================================================
	//concatenate matrices
	//==================================================================	
	
	status_display.horizontal_concatenate_with_delimiter_line(
	&resized_star_image_with_overlay,
	&resized_reflector_image_with_overlay);
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"end.";
		std::cout<<std::endl;
	}
}
//======================================================================
int sccan_point_pair_handler::
get_number_of_sccan_points_in_current_directory(){
	return list_of_sccan_points_in_directory.size();
}
//======================================================================
std::string sccan_point_pair_handler::
get_sccan_point_pair_name_at(int position_of_sccan_point_pair_name){
	if(position_of_sccan_point_pair_name>
	list_of_sccan_points_in_directory.size()){
		return "";
	}else{
		return list_of_sccan_points_in_directory.
		at(position_of_sccan_point_pair_name);
	}
}
//======================================================================
void sccan_point_pair_handler::
toggle_verbosity(){
	verbosity = !verbosity;
	
	list_of_star_positions_of_all_points_in_directory.
	toggle_verbosity(verbosity);
	
	star_position_indicator_image.
	toggle_verbosity(verbosity);
	
	status_display.
	toggle_verbosity(verbosity);
	
}
//======================================================================
bool sccan_point_pair_handler::is_c(char c){return c=='c';}
bool sccan_point_pair_handler::is_f(char c){return c=='f';}
bool sccan_point_pair_handler::is_g(char c){return c=='g';}
bool sccan_point_pair_handler::is_s(char c){return c=='s';}
bool sccan_point_pair_handler::is_a(char c){return c=='a';}
bool sccan_point_pair_handler::is_n(char c){return c=='n';}
bool sccan_point_pair_handler::is_point(char c){return c=='.';}
bool sccan_point_pair_handler::is_underscore(char c){return c=='_';}
bool sccan_point_pair_handler::is_end_of_line(char c){return c=='\0';}
