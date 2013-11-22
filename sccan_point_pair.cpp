#include "sccan_point_pair.h"
//======================================================================
sccan_point_pair::sccan_point_pair(
uint new_run_number,bool new_verbosity){
	initialize(new_run_number,new_verbosity);
}
//======================================================================
void sccan_point_pair::initialize(
uint new_run_number,bool new_verbosity){
	verbosity = new_verbosity;
	star_detection_verbosity =false;
	
	reflector_image.toggle_verbosity(verbosity);
	star_image.toggle_verbosity(verbosity);
	sccan_star.toggle_verbosity(verbosity);
	
	//star_camera_intrinsic_parameter.toggle_verbosity(verbosity);

	//reflector_camera_intrinsic_parameter.toggle_verbosity(verbosity);
	
	reflector_image_name_postfix = "_reflector";
	star_image_name_postfix = "_star";	

	run_number = new_run_number;
	
	pointer_to_star_camera = NULL;
	pointer_to_reflector_camera = NULL;

	// star detection
	single_star_has_been_found = false;
	//star_position_in_pixel_x = -1;
	//star_position_in_pixel_y = -1;	
	create_sccan_point_name_prefix_using_current_timestamp();
	file_name_extension_for_sccan_point_configuration_file = ".cfg";
	
	set_sccan_point_name_prefix(sccan_point_name_prefix);
	
	key_sccan_point 					="sccan";
	key_run_startup_time 				="run_startup_time";
	key_sccan_point_name_prefix 		="sccan_point_name";
	key_sccan_run_number 				="sccan_run_number";
	key_star_camera_ID 					="star_camera_ID";
	key_star_image_filename 			="star_image_filename";
	key_star_exposure_time_in_ms 		="star_exposure_time_in_ms";
	key_reflector_camera_ID 			="reflector_camera_ID";
	key_reflector_image_filename 		="reflector_image_filename";
	key_reflector_exposure_time_in_ms	=
	"reflector_exposure_time_in_ms";
	key_single_star_has_been_found		="single_star_has_been_found";
	key_star_position_in_pixels_x		="star_X_px";
	key_star_position_in_pixels_y		="star_Y_px";
	//key_star_light_flux_is_valid		="light_flux_was_estimated";
	key_light_flux_of_star_in_pixel_values_per_milli_second =
	"light_flux_of_star_in_pixel_values_per_milli_second";
	
	key_absolute_light_flux_in_standard_bulbs = "absolute_light_flux";
}
//======================================================================
void sccan_point_pair::set_sccan_point_name_prefix(
std::string new_file_name_prefix){
	
	sccan_point_name_prefix = new_file_name_prefix;
	
	sccan_point_configuration_file_name = sccan_point_name_prefix;
	sccan_point_configuration_file_name += 
	file_name_extension_for_sccan_point_configuration_file;
}
//======================================================================
void sccan_point_pair::set_run_number(uint new_run_number){
	run_number = new_run_number;
	
	create_sccan_point_name_prefix_using_current_timestamp();
	sccan_point_configuration_file_name = sccan_point_name_prefix;
	sccan_point_configuration_file_name += ".cfg";
}
//======================================================================
uint sccan_point_pair::get_run_number(){
return run_number;
}
//======================================================================
void sccan_point_pair::create_sccan_point_name_prefix_using_current_timestamp(){
	std::stringstream file_name_prefix_stream;
	file_name_prefix_stream<<global_time_stamp_manager_instance.
	get_startup_time_stamp();
	file_name_prefix_stream<<"_sccan_";
	file_name_prefix_stream<<run_number;
	sccan_point_name_prefix = file_name_prefix_stream.str();
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"create_sccan_point_name";
		std::cout<<"_prefix_using_current_timestamp() -> ";
		std::cout<<sccan_point_name_prefix<<std::endl;	
	}
}
//======================================================================
bool sccan_point_pair::save_sccan_point_pair(){
	if(verbosity){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()"<<std::endl;
	}	
	
	if(reflector_image.export_image_to_disk()==false)
	{
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving reflector sccan_image!"<<std::endl;
		return false;
	}
		
	if(star_image.export_image_to_disk()==false)
	{
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving star sccan_image!"<<std::endl;
		return false;
	}
	
	if(save_sccan_point_configuration()==false){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving sccan config"<<std::endl;
		return false;
	}
	
	/*
	if(star_camera_intrinsic_parameter.
	save_intrinsic_parameter
	(sccan_point_name_prefix+star_image_name_postfix)==false){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving intrinsic parameter of star camera";
		std::cout<<std::endl;
		return false;
	}*/
	
	/*
	if(reflector_camera_intrinsic_parameter.
	save_intrinsic_parameter
	(sccan_point_name_prefix+reflector_image_name_postfix)
	==false){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving intrinsic parameter of reflector camera";
		std::cout<<std::endl;
		return false;
	}*/
	
	return true;
}
//======================================================================
bool sccan_point_pair::load_sccan_point_pair(
std::string sccan_point_name_prefix_to_load){
	if(verbosity){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()"<<std::endl;
	}
	
	if(reflector_image.load_image
	(sccan_point_name_prefix_to_load+reflector_image_name_postfix)
	==false)
	{
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading reflector sccan_image!"<<std::endl;
		return false;
	}	
	
	if(star_image.load_image
	(sccan_point_name_prefix_to_load+star_image_name_postfix)
	==false)
	{
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading star sccan_image!"<<std::endl;
		return false;
	}

	if(load_sccan_point_configuration
	(sccan_point_name_prefix_to_load)==false){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading sccan config"<<std::endl;
		return false;
	}	
	
	/*
	if(star_camera_intrinsic_parameter.
	load_intrinsic_camera_parameters
	(sccan_point_name_prefix_to_load+star_image_name_postfix)==false){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading intrinsic parameter of star camera";
		std::cout<<std::endl;
		return false;
	}
	*/
	/*
	if(reflector_camera_intrinsic_parameter.
	load_intrinsic_camera_parameters
	(sccan_point_name_prefix_to_load+reflector_image_name_postfix)
	==false){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading intrinsic parameter of reflector camera";
		std::cout<<std::endl;
		return false;
	}	
	*/
	return true;
}
//======================================================================
bool sccan_point_pair::load_sccan_point_configuration
(std::string name_of_sccan_point_pair_to_load){
	libconfig::Config sccan_point_configuration_handle;
	//==================================================================
	// Read the file. If there is an error, report it and exit.
	//==================================================================
	name_of_sccan_point_pair_to_load +=
	file_name_extension_for_sccan_point_configuration_file;
	
	std::stringstream out;
	out<<"sccan_point_pair -> load_sccan_point_configuration() -> ";
	out<<name_of_sccan_point_pair_to_load<<"\n";
	std::cout << out.str();
	
	try
	{
		sccan_point_configuration_handle.
		readFile(name_of_sccan_point_pair_to_load.c_str());
	}
	catch(const libconfig::FileIOException &fioex)
	{
		std::cout << "I/O error while reading file." << std::endl;
		return(EXIT_FAILURE);
	}
	catch(const libconfig::ParseException &pex)
	{
		std::cout << "Parse error at " << pex.getFile() << ":";
		std::cout << pex.getLine() << " - " << pex.getError() << std::endl;
		return(EXIT_FAILURE);
	}
	//==================================================================
	// get data
	//==================================================================
	const libconfig::Setting& root = sccan_point_configuration_handle.getRoot();
	
	bool reading_is_valid = true;
	
	// Output a list of all books in the inventory.
	try
	{
		const libconfig::Setting &Sccan = root[key_sccan_point];

		// delete all old sccan information
	//==================================================================
	// run number		
	//==================================================================
	int new_sccan_point_pair_run_number;
	if( Sccan.lookupValue
	(key_sccan_run_number, new_sccan_point_pair_run_number) ){
		initialize((uint)new_sccan_point_pair_run_number,verbosity);
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_sccan_run_number<<" is set to ";
			std::cout<<run_number<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> load_sccan_point_configuration() -> ";
		std::cout<<key_sccan_run_number<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	// sccan_point_name_prefix
	//==================================================================
	std::string new_sccan_point_name_prefix;
	if( Sccan.lookupValue
	(key_sccan_point_name_prefix, new_sccan_point_name_prefix) ){
		set_sccan_point_name_prefix(new_sccan_point_name_prefix);
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_sccan_point_name_prefix<<" is set to ";
			std::cout<<sccan_point_name_prefix<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> load_sccan_point_configuration() -> ";
		std::cout<<key_sccan_point_name_prefix<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// single star flag
	//==================================================================
	bool single_star_has_been_found_flag_of_file;
	if( Sccan.lookupValue
	(key_single_star_has_been_found, 
	single_star_has_been_found_flag_of_file) 
	){
		
		single_star_has_been_found = 
		single_star_has_been_found_flag_of_file;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_single_star_has_been_found<<" is set to ";
			if(single_star_has_been_found)
			{std::cout<<"true";}else{std::cout<<"false";}
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_sccan_point_name_prefix<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// single star pixel position X
	//==================================================================
	int new_single_star_pixel_position_x;
	if( 
	Sccan.lookupValue(
	key_star_position_in_pixels_x, new_single_star_pixel_position_x) 
	){
		sccan_star.position_in_pixels.x = 
		new_single_star_pixel_position_x;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_star_position_in_pixels_x<<" is set to ";
			std::cout<<sccan_star.position_in_pixels.x;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_star_position_in_pixels_x<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// single star pixel position Y
	//==================================================================
	int new_single_star_pixel_position_y;
	if( 
	Sccan.lookupValue(
	key_star_position_in_pixels_y, new_single_star_pixel_position_y) 
	){
		sccan_star.position_in_pixels.y =
		new_single_star_pixel_position_y;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_star_position_in_pixels_y<<" is set to ";
			std::cout<<sccan_star.position_in_pixels.y;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_star_position_in_pixels_y<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	sccan_star.star_direction_relative_to_camera = 
	star_image.intrinsic_parameters.
	calculate_pointing_direction(sccan_star.position_in_pixels);
	
	//==================================================================
	// light flux in
	//==================================================================
	/*
	double new_light_flux_of_star_in_pixel_values_per_milli_second;
	if( 
	Sccan.lookupValue(
	key_light_flux_of_star_in_pixel_values_per_milli_second,
	new_light_flux_of_star_in_pixel_values_per_milli_second) 
	){
		light_flux_of_star_in_pixel_values_per_milli_second = 
		new_light_flux_of_star_in_pixel_values_per_milli_second;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<
			key_light_flux_of_star_in_pixel_values_per_milli_second;
			std::cout<<" is set to ";
			std::cout<<light_flux_of_star_in_pixel_values_per_milli_second;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_light_flux_of_star_in_pixel_values_per_milli_second;
		std::cout<<" fail."<<std::endl;
		reading_is_valid = false;
	}	*/
	//==================================================================
	// absolute light flux standard bulbs
	//==================================================================
	double new_absolute_light_flux_in_standard_bulbs;
	if( 
	Sccan.lookupValue(
	key_absolute_light_flux_in_standard_bulbs,
	new_absolute_light_flux_in_standard_bulbs) 
	){
		sccan_star.light_flux_of_star_in_bulbs = 
		new_absolute_light_flux_in_standard_bulbs;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<
			key_absolute_light_flux_in_standard_bulbs;
			std::cout<<" is set to ";
			std::cout<<sccan_star.light_flux_of_star_in_bulbs;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_absolute_light_flux_in_standard_bulbs;
		std::cout<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	
	//==================================================================		
	}
	catch(const libconfig::SettingNotFoundException &nfex)
	{
		std::cout<<"reflector -> load_sccan_point_configuration() ->";
		std::cout<<"loading failed! Settings not found!"<<std::endl;
		reading_is_valid  = false;
	}
	return reading_is_valid;
}
//======================================================================
std::string sccan_point_pair::get_sccan_point_pair_string(){
	std::stringstream out;
	out<<" _sccan_point_pair_No"<<run_number;
	out<<"_____________________"<<std::endl;
	out<<"| "<<std::endl;
	out<<"| sccan name prefix...: "<<sccan_point_name_prefix<<std::endl;
	out<<"| startup time........: "<<global_time_stamp_manager_instance.get_startup_time_detail();
	out<<"| run number..........: "<<run_number<<std::endl;
	out<<"| "<<std::endl;
	out<<"| star camera ID......: ";
	if(pointer_to_star_camera != NULL){

	out<<pointer_to_star_camera->get_camera_ID()<<std::endl;
	}else if(
	pointer_to_star_camera == NULL && 
	reflector_image.get_init_status() == true){
	out<<star_image.
	get_camera_ID_of_camera_taken_this_image()<<std::endl;
	}else{
	out<<"not set yet"<<std::endl;
	}
	out<<"| star file name......: "<<star_image.get_file_name()<<std::endl;
	out<<"| exposure time.......: ";
	out<<star_image.get_exposure_time_in_ms()<<" ms"<<std::endl;
	out<<"| "<<std::endl;
	out<<"| reflector camera ID.: ";
	if(pointer_to_reflector_camera != NULL){
	out<<pointer_to_reflector_camera->get_camera_ID()<<std::endl;
	}else if(
	pointer_to_reflector_camera == NULL && 
	reflector_image.get_init_status() == true){
	out<<reflector_image.
	get_camera_ID_of_camera_taken_this_image()<<std::endl;
	}else{
	out<<"not set yet"<<std::endl;
	}
	out<<"| reflector file name.: "<<reflector_image.get_file_name()<<std::endl;
	out<<"| exposure time.......: ";
	out<<reflector_image.get_exposure_time_in_ms()<<" ms"<<std::endl;
	out<<"| "<<std::endl;
	out<<"| single star was found......:";
	if(single_star_has_been_found){
	out<<"TRUE at ("<<sccan_star.position_in_pixels.x;
	out<<"|"<<sccan_star.position_in_pixels.y<")";
	}else{
	out<<"FALSE";
	}
	out<<std::endl;
	out<<"|__________________________________________"<<std::endl;
	
	if(verbosity){
		std::cout<<"sccan_point_pair->get_sccan_point_pair_string()"<<std::endl;
	}
	return out.str();
}
//======================================================================
void sccan_point_pair::disp(){
	std::cout<<get_sccan_point_pair_string();
}
//======================================================================
void sccan_point_pair::set_cameras(
ueye_camera* star_camera,ueye_camera* reflector_camera){
pointer_to_star_camera = star_camera;
//star_camera_intrinsic_parameter = pointer_to_star_camera->intrinsic;

pointer_to_reflector_camera = reflector_camera;
//reflector_camera_intrinsic_parameter = reflector_camera->intrinsic;
}
//======================================================================
bool sccan_point_pair::acquire_sccan_images(
double *pointer_to_star_exposure_time_in_ms, 
double *pointer_to_reflector_exposure_time_in_ms,
double desired_max_rel_star_cam_response,
double desired_max_rel_refl_cam_response,
int		max_number_of_exposure_itterations,
bool	*max_number_of_exposure_itterations_has_been_exceeded
){
	if(pointer_to_star_camera->initialization_succesfull == FALSE){
		std::cout<<"sccan_point_pair -> acquire_sccan_images() -> ";
		std::cout<<"star camera is not initialized.\n";		
		return FALSE;
	}

	if(pointer_to_reflector_camera->initialization_succesfull == FALSE){
		std::cout<<"sccan_point_pair -> acquire_sccan_images() -> ";
		std::cout<<"reflector camera is not initialized.\n";			
		return FALSE;
	}
	
	
	bool star_image_ok;
	bool reflector_image_ok;
	
	star_image_ok =
	pointer_to_star_camera->
	acquire_image(
	pointer_to_star_exposure_time_in_ms,
	desired_max_rel_star_cam_response);
	
	reflector_image_ok =
	pointer_to_reflector_camera->
	acquire_image(
	pointer_to_reflector_exposure_time_in_ms,
	desired_max_rel_refl_cam_response);	
	
	star_image = 
	pointer_to_star_camera->get_latest_image();
	star_image.set_name
	(sccan_point_name_prefix+star_image_name_postfix);
	star_image.toggle_verbosity(verbosity);
	
	reflector_image = 
	pointer_to_reflector_camera->get_latest_image();
	reflector_image.set_name
	(sccan_point_name_prefix+reflector_image_name_postfix);
	reflector_image.toggle_verbosity(verbosity);
	
	if(star_image_ok==true && reflector_image_ok==true){
		if(verbosity){
			std::cout<<"sccan_point_pair -> acquire_sccan_images() -> ";
			std::cout<<"images have been acquired"<<std::endl;
			*max_number_of_exposure_itterations_has_been_exceeded = true;
		}
		return true;
	}else{
		std::cout<<"sccan_point_pair -> acquire_sccan_images() -> ";
		std::cout<<"aquisition failed. ";
		std::cout<<"Exposure might be out of range!"<<std::endl;
		*max_number_of_exposure_itterations_has_been_exceeded = false;
		return false;
	}
}
//======================================================================
bool sccan_point_pair::save_sccan_point_configuration(){
	libconfig::Config sccan_point_configuration_handle;
	libconfig::Setting &root = sccan_point_configuration_handle.getRoot();

	// Add some settings to the configuration.
	libconfig::Setting &sccan_point_settings = 
	root.add(key_sccan_point, libconfig::Setting::TypeGroup);
	
	sccan_point_settings.
	add(key_sccan_point_name_prefix,libconfig::Setting::TypeString) =
	sccan_point_name_prefix;
	
	sccan_point_settings.
	add(key_run_startup_time,libconfig::Setting::TypeString) =
	global_time_stamp_manager_instance.get_startup_time_detail();
	
	sccan_point_settings.
	add(key_sccan_run_number, libconfig::Setting::TypeInt) =
	(int)run_number;

	sccan_point_settings.
	add(key_star_image_filename,libconfig::Setting::TypeString) = 
	star_image.get_file_name();

	sccan_point_settings.
	add(key_reflector_image_filename,	libconfig::Setting::TypeString) = 
	reflector_image.get_file_name();

	sccan_point_settings.
	add(key_single_star_has_been_found,libconfig::Setting::TypeBoolean) = 
	single_star_has_been_found;

	sccan_point_settings.
	add(key_star_position_in_pixels_x,libconfig::Setting::TypeInt) = 
	(int)sccan_star.position_in_pixels.x;
	
	sccan_point_settings.
	add(key_star_position_in_pixels_y,libconfig::Setting::TypeInt) = 
	(int)sccan_star.position_in_pixels.y;
	//==================================================================
	// light flux of star
	//==================================================================
	//relative 
	/*
	sccan_point_settings.
	add(key_light_flux_of_star_in_pixel_values_per_milli_second,
	Setting::TypeFloat) = 
	light_flux_of_star_in_pixel_values_per_milli_second;
	*/
	// absolute
	sccan_point_settings.
	add(key_absolute_light_flux_in_standard_bulbs,
	libconfig::Setting::TypeFloat) = 
	sccan_star.light_flux_of_star_in_bulbs;
	
	// Write out the new configuration.
	try
	{
		sccan_point_configuration_handle.
		writeFile(sccan_point_configuration_file_name.c_str());
		
		if(verbosity){
			std::cout << "New configuration successfully written to: "<< 
			sccan_point_configuration_file_name<<std::endl;
		}
	}
	catch(const libconfig::FileIOException &fioex)
	{
		std::cerr << "I/O error while writing file: "<<
		sccan_point_configuration_file_name<<std::endl;
		return(false);
	}

	return(true);	
}
//======================================================================
void sccan_point_pair::find_bright_star_in_star_image
(double threshold_in_sigmas,
double radius_one_sigma_for_star_detection_in_degrees){
	
	std::vector<star_in_image> list_of_stars_in_star_image =
	star_image.find_stars(
	threshold_in_sigmas,
	radius_one_sigma_for_star_detection_in_degrees
	);

	if(list_of_stars_in_star_image.size() == 1){
		//one single star was found
		
		single_star_has_been_found = true;
		sccan_star = list_of_stars_in_star_image.at(0);
		
		if(verbosity){
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<" found one single star brighter than ";
			std::cout<<threshold_in_sigmas<<" sigma at ";
			std::cout<<"("<<sccan_star.position_in_pixels.x;
			std::cout<<"|"<<sccan_star.position_in_pixels.y;
			std::cout<<")px ";
			std::cout<<"flux ";
			std::cout<<sccan_star.light_flux_of_star_in_bulbs;
			std::cout<<"bulbs";
			std::cout<<std::endl;
		}
		
	}else if(list_of_stars_in_star_image.size() == 0){
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<" found no star at all brighter than ";
			std::cout<<threshold_in_sigmas<<" sigma."<<std::endl;
	}else{
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<" found "<<list_of_stars_in_star_image.size();
			std::cout<<" stars brighter than ";
			std::cout<<threshold_in_sigmas<<" sigma."<<std::endl;
	}
}
//======================================================================
void sccan_point_pair::display_calibration_star_in_star_image(
int time_to_display_in_ms){
	if(single_star_has_been_found){
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"display_calibration_star_in_star_image() -> ";
			std::cout<<"the calibration star was found in star_image.";
			std::cout<<std::endl;
		}
		//visualize stars
		list_of_pixel_positions list_of_star_locations_in_pixels;
		
		cv::Point calibration_star=
		sccan_star.position_in_pixels;
		
		list_of_star_locations_in_pixels.push_back(calibration_star);
		
		simple_image star_image_with_overlay = star_image.
		get_overlay_with_stars(
		list_of_star_locations_in_pixels,
		cv::Scalar(128,128,255)
		);
		
		std::stringstream window_name; 
		window_name<<"star overlay, ";
		window_name<<list_of_star_locations_in_pixels.size()<<"stars, ";

		star_image_with_overlay.
		disp_simple_image(window_name.str(),time_to_display_in_ms);	
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"display_calibration_star_in_star_image() -> ";
		std::cout<<"the calibration star was not found in star_image now!";
		std::cout<<std::endl;
	}
}
//======================================================================
bool sccan_point_pair::a_single_star_has_been_found(){
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"a_single_star_has_been_found() -> ";
		std::cout<<"return ";
		if(single_star_has_been_found)
		{std::cout<<"true";}else{std::cout<<"false";}
		std::cout<<std::endl;
	}
	return(single_star_has_been_found);
}
//======================================================================
cv::Point sccan_point_pair::get_point_of_star_in_star_image(){
	return sccan_star.position_in_pixels;
}
//======================================================================
simple_image sccan_point_pair::get_resized_star_image_with_overlay
(int height_of_alignment_image){
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_star_image_with_overlay() -> ";
		std::cout<<"start."<<std::endl;
	}
	
	double scaling_factor_for_star_image;
	simple_image resized_star_image = star_image.get_resized_image
	(height_of_alignment_image,&scaling_factor_for_star_image);

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_star_image_with_overlay() -> ";
		std::cout<<"got resized star image."<<std::endl;
	}
	
	//==================================================================
	// add information
	//==================================================================
	int number_of_pixels_of_image = 
	resized_star_image.get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);
	

	double circle_radius_in_pixel_per_image_line = 25.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 1.0/1e3;
	
	// circle spcific information
	double circle_radius_in_pixel = 
	circle_radius_in_pixel_per_image_line*mean_number_of_image_lines;
	int circle_thickness_in_pixel = 
	(int)ceil(
	circle_thickness_in_pixel_per_image_line*mean_number_of_image_lines
	);
	int circle_lineType	=8;		
	
	// exposure time
	std::stringstream text_to_put_in_image;
	text_to_put_in_image<<star_image.
	get_exposure_time_in_ms();
	text_to_put_in_image<<"ms";
	
	resized_star_image.text_overlay(text_to_put_in_image.str(),
	0.1,0.1,1.0);
	
	// star position
	cv::Point star_position_in_original;
	cv::Point star_position_in_scaled_image;
	if(a_single_star_has_been_found()){
		
		star_position_in_original = 
		sccan_star.position_in_pixels;
		
		star_position_in_scaled_image.y = (int)
		((double)star_position_in_original.x*
		scaling_factor_for_star_image);
		
		star_position_in_scaled_image.x = (int)
		((double)star_position_in_original.y*
		scaling_factor_for_star_image);
		
		cv::Point circle_position_openCV_bug_x_y_switched;
		circle_position_openCV_bug_x_y_switched.x = 
		star_position_in_scaled_image.y;
		circle_position_openCV_bug_x_y_switched.y = 
		star_position_in_scaled_image.x;
		
		// star circle
		cv::circle( 
		resized_star_image.image_matrix,
		circle_position_openCV_bug_x_y_switched,
		circle_radius_in_pixel,
		cv::Scalar( 0, 0, 255),
		circle_thickness_in_pixel,
		circle_lineType );
				
		// light flux
		text_to_put_in_image.str("");
		text_to_put_in_image.precision(3);
		text_to_put_in_image<<
		sccan_star.light_flux_of_star_in_bulbs;
		text_to_put_in_image<<"bulbs";
		
		cv::Point light_flux_text_position = 
		circle_position_openCV_bug_x_y_switched + 
		cv::Point(2*circle_radius_in_pixel,0);
		
		
		resized_star_image.text_overlay(text_to_put_in_image.str(),
		light_flux_text_position,1.0);

	}else{
		resized_star_image.text_overlay(
		"No single star found!",0.5,0.5,2.0);
	}		
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_star_image_with_overlay() -> ";
		std::cout<<"end."<<std::endl;
	}
	
	return resized_star_image;
}
//======================================================================
simple_image sccan_point_pair::get_resized_reflector_image
(int height_of_alignment_image){

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_reflector_image_with_overlay() -> ";
		std::cout<<"start."<<std::endl;
	}
	
	double scaling_factor_for_reflector_image;
	
	simple_image resized_reflector_image = 
	reflector_image.get_resized_image
	(height_of_alignment_image,&scaling_factor_for_reflector_image);

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_reflector_image_with_overlay() -> ";
		std::cout<<"got resized reflector image."<<std::endl;
	}
		
	return resized_reflector_image;
}
//======================================================================
void sccan_point_pair::get_reflector_image_exposure_time_stamp
(simple_image* pointer_to_reflector_image_for_overlay){

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_reflector_image_exposure_time_stamp() -> ";
		std::cout<<"start."<<std::endl;
	}

	//==================================================================
	// add information
	//==================================================================
	std::stringstream exposure_time_prompt;
	exposure_time_prompt<<reflector_image.get_exposure_time_in_ms();
	exposure_time_prompt<<"ms";
	
	pointer_to_reflector_image_for_overlay->text_overlay(
	exposure_time_prompt.str(),0.1,0.1,1.0);
	
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_reflector_image_exposure_time_stamp() -> ";
		std::cout<<"end."<<std::endl;
	}
		
}
//======================================================================
// analysis
//======================================================================
sccan_analysis_point* sccan_point_pair::calculate_sccan_analysis_point(
list_of_pixel_positions* pointer_to_list_of_pixel_positions_of_specific_mirror,
uint mirror_ID
){
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"calculate_sccan_analysis_point() -> ";
		std::cout<<"start."<<std::endl;
	}
	
	if(single_star_has_been_found){
		//single_star_has_been_found
		
		sccan_analysis_point* pointer_to_a_new_sccan_analysis_point;

		pointer_to_a_new_sccan_analysis_point = 
		new sccan_analysis_point;
		
		pointer_to_a_new_sccan_analysis_point->
		toggle_verbosity(verbosity);
		
		//==============================================================
		// fill sccan analysis point
		//==============================================================
		pointer_to_a_new_sccan_analysis_point->
		set_mirror_ID(mirror_ID);
	
		pointer_to_a_new_sccan_analysis_point->
		set_star_position_relative_to_pointing_direction(
		sccan_star.star_direction_relative_to_camera
		);
	
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"calculate_sccan_analysis_point() -> ";
			std::cout<<"number of pixels in mask ";
			std::cout<<pointer_to_list_of_pixel_positions_of_specific_mirror->size();
			std::cout<<std::endl;
		}
		
		double mean_light_flux_of_specific_reflector_mirror_in_bulbs = 
		reflector_image.
		mean_light_flux_in_bulbs_of_complex_image_region(
		pointer_to_list_of_pixel_positions_of_specific_mirror);
		
		double normalized_light_flux = 
		mean_light_flux_of_specific_reflector_mirror_in_bulbs/
		sccan_star.light_flux_of_star_in_bulbs;
		
		//~ pointer_to_a_new_sccan_analysis_point->
		//~ set_normalized_light_flux(normalized_light_flux);

		pointer_to_a_new_sccan_analysis_point->
		set_light_flux(
		sccan_star.light_flux_of_star_in_bulbs,
		mean_light_flux_of_specific_reflector_mirror_in_bulbs
		);	
			
		return pointer_to_a_new_sccan_analysis_point;
	
	}else{
		//no single_star_has_been_found	
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"calculate_sccan_analysis_point() -> ";
		std::cout<<"was called but there was no single ";
		std::cout<<"star found in this sccan point ";
		std::cout<<sccan_point_name_prefix;
		std::cout<<std::endl;

		return NULL;
	}
	
}
//======================================================================
