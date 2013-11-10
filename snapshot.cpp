#include "snapshot.h"
//======================================================================
snapshot::snapshot(){
	initialize();
	menu_name = "snapshot";
	//add_camera(NULL);
}
//======================================================================
snapshot::snapshot(ueye_camera* ptr_to_snapshot_cam){
	add_camera(ptr_to_snapshot_cam);
	initialize();
}
//======================================================================
void snapshot::initialize(){
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
void snapshot::add_camera(ueye_camera* ptr_to_snapshot_cam){
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
void snapshot::interaction(){

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
void snapshot::create_snapshot_file_name(){
	std::stringstream snapshot_file_name_stream;
	snapshot_file_name_stream<<global_time_stamp_manager_instance.
	get_current_time_stamp();
	snapshot_file_name_stream<<"_snapshot";
	snapshot_file_name_stream<<"_"<<snapshot_counter;
	snapshot_file_name = snapshot_file_name_stream.str();
}
//======================================================================
std::string snapshot::get_snapshot_prompt(){
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
bool snapshot::choose_camera(uint camera_ID_to_use){
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
void snapshot::toggle_verbosity(){
	snapshot_verbosity = !snapshot_verbosity;
}
//======================================================================
bool snapshot::verbosity_status(){
	return snapshot_verbosity;
}
//======================================================================
void snapshot::take_exposure_time_series(
int number_of_exposure_itterations){
	
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
void snapshot::spectate_stars(
double threshold_in_sigmas_for_star_detection,
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
