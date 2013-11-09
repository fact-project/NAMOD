// minimal ueye example
// basic include
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
// time
#include <time.h>

// include for configuration file
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

// look up directorys
#include <dirent.h>

// open CV
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

// programm includes
//#include "global_variables.h"
#include "programm_verbosity.h"
#include "user_interaction.h"
#include "time_stamp_manager.h"
#include "list_of_pixel_positions.h"
#include "pointing_direction.h"
#include "star_in_image.h"
#include "intrinsic_camera_parameter.h"
#include "simple_image.h"
#include "sccan_image.h"
#include "ueye_camera.h"
#include "sccan_analysis_point.h"
#include "sccan_point_pair.h"
#include "mirror.h"
#include "reflector.h"

// matplotpp
//#include "matplotpp.h"
#include "Python.h"

using namespace libconfig;
// pre decleration
class mirror;

//======================================================================
class snapshot 					:public user_interaction{
public:
	bool 		snapshot_verbosity;
	uint 		snapshot_counter;
	bool 		flag_long_time_exposure;
	double 		snapshot_exposure_time_in_ms;
	std::string		snapshot_file_name;
	ueye_camera *pointer_to_snapshot_camera;
	std::vector<ueye_camera*> list_of_pointers_to_cameras;
//======================================================================
snapshot(){
	initialize();
	menu_name = "snapshot";
	//add_camera(NULL);
}
//======================================================================
snapshot(ueye_camera* ptr_to_snapshot_cam){
	add_camera(ptr_to_snapshot_cam);
	initialize();
}
//======================================================================
void initialize(){
	snapshot_verbosity = true;

	snapshot_counter = 1;
	snapshot_exposure_time_in_ms = 10;
	create_snapshot_file_name();
	flag_long_time_exposure = false;
	if(list_of_pointers_to_cameras.size()>0){
		pointer_to_snapshot_camera = 
		list_of_pointers_to_cameras.at(0);
	}
	
	if(snapshot_verbosity){
		std::cout<<"snapshot ->";
		std::cout<<"snapshot() ->";
		std::cout<<"initialize exp. time ";
		std::cout<<snapshot_exposure_time_in_ms<<"ms, ";
		std::cout<<"number of cameras ";
		std::cout<<list_of_pointers_to_cameras.size()<<std::endl;
	}
}
//======================================================================
void add_camera(ueye_camera* ptr_to_snapshot_cam){
	if(choose_camera(ptr_to_snapshot_cam->get_camera_ID())){
		//camera already in list
		std::cout<<"snapshot -> add_camera() -> camera ID ";
		std::cout<<ptr_to_snapshot_cam->get_camera_ID();
		std::cout<<" already in list!"<<std::endl;
	}else{
		// a new camera
		list_of_pointers_to_cameras.push_back(ptr_to_snapshot_cam);
		if(list_of_pointers_to_cameras.size()==1)
		{
			pointer_to_snapshot_camera = 
			list_of_pointers_to_cameras.at(0);
		}
	}
}
//======================================================================
void interaction(){

	std::string key_user_wants_to_acquire_image ="y";
	add_control(key_user_wants_to_acquire_image,"take a snapshot");
	
	std::string key_modify_exposure_time ="e";	
	add_control(key_modify_exposure_time,"manipulate exposure time");
	
	std::string key_modify_long_time_exposure ="l";	
	add_control(key_modify_long_time_exposure,
	"toggle long time exposure");
	
	std::string key_modify_camera_ID ="c";	
	add_control(key_modify_camera_ID,"choose camera ID");
	
	std::string key_take_exposure_time_series ="eS";
	add_control(key_take_exposure_time_series,
	"take exposure time series");
	
	std::string key_spectate_stars = "s";
	add_control(key_spectate_stars,"spectate stars");
	
	std::string key_user_wants_to_end_snapshot  ="back";
	add_control(key_user_wants_to_end_snapshot ,"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_interaction = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_snapshot_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();	
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_end_snapshot)==0){
		flag_user_wants_to_interaction = false;
	}
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_acquire_image)==0){	
		//==========================================================
		// chose to take another interaction_with_ueye_camera
		//==========================================================
		// increase snapshot counter for snapshot number in filename
		
		if(pointer_to_snapshot_camera->
		acquire_image(&snapshot_exposure_time_in_ms)){
			// sccan_image acquisition was ok
			pointer_to_snapshot_camera->
			export_latest_image(snapshot_file_name);
			
			pointer_to_snapshot_camera->disp_latest_image();
		}else{
			// sccan_image acquisition failed
		}
		snapshot_counter++;
		create_snapshot_file_name();
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_exposure_time)==0){
		//==========================================================
		// user chose to adjust exposure time
		//==========================================================
		snapshot_exposure_time_in_ms = ask_user_for_non_zero_float
		("Enter new exposure time [ms]",0.001,60000);
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_long_time_exposure)==0){
		if(flag_long_time_exposure==true){
			flag_long_time_exposure = false;
		}else{
			flag_long_time_exposure = true;
		}	
		pointer_to_snapshot_camera->
		long_time_exposure(flag_long_time_exposure);
	}	
	//==================================================================
	if(valid_user_input.compare(key_take_exposure_time_series)==0){

		int user_input_of_number_of_exposure_time_itterations =
		ask_user_for_integer(
		"Enter number of exposure time itterations",
		0,100);

		take_exposure_time_series(
		user_input_of_number_of_exposure_time_itterations);
	
	}
	//==================================================================	
	if(valid_user_input.compare(key_spectate_stars)==0){

		double star_detection_threshold_in_sigmas = 
		ask_user_for_non_zero_float(
		"Enter star detection threshold in sigmas",
		0.1,
		1000.0
		);
		
		double star_detection_radius_in_degrees = 
		ask_user_for_non_zero_float(
		"Enter star detection radius in degrees",
		0.001,
		180.0
		);
		
		spectate_stars(
		star_detection_threshold_in_sigmas,
		star_detection_radius_in_degrees
		);
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_camera_ID)==0){
		
		std::vector<int> list_of_valid_camera_IDs;
		for(int cam_itt=0;
		cam_itt<list_of_pointers_to_cameras.size();
		cam_itt++)
		{
			list_of_valid_camera_IDs.push_back(
			list_of_pointers_to_cameras.at(cam_itt)->get_camera_ID()
			);
		}
		uint user_chosen_camera_ID =ask_user_for_integer
		("Enter camera ID",&list_of_valid_camera_IDs);
		choose_camera(user_chosen_camera_ID);
		/*
		bool flag_user_input_camera_ID_is_valid = false;
		uint user_chosen_camera_ID;
		do{
			user_chosen_camera_ID = 
			(uint)ask_user_for_integer("Enter camera ID",0,256);
			
			if(choose_camera(user_chosen_camera_ID))
			{
				//camera was found
				flag_user_input_camera_ID_is_valid = true;
			}else{
				//camera is not in list
				out.str("");
				out<<"->"<<user_chosen_camera_ID<<"<- ";
				out<<"is not in camera ID list!"<<std::endl;
				std::cout<<out.str();	
			}
		}while( !flag_user_input_camera_ID_is_valid  );
		*/
	}
	//==================================================================

}while(flag_user_wants_to_interaction);
}
//======================================================================
void create_snapshot_file_name(){
	std::stringstream snapshot_file_name_stream;
	snapshot_file_name_stream<<global_time_stamp_manager_instance.
	get_current_time_stamp();
	snapshot_file_name_stream<<"_snapshot";
	snapshot_file_name_stream<<"_"<<snapshot_counter;
	snapshot_file_name = snapshot_file_name_stream.str();
}
//======================================================================
std::string get_snapshot_prompt(){
	int sys_return =  system("clear");
	std::stringstream out, info;
	
	out<<"| cameras (IDs): ";
	for(int camera_itterator=0;
	camera_itterator<list_of_pointers_to_cameras.size();
	camera_itterator++){
	out<<
	list_of_pointers_to_cameras.at(camera_itterator)->
	get_camera_ID()<<", ";
	}
	out<<std::endl;
	
	// current camera ID
	info.str("");
	info<<pointer_to_snapshot_camera->get_camera_ID();
	out<<make_nice_line_with_dots("| current camera ID",info.str());
	
	out<<"|"<<std::endl;
	
	// snapshot counter
	info.str("");
	info<<snapshot_counter;
	out<<make_nice_line_with_dots("| snapshot counter",info.str());

	// next snapshot
	info.str("");
	info<<snapshot_file_name;
	out<<make_nice_line_with_dots("| next snapshot",info.str());

	// next desired exposure time
	info.str("");
	info<<snapshot_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| next desired exposure time",info.str());

	// long time exposure feature
	info.str("");
	if(flag_long_time_exposure){info<<"active";}else{info<<"inactive";}
	out<<make_nice_line_with_dots
	("| long time exposure feature",info.str());
	
	// horizontal line 
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str(); 	
}
//======================================================================
bool choose_camera(uint camera_ID_to_use){
	bool camera_found = false;
	
	for(int camera_itterator=0;
	camera_itterator<list_of_pointers_to_cameras.size();
	camera_itterator++)
	{
		if(	list_of_pointers_to_cameras.at(camera_itterator)->
		get_camera_ID() == camera_ID_to_use)
		{
			camera_found = true;
			pointer_to_snapshot_camera = 
			list_of_pointers_to_cameras.at(camera_itterator);
		}
	}
	return camera_found;	
}
//======================================================================
void toggle_verbosity(){
	snapshot_verbosity = !snapshot_verbosity;
}
//======================================================================
bool verbosity_status(){
	return snapshot_verbosity;
}
//======================================================================
void take_exposure_time_series(int number_of_exposure_itterations){
	
	if(verbosity){
		std::cout<<"snapshot -> ";
		std::cout<<"take_exposure_time_series() -> taking ";
		std::cout<<number_of_exposure_itterations<<" images.";
		std::cout<<std::endl;	
	}
	
	if(number_of_exposure_itterations<=0)
	return;
	
	std::stringstream exposure_series_protocoll;
	
	exposure_series_protocoll<<" ____exposure_time_series_______"<<std::endl;
	
	for(
	int snapshot_itterator = 0;
	snapshot_itterator<number_of_exposure_itterations;
	snapshot_itterator++){
	
		double exposure_time_in_ms_for_specific_snapshot = 
		snapshot_exposure_time_in_ms*
		pow(2.0,(double)snapshot_itterator);
		
		if(verbosity){
			std::cout<<"snapshot -> ";
			std::cout<<"take_exposure_time_series() -> ";
			std::cout<<"exposure time ";
			std::cout<<exposure_time_in_ms_for_specific_snapshot<<"ms";
			std::cout<<std::endl;	
		}	
		
	
		pointer_to_snapshot_camera->
		acquire_image(&exposure_time_in_ms_for_specific_snapshot);
		
		simple_image snapshot_image_for_specific_exposure_time =
		pointer_to_snapshot_camera->get_latest_image();
		
		std::stringstream window_name_for_specific_snapshot;
		
		if(verbosity){
			window_name_for_specific_snapshot<<
			"snapshot, exposure time ";
			window_name_for_specific_snapshot<<
			exposure_time_in_ms_for_specific_snapshot;
			window_name_for_specific_snapshot<<"ms";
			/*
			snapshot_image_for_specific_exposure_time.
			disp_simple_image(
			window_name_for_specific_snapshot.str());
			*/
		}
		
		cv::Scalar mean_pixel_value;
		
		mean_pixel_value = cv::mean(
		snapshot_image_for_specific_exposure_time.image_matrix
		);
		
	exposure_series_protocoll<<"| "<<snapshot_itterator<<")\t";
	
	bool expose_longer;
	if(snapshot_image_for_specific_exposure_time.
	check_exposure_time(0.0,255,&expose_longer)){
		exposure_series_protocoll.precision(3);
		exposure_series_protocoll<<
		exposure_time_in_ms_for_specific_snapshot<<"ms\t";
		
		//exposure_series_protocoll<<"R "<<mean_pixel_value[2]<<"\t";
		//exposure_series_protocoll<<"G "<<mean_pixel_value[1]<<"\t";
		//exposure_series_protocoll<<"B "<<mean_pixel_value[0]<<"\t";
		exposure_series_protocoll<<"Mean "<<
		(double)
		(mean_pixel_value[2]+mean_pixel_value[1]+mean_pixel_value[0])/
		3.0;
		
	}else{
		exposure_series_protocoll<<"overflow";
	}
	
		if(verbosity){
			std::cout<<"snapshot -> ";
			std::cout<<"take_exposure_time_series() -> ";
			std::cout<<"Mean \tR "<<mean_pixel_value[2]<<" ";
			std::cout<<"\tG "<<mean_pixel_value[1]<<" ";
			std::cout<<"\tB "<<mean_pixel_value[0]<<" ";
			std::cout<<std::endl;
		}
		exposure_series_protocoll<<std::endl;
	}
	
	exposure_series_protocoll<<"|_______________________________"<<std::endl;
	
	std::cout<<exposure_series_protocoll.str();
}
//======================================================================
void spectate_stars(double threshold_in_sigmas_for_star_detection,
double radius_in_deg_of_one_sigma_region_for_star_detection){
	
	pointer_to_snapshot_camera->
	acquire_image(&snapshot_exposure_time_in_ms);
	
	sccan_image snapshot_image_to_spectate_stars =
	pointer_to_snapshot_camera->get_latest_image();
	
	std::vector<star_in_image> list_of_all_stars_found_in_snapshot = 
	snapshot_image_to_spectate_stars.find_stars(
	threshold_in_sigmas_for_star_detection,
	radius_in_deg_of_one_sigma_region_for_star_detection
	);
	
	if(verbosity){
		std::cout<<"snapshot -> ";
		std::cout<<"spectate_stars() -> ";
		std::cout<<list_of_all_stars_found_in_snapshot.size()<<" stars";
		std::cout<<"have been found.";
		std::cout<<std::endl;
	}
	
	simple_image stars_with_overlay = snapshot_image_to_spectate_stars.
	get_overlay_with_stars(
	list_of_all_stars_found_in_snapshot,
	cv::Scalar(255,128,128)
	);
	
	stars_with_overlay.disp_simple_image("star snapshot");
}
//======================================================================
};
//======================================================================
class sccan_point_pair_handler 	:public user_interaction{
	public:
	//std::vector<sccan_point_pair> list_of_sccan_point_pairs;
	
