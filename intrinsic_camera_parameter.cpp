#include "intrinsic_camera_parameter.h"
//======================================================================
intrinsic_camera_parameter::
intrinsic_camera_parameter(){
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"intrinsic_camera_parameter()";
		std::cout<<std::endl;
	}
	
	name_of_camera = "default camera";
	name_of_optical_system = "default optical system";
	degrees_per_pixel = 3.34375E-3;
	
	radiometric_coefficient_for_log_exp_time = 0.0;
	radiometric_coefficient_for_log_camera_response = 0.0;
	radiometric_coefficient_constant = 1.0;
	
	filename_extension = ".int";
	key_intrinsic_parameter = "intrinsic_parameter";
	key_name_of_camera = "camera";
	key_name_of_optical_system = "optics";
	key_sccan_point_name_prefix = "sccan_point";
	key_camera_sensor_width = "camera_sensor_width";
	key_camera_sensor_height = "camera_sensor_height";
	key_simple_pixel2angle = "pixel2angle";
	
	key_radiometric_coefficient_for_log_exp_time = "polynom_coeff_2";
	key_radiometric_coefficient_for_log_camera_response = "polynom_coeff_1";
	key_radiometric_coefficient_constant = "polynom_coeff_0";
}
//======================================================================
void intrinsic_camera_parameter::
set_camera_sensor_size(int width,int height){
	camera_sensor_size.width = (double)width;
	camera_sensor_size.height = (double)height;
}
//======================================================================
void intrinsic_camera_parameter::
set_names(
std::string new_camera_name,
std::string new_name_of_optical_system){
	name_of_camera = new_camera_name;
	name_of_optical_system = new_name_of_optical_system;
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"set_names() -> ";
		std::cout<<"camera: "<<name_of_camera<<", optics: ";
		std::cout<<name_of_optical_system;
		std::cout<<std::endl;
	}
}
//======================================================================
void intrinsic_camera_parameter::
set_coefficients_for_radiometric_correction_plane(	
double new_radiometric_coefficient_for_log_exp_time,
double new_radiometric_coefficient_for_log_camera_response,
double new_radiometric_coefficient_constant){
	
	radiometric_coefficient_for_log_exp_time = 
	new_radiometric_coefficient_for_log_exp_time;
	
	radiometric_coefficient_for_log_camera_response = 
	new_radiometric_coefficient_for_log_camera_response;
	
	radiometric_coefficient_constant = 
	new_radiometric_coefficient_constant;
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"set_coefficients_for_radiometric_correction_plane()";
		std::cout<<" -> ";
		std::cout<<radiometric_coefficient_for_log_exp_time<<", ";
		std::cout<<radiometric_coefficient_for_log_camera_response<<", ";
		std::cout<<radiometric_coefficient_constant;		
		std::cout<<std::endl;
	
	}		
}
//======================================================================
void intrinsic_camera_parameter::
set_FoV_to_pixel_mapping(const double new_degrees_per_pixel){
	degrees_per_pixel = new_degrees_per_pixel;
}
//======================================================================
std::string 
intrinsic_camera_parameter::
get_status_prompt(){
	std::stringstream out;
	out<<"| intrinsic parameters"<<std::endl;
	out<<"| camera......: "<<name_of_camera<<std::endl;
	out<<"| optics......: "<<name_of_optical_system<<std::endl;
	out<<"| degrees_per_pixel..: "<<degrees_per_pixel<<std::endl;
	out<<"| sensor size: "<<camera_sensor_size.width<<"x";
	out<<camera_sensor_size.height<<" pixels"<<std::endl;

	return out.str();
}
//======================================================================
void intrinsic_camera_parameter::
disp(){
	std::cout<<get_status_prompt();
}
//======================================================================
bool intrinsic_camera_parameter::
save_intrinsic_parameter(std::string sccan_point_name_prefix){
	
	std::string intrinsic_parameter_file_name_to_save = 
	sccan_point_name_prefix +
	filename_extension;
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"save_intrinsic_camera_parameters() -> ";
		std::cout<<intrinsic_parameter_file_name_to_save;
		std::cout<<std::endl;
	}
	
	libconfig::Config intrinsic_parameter_configuration_handle;
	libconfig::Setting &root = intrinsic_parameter_configuration_handle.getRoot();

	// Add some settings to the configuration.
	libconfig::Setting &intrinsic_parameter_settings = 
	root.add(key_intrinsic_parameter, libconfig::Setting::TypeGroup);
	
	intrinsic_parameter_settings.
	add(key_sccan_point_name_prefix,libconfig::Setting::TypeString) =
	sccan_point_name_prefix;
	
	intrinsic_parameter_settings.
	add(key_name_of_camera,libconfig::Setting::TypeString) =
	name_of_camera;

	intrinsic_parameter_settings.
	add(key_name_of_optical_system,libconfig::Setting::TypeString) =
	name_of_optical_system;	

	// camera sensor size
	intrinsic_parameter_settings.
	add(key_camera_sensor_width,libconfig::Setting::TypeInt) =
	(int)camera_sensor_size.width;	

	intrinsic_parameter_settings.
	add(key_camera_sensor_height,libconfig::Setting::TypeInt) =
	(int)camera_sensor_size.height;	

	// first order deg/px
	intrinsic_parameter_settings.
	add(key_simple_pixel2angle,libconfig::Setting::TypeFloat) =
	degrees_per_pixel;	
	
	//polynom coefficients
	intrinsic_parameter_settings.
	add(key_radiometric_coefficient_for_log_exp_time,
	libconfig::Setting::TypeFloat) =
	radiometric_coefficient_for_log_exp_time;	

	intrinsic_parameter_settings.
	add(key_radiometric_coefficient_for_log_camera_response,
	libconfig::Setting::TypeFloat) =
	radiometric_coefficient_for_log_camera_response;	
	
	intrinsic_parameter_settings.
	add(key_radiometric_coefficient_constant,libconfig::Setting::TypeFloat) =
	radiometric_coefficient_constant;	
	
	// Write out the new configuration.
	try
	{
		intrinsic_parameter_configuration_handle.
		writeFile(intrinsic_parameter_file_name_to_save.c_str());
		if(verbosity){
			std::cout << "New configuration successfully written to: "<< 
			intrinsic_parameter_file_name_to_save<<std::endl;
		}
	}
	catch(const libconfig::FileIOException &fioex)
	{
		std::cerr << "I/O error while writing file: "<<
		intrinsic_parameter_file_name_to_save<<std::endl;
		return false;
	}
	return true;	
}
//======================================================================
bool intrinsic_camera_parameter::
load_intrinsic_camera_parameters(std::string sccan_point_name_prefix){

	libconfig::Config intrinsic_parameter_configuration_handle;
	//==================================================================
	// Read the file. If there is an error, report it and exit.
	//==================================================================
	std::string 
	intrinsic_parameter_file_name_to_load = 
	sccan_point_name_prefix+
	filename_extension;
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<intrinsic_parameter_file_name_to_load<<std::endl;
	}
	
	try{
		intrinsic_parameter_configuration_handle.
		readFile(intrinsic_parameter_file_name_to_load.c_str());
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
	const libconfig::Setting& root = 
	intrinsic_parameter_configuration_handle.getRoot();
	
	bool reading_is_valid = true;
	
	// Output a list of all books in the inventory.
	try{
		const libconfig::Setting &Intinsic = root[key_intrinsic_parameter];

	//==================================================================
	// name of camera		
	//==================================================================
	std::string new_name_of_camera;
	if( Intinsic.lookupValue
	(key_name_of_camera, new_name_of_camera) ){
		name_of_camera = new_name_of_camera;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_name_of_camera<<" is set to ";
			std::cout<<name_of_camera<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_name_of_camera<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// name of optical system		
	//==================================================================
	std::string new_name_of_optical_system;
	if( Intinsic.lookupValue
	(key_name_of_optical_system, new_name_of_optical_system) ){
		name_of_optical_system = new_name_of_optical_system;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_name_of_optical_system<<" is set to ";
			std::cout<<name_of_optical_system<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_name_of_optical_system<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	// simple_pixel2angle		
	//==================================================================
	double new_pixel2angle;
	if( Intinsic.lookupValue
	(key_simple_pixel2angle, new_pixel2angle) ){
		degrees_per_pixel = new_pixel2angle;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_simple_pixel2angle<<" is set to ";
			std::cout<<degrees_per_pixel<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_simple_pixel2angle<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	// sensor width		
	//==================================================================
	int camera_sensor_width;
	if( Intinsic.lookupValue
	(key_camera_sensor_width, camera_sensor_width) ){
		camera_sensor_size.width = camera_sensor_width;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_camera_sensor_width<<" is set to ";
			std::cout<<camera_sensor_size.width<<" pixels"<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_camera_sensor_width<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// sensor height		
	//==================================================================
	int camera_sensor_height;
	if( Intinsic.lookupValue
	(key_camera_sensor_height, camera_sensor_height) ){
		camera_sensor_size.height = camera_sensor_height;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_camera_sensor_height<<" is set to ";
			std::cout<<camera_sensor_size.height<<" pixels"<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_camera_sensor_height<<" fail."<<std::endl;
		reading_is_valid = false;
	}					
	//==================================================================
	// polynom coefficients		
	//==================================================================
	// coeff 2
	double new_radiometric_coefficient_for_log_exp_time;
	if( Intinsic.lookupValue
	(key_radiometric_coefficient_for_log_exp_time, 
	new_radiometric_coefficient_for_log_exp_time) ){
		radiometric_coefficient_for_log_exp_time = 
		new_radiometric_coefficient_for_log_exp_time;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_radiometric_coefficient_for_log_exp_time<<" is set to ";
			std::cout<<radiometric_coefficient_for_log_exp_time<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_radiometric_coefficient_for_log_exp_time<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	// coeff 1
	double new_radiometric_coefficient_for_log_camera_response;
	if( Intinsic.lookupValue
	(key_radiometric_coefficient_for_log_camera_response, 
	new_radiometric_coefficient_for_log_camera_response) ){
		radiometric_coefficient_for_log_camera_response = 
		new_radiometric_coefficient_for_log_camera_response;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_radiometric_coefficient_for_log_camera_response<<" is set to ";
			std::cout<<radiometric_coefficient_for_log_camera_response<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_radiometric_coefficient_for_log_camera_response<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	// coeff 0
	double new_radiometric_coefficient_constant;
	if( Intinsic.lookupValue
	(key_radiometric_coefficient_constant, new_radiometric_coefficient_constant) ){
		radiometric_coefficient_constant = new_radiometric_coefficient_constant;
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"load_intrinsic_camera_parameters() -> ";
			std::cout<<key_radiometric_coefficient_constant<<" is set to ";
			std::cout<<radiometric_coefficient_constant<<std::endl;
		}
	}else{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<key_radiometric_coefficient_constant<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	}
	catch(const libconfig::SettingNotFoundException &nfex)
	{
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		std::cout<<"loading failed! Settings not found!"<<std::endl;
		reading_is_valid  = false;
	}
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"load_intrinsic_camera_parameters() -> ";
		if(reading_is_valid){
			std::cout<<"success";
		}else{
			std::cout<<"fail";
		}
		std::cout<<std::endl;
	}
	
	return reading_is_valid;	
}
//======================================================================
pointing_direction intrinsic_camera_parameter::
calculate_pointing_direction(cv::Point position){
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_pointing_direction() -> ";
		std::cout<<"Point ("<<position.x<<"|"<<position.y<<")"<<std::endl;
	}
	
	//==================================================================
	// very simple correction of intrinsic parameters
	//==================================================================
	// calculate image center
	double middle_in_sensor_width = 
	(double)camera_sensor_size.width/2.0;
	
	double middle_in_sensor_height = 
	(double)camera_sensor_size.height/2.0;
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_pointing_direction() -> ";
		std::cout<<"image center ";
		std::cout<<"("<<middle_in_sensor_width<<"|";
		std::cout<<middle_in_sensor_height<<")"<<std::endl;
	}

	// transform to frame centered in image 
	double horizontal_position_in_pixels_in_center_frame =
	-middle_in_sensor_width + position.x;
	double vertical_position_in_pixels_in_center_frame = 
	+middle_in_sensor_height - position.y;
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_pointing_direction() -> ";
		std::cout<<"Point in center frame ";
		std::cout<<"("<<horizontal_position_in_pixels_in_center_frame<<"|";
		std::cout<<vertical_position_in_pixels_in_center_frame<<")"<<std::endl;
	}
	
	//transform to polar coordiantes
	double r_in_pixels_center_frame = sqrt(
	pow(horizontal_position_in_pixels_in_center_frame,2.0) + 
	pow(vertical_position_in_pixels_in_center_frame,2.0));
	
	double phi_in_radiant_in_center_frame = atan2(
	vertical_position_in_pixels_in_center_frame,
	horizontal_position_in_pixels_in_center_frame);
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_pointing_direction() -> ";
		std::cout<<"Point in center frame polar ";
		std::cout<<"r="<<r_in_pixels_center_frame<<"pixels ";
		std::cout<<"phi="<<phi_in_radiant_in_center_frame<<"rad"<<std::endl;
	}
	
	// take distortion into account
	// here a polynomial distortion function of at least order
	// 2 has to be/might be used
	double r_in_degrees_in_center_frame = 
	r_in_pixels_center_frame*degrees_per_pixel;
	
	double x_tilt_in_degrees = r_in_degrees_in_center_frame*cos(
	phi_in_radiant_in_center_frame);
	double y_tilt_in_degrees = r_in_degrees_in_center_frame*sin(
	phi_in_radiant_in_center_frame);	
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_pointing_direction() -> ";
		std::cout<<"pointing direction ";
		std::cout<<"x-tilt="<<x_tilt_in_degrees<<"deg, ";
		std::cout<<"y-tilt="<<y_tilt_in_degrees<<"deg"<<std::endl;
	}
	
	pointing_direction direction_of_point;
	direction_of_point.direction_in_x_in_radiant = 
	x_tilt_in_degrees*((2.0*M_PI)/360.0);
	
	direction_of_point.direction_in_y_in_radiant = 
	y_tilt_in_degrees*((2.0*M_PI)/360.0);

	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_pointing_direction() -> ";
		std::cout<<"return pointing direction in radiant";
		std::cout<<"x-tilt=";
		std::cout<<direction_of_point.direction_in_x_in_radiant <<"rad, ";
		std::cout<<"y-tilt=";
		std::cout<<direction_of_point.direction_in_x_in_radiant <<"rad";
		std::cout<<std::endl;
	}
	
	return direction_of_point;
}
//======================================================================
double intrinsic_camera_parameter::
calculate_absolute_light_flux(
double exposure_time_in_ms,
double normalized_pixel_value){

	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_absolute_light_flux() -> ";
		std::cout<<"exposure time "<<exposure_time_in_ms<<"[ms], ";
		std::cout<<"normalized pixel value "<<normalized_pixel_value<<"[1]";
		std::cout<<std::endl;
	}
	
	if(normalized_pixel_value<0.0 || normalized_pixel_value>1.0){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_absolute_light_flux() -> ";
		std::cout<<"normalized pixel value must be in range (0,1] but is ";
		std::cout<<normalized_pixel_value<<std::endl;
		std::cout<<std::endl;		
		return 0;
	}
	
	if(normalized_pixel_value==0){
		if(verbosity){
			std::cout<<"intrinsic_camera_parameter -> ";
			std::cout<<"calculate_absolute_light_flux() -> ";
			std::cout<<"normalized pixel value is ZERO";
			std::cout<<std::endl;
		}
		return 0;		
	}
	
	if(exposure_time_in_ms<0.0){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_absolute_light_flux() -> ";
		std::cout<<"exposure_time_in_ms < 0.0!";
		std::cout<<std::endl;		
		return 0;
	}	
	
	double log_of_exposure_time_in_s = 
	log10(exposure_time_in_ms/1000.0);
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_absolute_light_flux() -> ";
		std::cout<<"log10 of exposure time "<<log_of_exposure_time_in_s;
		std::cout<<std::endl;
	}
	
	double log_of_normalized_camera_response =
	log10(normalized_pixel_value);
	
	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_absolute_light_flux() -> ";
		std::cout<<"log10 of normalized camera response ";
		std::cout<<log_of_normalized_camera_response;
		std::cout<<std::endl;
	}
	
	double absolute_light_flux = pow(10.0,
	radiometric_coefficient_for_log_exp_time*
	log_of_exposure_time_in_s+ 
	radiometric_coefficient_for_log_camera_response*
	log_of_normalized_camera_response+
	radiometric_coefficient_constant	
	);

	if(verbosity){
		std::cout<<"intrinsic_camera_parameter -> ";
		std::cout<<"calculate_absolute_light_flux() -> ";
		std::cout<<"absolute_light_flux = "<<absolute_light_flux<<"bulbs";
		std::cout<<std::endl;
	}
	
	return absolute_light_flux;
}
//======================================================================