	double star_camera_last_good_exposure_time_in_ms;
	double reflector_camera_last_good_exposure_time_in_ms;
	
	ueye_camera 	*pointer_to_star_camera;
	ueye_camera 	*pointer_to_reflector_camera;
	bool			exposure_range_is_good;
	int 			sccan_run_number;
	//bool			verbosity_sccan_handler;

	double 	threshold_in_sigmas_for_star_detection;
	double 	star_recognition_one_sigma_radius_in_degrees;
	int		max_number_of_exposure_manipulations;
	
	// status
	simple_image 	star_position_indicator_image;
	simple_image	status_display;

	list_of_pixel_positions 	list_of_star_positions_of_all_points_in_directory;
	
	std::vector<std::string>	list_of_sccan_points_in_directory;
//======================================================================
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
	//list_of_sccan_point_pairs.clear();
	
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
void set_cameras(ueye_camera* star_camera,ueye_camera* reflector_camera){
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
void acquire_sccan_points(int number_of_sccan_points_to_acquire){
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
sccan_point_pair acquire_single_sccan_point(
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
std::string get_prompt(){
	std::stringstream out, info;
	// sccan run number
	info.str("");
	info<<sccan_run_number;
	out<<make_nice_line_with_dots
	("| sccan run number",info.str());
	
	// star camera exposure time
	info.str("");
	info<<star_camera_last_good_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| star camera exposure time",info.str());
	
	// reflector camera exposure time
	info.str("");
	info<<reflector_camera_last_good_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| reflector camera exposure time",info.str());

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
void interaction(){
	
	std::string key_user_wants_to_take_more_sccan_points  ="m";
	std::string key_show_list_of_sccan_points_in_current_directory ="s";
	std::string key_reset_exposure_times ="r";
	std::string key_show_star_position_indicator_image ="i";
	std::string key_modify_sigma_threshold ="sigma";
	std::string key_modify_recognition_radius ="radius";
	std::string key_user_wants_to_end_taking_sccan_points  ="back";
	
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
	valid_user_input.compare(key_show_star_position_indicator_image)
	==0)
	{
		display_star_position_indicator_image();
	}
	//==================================================================
	if(valid_user_input.compare(key_reset_exposure_times)==0){
		reset_exposure_times();
	}	
	//==================================================================
}while(flag_user_wants_to_acquire_sccan_points);	
}
//======================================================================
void update_list_of_runs_in_current_directory(){
	
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
void disp_list_of_sccan_points_in_current_directory(){
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
bool is_string_a_sccan_run_config(std::string line){
	
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
void update_star_position_indicator_image(){
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
void display_star_position_indicator_image(){
	update_star_position_indicator_image();
	star_position_indicator_image.disp_simple_image
	("star position indicator image");
}
//======================================================================
void update_list_of_star_positions(){
	
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
void reset_exposure_times(){
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
void update_status_display
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
int get_number_of_sccan_points_in_current_directory(){
	return list_of_sccan_points_in_directory.size();
}
//======================================================================
std::string get_sccan_point_pair_name_at(
int position_of_sccan_point_pair_name){
	if(position_of_sccan_point_pair_name>
	list_of_sccan_points_in_directory.size()){
		return "";
	}else{
		return list_of_sccan_points_in_directory.
		at(position_of_sccan_point_pair_name);
	}
}
//======================================================================
void toggle_verbosity(){
	verbosity = !verbosity;
	
	list_of_star_positions_of_all_points_in_directory.
	toggle_verbosity(verbosity);
	
	star_position_indicator_image.
	toggle_verbosity(verbosity);
	
	status_display.
	toggle_verbosity(verbosity);
	
}
//======================================================================
private:
bool is_c(char c){return c=='c';}
bool is_f(char c){return c=='f';}
bool is_g(char c){return c=='g';}
bool is_s(char c){return c=='s';}
bool is_a(char c){return c=='a';}
bool is_n(char c){return c=='n';}
bool is_point(char c){return c=='.';}
bool is_underscore(char c){return c=='_';}
bool is_end_of_line(char c){return c=='\0';}
};
//======================================================================
class quick_align 				:public user_interaction{
public:
reflector* pointer_to_reflector;
sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;

std::string window_name_of_alignment_image;
simple_image alignment_image;

int height_of_alignment_image;
//======================================================================
quick_align(
reflector* new_pointer_to_reflector,
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler){
	//quick_align_verbosity = true;
	if(verbosity){
		std::cout<<"quick_align -> quick_align()"<<std::endl;
	}
	
	
	menu_name = "quick single mirror alignment";
	
	pointer_to_reflector = 
	new_pointer_to_reflector;
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	
	window_name_of_alignment_image = "quick alignment display";
	height_of_alignment_image = 480;
	alignment_image.toggle_verbosity(verbosity);
}
//======================================================================
void start_alignment(){
	if(verbosity){
		std::cout<<"quick_align -> start_alignment() -> start"<<std::endl;
	}	
	
	// check whether there is a mirror to work with or not
	if(pointer_to_reflector->is_there_a_mirror_to_work_with()){
		if(verbosity){
			std::cout<<"quick_align -> start_alignment() -> ";
			std::cout<<"working on mirror ID ";
			std::cout<<pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->get_mirror_ID();
			std::cout<<std::endl;
		}
		// run alignment
		run_alignment();
	}else{
		if(verbosity){
			std::cout<<"quick_align -> start_alignment() -> ";
			std::cout<<"there is no mirror set to work with!"<<std::endl;
		}		
	}
}
//======================================================================
void run_alignment(){
	display_help();
	
	int time_until_images_are_updated_in_ms = 1;
	int key_pressed_by_user=0;
	
	
	cv::namedWindow(
	window_name_of_alignment_image.c_str(),
	CV_WINDOW_AUTOSIZE 
	);
	
	while(key_pressed_by_user!=27) //27 is ESC key
	{
		update_alignment_window();
		
		key_pressed_by_user =
		cvWaitKey(time_until_images_are_updated_in_ms);

		switch(key_pressed_by_user){
			//~ case 'w': move_forward();
			//~ break;
			//~ case 's': move_backward();
			//~ break;
			//~ case 'a': move_left();
			//~ break;
			//~ case 'd': move_right();
			//~ break;
			//~ case '+': increase_FoV();
			//~ break;
			//~ case '-': decrease_FoV();
			//~ break;
			//~ case 'i' : look_up();
			//~ break;
			//~ case 'k' : look_down();
			//~ break;
			//~ case 'j' : look_left();
			//~ break;
			//~ case 'l' : look_right();
			//~ break;
			case 'h' : {
				display_help();
			};
			break;
			default: int i=1;
		}
	}
	cv::destroyWindow(window_name_of_alignment_image.c_str());
}
//======================================================================
void display_help(){
	std::cout<<"quick_align -> start_alignment() -> ";
	std::cout<<"help"<<std::endl;
}
//======================================================================
void update_alignment_window(){
	
	acquire_sccan_point_for_quick_alignment();
	
	cv::imshow(window_name_of_alignment_image.c_str(),
	alignment_image.image_matrix); 
}
//======================================================================
void acquire_sccan_point_for_quick_alignment(){

	bool exposure_range_is_good = true;
	sccan_point_pair current_sccan_point = 
	pointer_to_sccan_point_pair_handler->
	acquire_single_sccan_point(&exposure_range_is_good);
	
	update_alignment_window(current_sccan_point);
	
	if(exposure_range_is_good){
		if(verbosity){
			std::cout<<"quick_align -> ";
			std::cout<<"acquire_sccan_point_for_quick_alignment() -> ";
			std::cout<<"sccan point acquiered, exposure range is good.";
			std::cout<<std::endl;
		}	
	}else{
		std::cout<<"quick_align -> ";
		std::cout<<"acquire_sccan_point_for_quick_alignment() -> ";
		std::cout<<"sccan point acquiered, exposure range is bad!";
		std::cout<<std::endl;
	}
}
//======================================================================
void update_alignment_window(sccan_point_pair current_sccan_point){

	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"start.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get star camera image
	//==================================================================
	
	simple_image resized_star_image_with_overlay = 
	current_sccan_point.get_resized_star_image_with_overlay
	(height_of_alignment_image);
	resized_star_image_with_overlay.toggle_verbosity(verbosity);

	if(current_sccan_point.a_single_star_has_been_found()){
		// overlay with star direction
		std::stringstream star_direction_prompt;
		
		star_direction_prompt<<"x ";
		star_direction_prompt<<current_sccan_point.sccan_star.
		star_direction_relative_to_camera.
		get_x_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.1,0.9,0.25);
		
		star_direction_prompt.str("");
		star_direction_prompt<<"y ";
		star_direction_prompt<<current_sccan_point.sccan_star.
		star_direction_relative_to_camera.
		get_y_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.5,0.9,0.25);
	}
	
	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"got resized star image with overlay.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get reflector camera image
	//==================================================================
	
	simple_image resized_reflector_image_with_overlay;
	resized_reflector_image_with_overlay.toggle_verbosity(verbosity);
	
	// calculate region of interest for specific mirror ID
	if(pointer_to_reflector->pointer_to_current_mirror_to_work_with 
	!= NULL){
		if(verbosity){
			std::cout<<"quick_align -> ";
			std::cout<<"update_alignment_window() -> ";
			std::cout<<"region of interest for ";	
			std::cout<<"mirror ID ";
			std::cout<<pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->get_mirror_ID();
			std::cout<<std::endl;	
		}	
			
			simple_image roi = 
			pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->
			highlight_mirror_in_reflector_image
			(&current_sccan_point.reflector_image);
			roi.toggle_verbosity(verbosity);
			
			double scaling = 1.0;
			
			resized_reflector_image_with_overlay=
			roi.get_resized_image(height_of_alignment_image,&scaling);
			
			// overlay with specific mirror ID
			std::stringstream mirror_ID_prompt_in_image;
			mirror_ID_prompt_in_image<<"mirror ID ";
			mirror_ID_prompt_in_image<<pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->get_mirror_ID();
			
			resized_reflector_image_with_overlay.text_overlay(
			mirror_ID_prompt_in_image.str(),0.1,0.15,1.0);
			
			// overlay with mean light flux
			double mean_light_flux_in_bulbs_of_mirror = 				
			current_sccan_point.reflector_image.
			mean_light_flux_in_bulbs_of_complex_image_region(
			pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->
			get_pointer_to_list_of_Points_inside_mirror_polygon());
			
			std::stringstream light_flux_mirror_promot;
			light_flux_mirror_promot.precision(3);
			light_flux_mirror_promot<<"light flux mirror: ";
			light_flux_mirror_promot<<
			mean_light_flux_in_bulbs_of_mirror<<" bulbs";
			
			resized_reflector_image_with_overlay.text_overlay(
			light_flux_mirror_promot.str(),0.1,0.2,1.0);	
			
			// overlay normalized light flux
			if(current_sccan_point.a_single_star_has_been_found()){
				
				std::stringstream normalized_light_flux_mirror_promot;
				normalized_light_flux_mirror_promot.precision(3);
				normalized_light_flux_mirror_promot<<
				"normalized light flux: "<<
				mean_light_flux_in_bulbs_of_mirror/
				current_sccan_point.sccan_star.
				light_flux_of_star_in_bulbs<<" [1]";

				resized_reflector_image_with_overlay.text_overlay(
				normalized_light_flux_mirror_promot.str(),0.1,0.25,1.0);	
			}	
			
	}else{
		if(verbosity){
			std::cout<<"quick_align -> ";
			std::cout<<"update_alignment_window() -> ";	
			std::cout<<"mirror ID was not set yet!";
			std::cout<<std::endl;	
		}
		resized_reflector_image_with_overlay=
		current_sccan_point.get_resized_reflector_image(
		height_of_alignment_image);
		
		resized_reflector_image_with_overlay.text_overlay(
		"mirror ID has not been set!",0.1,0.15,1.0);
	}
	
	current_sccan_point.get_reflector_image_exposure_time_stamp(
	&resized_reflector_image_with_overlay);
	
	
	
	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"got resized reflector image with overlay.";
		std::cout<<std::endl;
	}	
	
	//==================================================================
	//concatenate matrices
	//==================================================================	
	
	alignment_image.horizontal_concatenate_with_delimiter_line(
	&resized_star_image_with_overlay,
	&resized_reflector_image_with_overlay);
	
	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"end.";
		std::cout<<std::endl;
	}
}
//======================================================================
std::string get_quick_alignment_prompt(){
	std::stringstream out, info;
	out<<pointer_to_sccan_point_pair_handler->get_prompt();
	
	out<<"|"<<std::endl;
	
	// Mirror ID to align
	info.str("");
	if(pointer_to_reflector->
	pointer_to_current_mirror_to_work_with==NULL){
		info<<"has not beem set yet";
	}else{
		info<<pointer_to_reflector->
		pointer_to_current_mirror_to_work_with->get_mirror_ID();
	}
	out<<make_nice_line_with_dots("| Mirror ID to align",info.str());
	
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
void interaction(){
	
	std::string key_user_wants_to_satrt_quick_mirror_alignment  ="q";
	add_control(key_user_wants_to_satrt_quick_mirror_alignment,
	"start quick mirror alignment");	
	
	std::string key_user_wants_to_end_quick_mirror_alignment  ="back";
	add_control(key_user_wants_to_end_quick_mirror_alignment,
	"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_align_mirrors = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_quick_alignment_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	if(valid_user_input.
	compare(key_user_wants_to_end_quick_mirror_alignment)==0){
		flag_user_wants_to_align_mirrors = false;
	}
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_satrt_quick_mirror_alignment)==0){
		run_alignment();
	}	
	//==================================================================
	}while(flag_user_wants_to_align_mirrors);
}
//======================================================================
void toggle_verbosity(){
	verbosity = !verbosity;
	alignment_image.toggle_verbosity(verbosity);
}
//======================================================================
};
//======================================================================
class sccan_point_analysis 		:public user_interaction{
	public:
	bool 	sccan_point_analysis_verbosity;
	
	std::vector< std::vector<sccan_analysis_point*> > sccan_matrix;
	// dimension 1 sccan analysis points
	// dimension 2 mirrors
	//input
	sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;
	reflector				* pointer_to_reflector;
//======================================================================
sccan_point_analysis(
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
void fill_sccan_matrix(){
	
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
std::string get_sccan_matrix_prompt(){
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
void display_sccan_matrix_prompt(){
	std::cout<<std::endl;
	std::cout<<get_sccan_matrix_prompt();
}
//======================================================================
std::string get_analysis_prompt(){
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
void interaction(){
	
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
void toggle_verbosity(){
	sccan_point_analysis_verbosity = !sccan_point_analysis_verbosity;
}
//======================================================================
bool verbosity_status(){
	return sccan_point_analysis_verbosity;
}
//======================================================================
void draw_telescope_pointing_directions_of_sccan_runs(){
   
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
void draw_all_mirror_light_flux_response_maps(){
	
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
void draw_normalized_light_flux_response_map(uint mirror_itterator){
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
private:
std::string get_tilt_position_matrix_for_python(){
	
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
std::string get_light_flux_matrix_for_python(uint mirror_itterator){
	

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
};
//======================================================================
class verbosity_handler 		:public user_interaction{
	//this class is for toggling programm verbosity during runtime
	time_stamp_manager 	*pointer_to_time_stamp_manager;
	ueye_camera		*pointer_to_star_camera;
	ueye_camera		*pointer_to_reflector_camera;
	reflector		*pointer_to_reflector;
	snapshot		*pointer_to_snapshot;
	sccan_point_pair_handler	*pointer_to_sccan_point_pair_handler;
	quick_align		*pointer_to_quick_align;
	sccan_point_analysis	*pointer_to_sccan_point_analysis;
public:
//======================================================================
verbosity_handler(
	time_stamp_manager 	*new_pointer_to_time_stamp_manager,
	ueye_camera		*new_pointer_to_star_camera,
	ueye_camera		*new_pointer_to_reflector_camera,
	reflector		*new_pointer_to_reflector,
	snapshot		*new_pointer_to_snapshot,
	sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
	quick_align		*new_pointer_to_quick_align,
	sccan_point_analysis	*new_pointer_to_sccan_point_analysis
){
	pointer_to_time_stamp_manager = new_pointer_to_time_stamp_manager;
	pointer_to_star_camera 	= new_pointer_to_star_camera;
	pointer_to_reflector_camera = new_pointer_to_reflector_camera;
	pointer_to_reflector 	= new_pointer_to_reflector;
	pointer_to_snapshot 	= new_pointer_to_snapshot;
	pointer_to_sccan_point_pair_handler =
	new_pointer_to_sccan_point_pair_handler;
	pointer_to_quick_align = new_pointer_to_quick_align;
	pointer_to_sccan_point_analysis = 
	new_pointer_to_sccan_point_analysis;
	
	menu_name = "verbosity menu";
	toggle_all_verbosity(false);
}
//======================================================================
std::string get_verbosity_menu_prompt(){
	std::stringstream out, info;
	
	out<<"| verbosity status"<<std::endl;
	
	// time manager
	info.str("");
	if(pointer_to_time_stamp_manager->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| time manager",info.str());

	// star_camera
	info.str("");
	if(pointer_to_star_camera->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| star camera",info.str());

	// reflector_camera
	info.str("");
	if(pointer_to_reflector_camera->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| reflector camera",info.str());

	// reflector
	info.str("");
	if(pointer_to_reflector->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| reflector manager",info.str());

	//snapshot
	info.str("");
	if(pointer_to_snapshot->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| snapshot",info.str());
		
	//sccan_point_pair_handler
	info.str("");
	if(pointer_to_sccan_point_pair_handler->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| sccan point pair acquisition",info.str());
		
	//quick_align
	info.str("");
	if(pointer_to_quick_align->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| quick single mirror alignment",info.str());	
	
	//sccan_point_analysis
	info.str("");
	if(pointer_to_sccan_point_analysis->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| sccan point pair analysis",info.str());	
		
	//horizontal line
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	
	return out.str(); 
}
//======================================================================
void interaction(){
	std::string key_toggle_time_stamp_manager  ="t";
	add_control(key_toggle_time_stamp_manager,"time manager");	

	std::string key_toggle_star_camera = "sc";
	add_control(key_toggle_star_camera,"star camera");
	
	std::string key_toggle_reflector_camera = "rc";
	add_control(key_toggle_reflector_camera,"reflector camera");
	
	std::string key_toggle_reflector  ="r";
	add_control(key_toggle_reflector,"reflector handler");

	std::string key_toggle_sccan_point_handler  ="sh";
	add_control(key_toggle_sccan_point_handler,"sccan handler");

	std::string key_toggle_snapshot  ="sn";
	add_control(key_toggle_snapshot,"snapshot");

	std::string key_toggle_quick_align  ="q";
	add_control(key_toggle_quick_align,"quick align");

	std::string key_sccan_point_analysis  ="sa";
	add_control(key_sccan_point_analysis,"sccan point pair analysis");
	
	std::string key_all_verbosity_true  ="TRUE";
	add_control(key_all_verbosity_true,"activate all");	

	std::string key_all_verbosity_false  ="FALSE";
	add_control(key_all_verbosity_false,"inactivate all");	
	
	std::string key_back_to_main_menu  ="back";
	add_control(key_back_to_main_menu,"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_toggle_verbosity = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_verbosity_menu_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_back_to_main_menu)==0){
		flag_user_wants_to_toggle_verbosity = false;
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_time_stamp_manager)==0){
		pointer_to_time_stamp_manager->toggle_verbosity();
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_reflector_camera)==0){
		pointer_to_reflector_camera->toggle_verbosity();
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_star_camera)==0){
		pointer_to_star_camera->toggle_verbosity();
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_reflector)==0){
		pointer_to_reflector->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_sccan_point_handler)==0){
		pointer_to_sccan_point_pair_handler->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_quick_align)==0){
		pointer_to_quick_align->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_sccan_point_analysis)==0){
		pointer_to_sccan_point_analysis->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_all_verbosity_true)==0){
		toggle_all_verbosity(true);
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_snapshot )==0){
		pointer_to_snapshot->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_all_verbosity_false)==0){
		toggle_all_verbosity(false);
	}
	//==================================================================
}while(flag_user_wants_to_toggle_verbosity);
}
//======================================================================
void toggle_all_verbosity(bool new_verbosity){
	
	if(pointer_to_time_stamp_manager->verbosity_status() != new_verbosity)
	pointer_to_time_stamp_manager->toggle_verbosity();
			
	if(pointer_to_star_camera->verbosity_status() != new_verbosity)
		pointer_to_star_camera->toggle_verbosity();

	if(pointer_to_reflector_camera->verbosity_status() != new_verbosity)
		pointer_to_reflector_camera->toggle_verbosity();
		
	if(pointer_to_reflector->verbosity_status() != new_verbosity)
		pointer_to_reflector->toggle_verbosity();

	if(pointer_to_snapshot->verbosity_status() != new_verbosity)
		pointer_to_snapshot->toggle_verbosity();

	if(pointer_to_sccan_point_pair_handler->verbosity_status() 
	!= new_verbosity)
		pointer_to_sccan_point_pair_handler->toggle_verbosity();
	
	if(pointer_to_quick_align->verbosity_status() 
	!= new_verbosity)
		pointer_to_quick_align->toggle_verbosity();
		
	if(pointer_to_sccan_point_analysis->verbosity_status() 
	!= new_verbosity)
		pointer_to_sccan_point_analysis->toggle_verbosity();
}
//======================================================================
};
//======================================================================
class main_menu 				:public user_interaction{
	public:
	snapshot *pointer_to_snapshot;
	reflector *pointer_to_reflector;
	sccan_point_pair_handler *pointer_to_sccan_point_pair_handler;
	quick_align* pointer_to_quick_align;
	sccan_point_analysis *pointer_to_sccan_point_analysis;
	verbosity_handler *pointer_to_verbosity_handler;
	ueye_camera *pointer_to_star_camera;
	ueye_camera *pointer_to_reflector_camera;
//======================================================================
main_menu
(
snapshot *new_pointer_to_snapshot,
reflector *new_pointer_to_reflector,
sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
quick_align *new_pointer_to_quick_align,
sccan_point_analysis *new_pointer_to_sccan_point_analysis,
verbosity_handler *new_pointer_to_verbosity_handler,
ueye_camera *new_pointer_to_star_camera,
ueye_camera *new_pointer_to_reflector_camera){
	
	pointer_to_snapshot = 
	new_pointer_to_snapshot;
	
	pointer_to_reflector = 
	new_pointer_to_reflector;
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	
	pointer_to_quick_align = 
	new_pointer_to_quick_align;
	
	pointer_to_sccan_point_analysis = 
	new_pointer_to_sccan_point_analysis;
	
	pointer_to_verbosity_handler=
	new_pointer_to_verbosity_handler;
	
	pointer_to_star_camera = new_pointer_to_star_camera;
	pointer_to_reflector_camera = new_pointer_to_reflector_camera;
	
	menu_name = "Mirror alignment main menu";
}
//======================================================================
std::string get_main_menu_status_prompt(){
	std::stringstream out, info;
	
	// status star camera
	info.str("");
	if(pointer_to_star_camera->camera_status())
	{info<<"[online]";}else{info<<"[offline]";}
	out<<make_nice_line_with_dots
	("| star camera",info.str());

	// status reflector camera
	info.str("");
	if(pointer_to_reflector_camera->camera_status())
	{info<<"[online]";}else{info<<"[offline]";}
	out<<make_nice_line_with_dots
	("| reflector camera",info.str());
	
	// horizontal line 
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str(); 
}
//======================================================================
void interaction(){
	
	std::string key_user_wants_to_interaction  ="s";
	add_control(key_user_wants_to_interaction,
	"take snapshots");
	
	std::string key_user_wants_to_modify_reflector="r";
	add_control(key_user_wants_to_modify_reflector,
	"modify mirrors of reflector");
	
	std::string key_user_wants_to_acquire_sccan_points="a";
	add_control(key_user_wants_to_acquire_sccan_points,
	"acquire sccan points");
	
	std::string key_user_wants_to_quick_align_a_mirror ="q";
	add_control(key_user_wants_to_quick_align_a_mirror,
	"quick single mirror alignment");
	
	std::string key_analysis ="A";
	add_control(key_analysis,
	"analyse sccan points");
	
	std::string key_toggle_verbosity ="v";
	add_control(key_toggle_verbosity,
	"toggle programm verbosity");
	
	std::string key_user_wants_to_end_mirror_alignment  ="quit";
	add_control(key_user_wants_to_end_mirror_alignment,
	"quit mirror alignment");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_align_mirrors = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_main_menu_status_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_end_mirror_alignment)
	==0){
		flag_user_wants_to_align_mirrors = false;
	}
	//==================================================================
	if(valid_user_input.compare(key_analysis)==0){
		pointer_to_sccan_point_analysis->interaction();
	}
	//==================================================================
	if(valid_user_input.compare(key_user_wants_to_interaction)==0){
		pointer_to_snapshot->interaction();
	}
	//==================================================================
	if(valid_user_input.compare(key_user_wants_to_modify_reflector)==0){
		pointer_to_reflector->interaction();
	}
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_acquire_sccan_points)==0){
		pointer_to_sccan_point_pair_handler->interaction();
	} 
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_quick_align_a_mirror)==0){
		pointer_to_quick_align->interaction();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_verbosity)==0){
		pointer_to_verbosity_handler->interaction();
	}
	//==================================================================
}while(flag_user_wants_to_align_mirrors);
}
//======================================================================
};                                                                      
//======================================================================
int main(){
	int system_call_return_value;
	system_call_return_value = system("clear"); 
	system_call_return_value = system("clear"); 

	// python for plotting
	Py_Initialize();
	
	//==================================================================
	// star camera
	//==================================================================	
		
		//==============================================================
		// intrinsic paramters for star camera
		//==============================================================
	intrinsic_camera_parameter parameters_for_star_camera;
	parameters_for_star_camera.set_names(
	"ueye 5MPx CMOS",
	"Carl Zeiss Flektogon F2.4 / f35mm");
	
	parameters_for_star_camera.
	set_coefficients_for_radiometric_correction_plane(
	-1.2185,
	1.2021,
	0.99303
	);	
	
	ueye_camera star_camera(13,parameters_for_star_camera);
	//==================================================================
	// reflector camera
	//==================================================================	
		
		//==============================================================
		// intrinsic paramters for reflector camera
		//==============================================================
	intrinsic_camera_parameter parameters_for_reflector_camera;
	parameters_for_reflector_camera.set_names(
	"Thor Labs 1.3MPx CCD",
	"M12 the imageing source F2.0 / f4mm"
	);
	
	parameters_for_reflector_camera.
	set_coefficients_for_radiometric_correction_plane(
	-1.1527,
	1.0283,	
	-0.18637
	);
	
	ueye_camera reflector_camera(42,parameters_for_reflector_camera);


	star_camera.display_camera_information();
	reflector_camera.display_camera_information();
	//==================================================================
	// hanldes
	//==================================================================	
	
	sccan_point_pair_handler sccan_handle;
	sccan_handle.set_cameras(&star_camera,&reflector_camera);

	//sccan_handle.acquire_sccan_points(5);
	snapshot snap;
	snap.add_camera(&star_camera);
	snap.add_camera(&reflector_camera);

	reflector reflector_instance;
	quick_align quick(&reflector_instance,&sccan_handle);
	

	sccan_point_analysis analysis(
	&sccan_handle,&reflector_instance);
	
	verbosity_handler verbosity_interaction(
	&global_time_stamp_manager_instance,
	&star_camera,
	&reflector_camera,
	&reflector_instance,
	&snap,
	&sccan_handle,
	&quick,
	&analysis
	);
	
	main_menu menu(
	&snap,
	&reflector_instance,
	&sccan_handle,
	&quick,
	&analysis,
	&verbosity_interaction,
	&star_camera,
	&reflector_camera);
	
	menu.interaction();	
	
	return 0;
	}
	
