#include "sccan_image.h"
//======================================================================
sccan_image::sccan_image(){
	initialize(false);
}
//======================================================================
sccan_image::sccan_image(bool new_verbosity){
	initialize(new_verbosity);
}
//======================================================================
void sccan_image::initialize(bool new_verbosity){
	verbosity = new_verbosity;
	image_initialized = false;
	file_name_extension_for_image = ".png";
	file_name_extension_for_info = ".info";

	name_of_image_info_block = "sccan_image";
	name_of_image_name_info = "image_name";
	name_of_image_file_name_info = "image_file_name";
	name_of_exposure_time_in_ms_info = "exposure_time_in_ms";
	name_of_camera_ID_info = "ueye_camera_ID";
	name_of_time_since_startup_in_seconds =
	"time_since_startup_in_seconds";
	//initialize(480,640);
	image_name = "";
	exposure_time_in_ms = 0.0;
	ID_of_ueye_camera_taken_this_image = 0;
	time_since_startup_in_seconds = 0.0; 
}
//======================================================================
void sccan_image::set_name(std::string new_image_name){
	image_name = new_image_name;
	//create_image_file_name();
}
//======================================================================
void sccan_image::set_intrinsic_parameters(
intrinsic_camera_parameter new_intrinsic_parameters){
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"set_intrinsic_parameters()";
		std::cout<<std::endl;			
	}
	intrinsic_parameters = new_intrinsic_parameters;
}
//======================================================================
std::string sccan_image::get_file_name(){
	return (image_name+file_name_extension_for_image );
}
//======================================================================
uint sccan_image::get_camera_ID_of_camera_taken_this_image(){
	return ID_of_ueye_camera_taken_this_image;
}
//======================================================================
std::string sccan_image::get_name(){
	return image_name;
}
//======================================================================
bool sccan_image::export_image_to_disk(){

	// save image
	if(!save_simple_image(
	image_name+file_name_extension_for_image)){
		return false;
	};
	
	// save imaeg info
	if(write_image_info(image_name+file_name_extension_for_info)){
		std::cout<<"sccan_image -> export_image_to_disk() -> ";
		std::cout<<"config "<<image_name+file_name_extension_for_info;
		std::cout<<" exportd."<<std::endl;		
	}else{
		std::cout<<"sccan_image->export_image_to_disk()->ERROR->";
		std::cout<<image_name+file_name_extension_for_info;
		std::cout<<"Error while exporting sccan_image config data. ";
		return false;
	}
	
	//save intrinsic parameters
	if(intrinsic_parameters.save_intrinsic_parameter(
	image_name)==false){
		std::cout<<"sccan_image -> export_image_to_disk() -> ";
		std::cout<<"ERROR saving intrinsic parameters";
		std::cout<<std::endl;
		return false;
	}	
	
	if(verbosity){
		std::cout<<"sccan_image -> export_image_to_disk() -> ";
		std::cout<<"sccan_image "<<image_name<<" exported."<<std::endl;
	}
	
	return true;	
}
//======================================================================
bool sccan_image::write_image_info
(std::string image_configuration_file_name){
	libconfig::Config image_configuration_handle;
	
	libconfig::Setting &root = image_configuration_handle.getRoot();

	// Add some settings to the configuration.
	libconfig::Setting &image_info = 
	root.add(name_of_image_info_block, libconfig::Setting::TypeGroup);

	image_info.
	add(name_of_image_name_info,libconfig::Setting::TypeString)= 
	image_name;
	
	image_info.
	add(name_of_exposure_time_in_ms_info,libconfig::Setting::TypeFloat)= 
	exposure_time_in_ms;
	
	image_info.
	add(name_of_camera_ID_info, libconfig::Setting::TypeInt) = 
	(int)ID_of_ueye_camera_taken_this_image;

	image_info.
	add(name_of_time_since_startup_in_seconds, 
	libconfig::Setting::TypeFloat) = 
	time_since_startup_in_seconds;
	
	// Write out the new configuration.
	try
	{
		image_configuration_handle.
		writeFile(image_configuration_file_name.c_str());
		if(verbosity){
			std::cout << "New configuration successfully written to: ";
			std::cout << image_configuration_file_name << std::endl;
		}
	}
	catch(const libconfig::FileIOException &fioex)
	{
		std::cerr << "I/O error while writing file: ";
		std::cerr << image_configuration_file_name << std::endl;
		return(false);
	}

	return(true);	
}
//======================================================================
double sccan_image::get_exposure_time_in_ms(){
	if(verbosity){
		std::cout<<"sccan_image->get_exposure_time_in_ms()->";
		std::cout<<exposure_time_in_ms<<"ms"<<std::endl;
	}
	return exposure_time_in_ms;
}
//======================================================================
void sccan_image::set_exposure_time_in_ms
(double new_exposure_time_in_ms){
	exposure_time_in_ms = new_exposure_time_in_ms;
	if(verbosity){
		std::cout<<"sccan_image->set_exposure_time_in_ms()->";
		std::cout<<exposure_time_in_ms<<"ms"<<std::endl;
	}
	
}
//======================================================================
void sccan_image::initialize_image_matrix(int rows, int cols){
		image_matrix = cv::Mat::zeros(rows,cols,CV_8UC3);
		image_initialized = true;
}
//======================================================================
bool sccan_image::load_image(std::string name_of_image_to_load){

	// read in data
	bool image_found =
	read_in_image(
	name_of_image_to_load+file_name_extension_for_image);
	
	bool image_info_found = 
	read_in_image_info(
	name_of_image_to_load+file_name_extension_for_info);
	
	bool intrinsic_parameters_found = intrinsic_parameters.
	load_intrinsic_camera_parameters(
	name_of_image_to_load);
	
	if(image_found && image_info_found && intrinsic_parameters_found){
		if(verbosity){
			std::cout<<"sccan_image -> load_image() loading successful.";
			std::cout<<std::endl;
		}
		image_initialized = true;
		return true;
	}else{
		std::cout<<"sccan_image -> load_image() loading failed!"<<std::endl;
		image_initialized = false;
		return false;
	}
}
//======================================================================
void sccan_image::set_camera_ID(uint ueye_camera_ID){
	ID_of_ueye_camera_taken_this_image = ueye_camera_ID;
	if(verbosity){
		std::cout<<"sccan_image->set_camera_ID()->";
		std::cout<<" camera ID has been set to: ";
		std::cout<<ID_of_ueye_camera_taken_this_image<<std::endl;			
	}
}
//======================================================================
void sccan_image::disp_image(){
	if(image_initialized){
		
		if(verbosity){
			std::cout<<"sccan_image -> ";
			std::cout<<"disp_image() -> ";
			std::cout<<"display initialized image "<<image_name<<std::endl;
		}
		
		disp_simple_image(image_name);
	}else{
		std::cout<<"sccan_image -> disp_image() ";
		std::cout<<"sccan_image not initialized yet!"<<std::endl;
	}
}	
//======================================================================
bool sccan_image::read_in_image_info
(std::string file_name_of_image_info_to_load){
	libconfig::Config image_configuration_handle;
	//==================================================================
	// Read the file. If there is an error, report it and exit.
	//==================================================================
	try
	{
		image_configuration_handle.
		readFile(file_name_of_image_info_to_load.c_str());
	}
	catch(const libconfig::FileIOException &fioex)
	{
		std::cout << "I/O error while reading file." << std::endl;
		return(false);
	}
	catch(const libconfig::ParseException &pex)
	{
		std::cout << "Parse error at " << pex.getFile() << ":";
		std::cout << pex.getLine() << " - " << pex.getError() << std::endl;
		return(false);
	}
	//==================================================================
	// get data
	//==================================================================
	const libconfig::Setting& root = 
	image_configuration_handle.getRoot();

	try
	{
		const libconfig::Setting &image_info = root[name_of_image_info_block];
		
		if
		(	(
			image_info.lookupValue
			(name_of_image_name_info, image_name) &&
			image_info.lookupValue
			(name_of_exposure_time_in_ms_info,exposure_time_in_ms) &&
			image_info.lookupValue
			(name_of_camera_ID_info,ID_of_ueye_camera_taken_this_image) &&
			image_info.lookupValue
			(name_of_time_since_startup_in_seconds,
			time_since_startup_in_seconds)
			)
		)
		{
			//create_image_file_name();
		}else{
			std::cout<<"sccan_image -> read_in_image_info() -> ";
			std::cout<<"failed to read info file."<<std::endl;
		}

  }
  catch(const libconfig::SettingNotFoundException &nfex)
  {
	std::cout<<"sccan_image -> read_in_image_info() ->";
	std::cout<<"loading failed! Settings not found!"<<std::endl;
  }
  return true;
}
//======================================================================
std::string sccan_image::get_info_string(){
	std::stringstream out;
	out<<" __image______________________________________________"<<std::endl;
	out<<"|"<<std::endl;
	out<<"| initialized........: ";
	if(image_initialized){out<<"true";}else{out<<"false";}
	out<<std::endl;
	out<<"| sccan_image name.........: "<< image_name <<std::endl;
	out<<"| exposure time......: "<< exposure_time_in_ms <<"ms"<<std::endl;
	out<<"| camera ID..........: ";
	out<<ID_of_ueye_camera_taken_this_image<<std::endl;
	out<<"|_____________________________________________________"<<std::endl;
	return out.str();
}
//======================================================================
void sccan_image::disp(){
	std::cout<<get_info_string();
	disp_image();
}
//======================================================================
bool sccan_image::get_init_status(){
	return image_initialized;
}
//======================================================================
void sccan_image::set_time_stamp_in_seconds_since_startup(){
	time_since_startup_in_seconds =  
	global_time_stamp_manager_instance.
	get_number_of_seconds_since_startup();	
}
//======================================================================
void sccan_image::operator=(sccan_image eq){
	if(verbosity){
		std::cout<<"sccan_image -> operator=() ->"<<std::endl;
	}
	verbosity 	= eq.verbosity;
	image_initialized 	= eq.image_initialized;
	exposure_time_in_ms = eq.exposure_time_in_ms;
	eq.image_matrix.copyTo(image_matrix);
	image_name			= eq.image_name;
	ID_of_ueye_camera_taken_this_image = 
	eq.ID_of_ueye_camera_taken_this_image;
	time_since_startup_in_seconds = 
	eq.time_since_startup_in_seconds;
	
	intrinsic_parameters = eq.intrinsic_parameters;
}
//======================================================================
void sccan_image::operator=(simple_image eq){
	if(verbosity){
		std::cout<<"sccan_image -> operator=() -> get_simple_image"<<std::endl;
	}
	verbosity 	= eq.verbosity;
	image_initialized 	= true;
	exposure_time_in_ms = 0.0;
	eq.image_matrix.copyTo(image_matrix);
	image_name			= "";
	ID_of_ueye_camera_taken_this_image = 0;
	time_since_startup_in_seconds = 0.0;
}
//======================================================================
std::vector<star_in_image> sccan_image::find_stars(double threshold_in_sigmas,
double radius_of_one_sigma_region_for_star_detection_in_degrees){
	
	bool show_control_images = false;
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_image() -> "<<std::endl;
		std::cout<<"||==================== START ==================||"<<std::endl;
	}	
	
	//==================================================================
	// star processing radius
	//==================================================================
	// check star radius
	double maximal_camera_field_of_view_in_degrees = 
	intrinsic_parameters.degrees_per_pixel * 
	((image_matrix.cols + image_matrix.rows)/2.0);
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_image() -> ";
		std::cout<<"maximal_camera_field_of_view ";
		std::cout<<maximal_camera_field_of_view_in_degrees<<"deg"<<std::endl;
	}
	
	if(maximal_camera_field_of_view_in_degrees <
	10.0*radius_of_one_sigma_region_for_star_detection_in_degrees){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_image() -> ";
		std::cout<<"WARNING radius of one sigma region for star detection ";
		std::cout<<"is in the same order of magnitude as the whole ";
		std::cout<<"field of view!"<<std::endl;		
	}
	
	double radius_of_one_sigma_region_for_star_detection_in_pixels =
	radius_of_one_sigma_region_for_star_detection_in_degrees/
	intrinsic_parameters.degrees_per_pixel;
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_image() -> ";
		std::cout<<"star sigma radius ";
		std::cout<<radius_of_one_sigma_region_for_star_detection_in_pixels;
		std::cout<<"px, ";
		std::cout<<radius_of_one_sigma_region_for_star_detection_in_degrees;
		std::cout<<"deg, ";
		std::cout<<intrinsic_parameters.degrees_per_pixel<<" deg/px";
		std::cout<<std::endl;
	}	


	
	//==================================================================	
	//copy image to work with
	//==================================================================
	simple_image star(verbosity);
	image_matrix.copyTo(star.image_matrix);

	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_image() -> ";
		std::cout<<"image_to_work_with was copied."<<std::endl;
		if(show_control_images)
		star.disp_simple_image("original image to find star");

	}
	//==================================================================	
	// substracting median blurr
	//==================================================================
	simple_image star_minus_median(verbosity);
	star_minus_median.image_matrix = 
	star.image_matrix - 
	star.create_median_image
	(10.0*radius_of_one_sigma_region_for_star_detection_in_pixels);

	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_star_image() -> ";
		std::cout<<"spatial related median was substracted."<<std::endl;
		if(show_control_images)
		star_minus_median.disp_simple_image("star_minus_median");
	}
	//==================================================================
	// calculate noise level of image
	//==================================================================
	double noise_sigma_of_star_image_minus_median_image  =
	star_minus_median.calculate_pixelvalue_standard_deviation();

	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_star_image() -> ";
		std::cout<<"noise level was calculated ";
		std::cout<<noise_sigma_of_star_image_minus_median_image<<"pxv"<<std::endl;
		
		std::stringstream window_name; 
		window_name<<"star minus median, ";
		window_name<<"sigma=";
		window_name<<noise_sigma_of_star_image_minus_median_image;
		//star_minus_median.disp_simple_image(window_name.str());
	}
	
	// decide whether noise is neglectable or not
	cv::Point brightest_point;
	double highest_pixel_value =0.0;
	star.get_brightest_pixel_of_image
	(&brightest_point,&highest_pixel_value);
	
	if(
	noise_sigma_of_star_image_minus_median_image*threshold_in_sigmas <
	0.1 * highest_pixel_value
	){
		if(verbosity){
			std::cout<<"sccan_image -> ";
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<"noise is neglectable because sigma*threshold = ";
			std::cout<<noise_sigma_of_star_image_minus_median_image*
			threshold_in_sigmas;
			std::cout<<"pxv and brightest pixel is ";
			std::cout<<highest_pixel_value;
			std::cout<<"pxv."<<std::endl;
		}
		
		noise_sigma_of_star_image_minus_median_image=
		(0.1*highest_pixel_value)/threshold_in_sigmas;
			
		if(verbosity){	
			std::cout<<"sccan_image -> ";
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<"new sigma ";
			std::cout<<noise_sigma_of_star_image_minus_median_image;
			std::cout<<std::endl;
		}
	}
	//==================================================================
	// applie gaussian blur
	//==================================================================
	int pixel_radius_for_gaussian = (int)
	(round(round(
	radius_of_one_sigma_region_for_star_detection_in_pixels)
	/2.0))*2.0+1.0;
	
	cv::Size gaussian_size;
	gaussian_size.height = pixel_radius_for_gaussian;
	gaussian_size.width  = pixel_radius_for_gaussian;
	
	GaussianBlur(
	star_minus_median.image_matrix,
	star_minus_median.image_matrix,
	gaussian_size, 0.0,0.0);
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_star_image() -> ";
		std::cout<<"gaussian blur was applied. ";
		std::cout<<"pixel_radius_for_gaussian "<<pixel_radius_for_gaussian;
		std::cout<<"px"<<std::endl;
		if(show_control_images)
			star_minus_median.
			disp_simple_image("star image with gaussian blurr");		
	}
	
	//==================================================================	
	//adaptiveThreshold
	//==================================================================
	simple_image star_minus_median_single_channel(verbosity);
	star_minus_median_single_channel = star_minus_median;
	
	cv::cvtColor(
	star_minus_median_single_channel.image_matrix,
	star_minus_median_single_channel.image_matrix,
	CV_BGR2GRAY);

	simple_image threshold_image(verbosity);
	threshold_image = star_minus_median;
	
	cv::cvtColor(
	threshold_image.image_matrix,
	threshold_image.image_matrix,
	CV_BGR2GRAY);
	
	double pixel_value_of_destination_image =  255.0;
	double offset =(-1.0)*
	threshold_in_sigmas*noise_sigma_of_star_image_minus_median_image;

	cv::adaptiveThreshold(
	star_minus_median_single_channel.image_matrix,
	threshold_image.image_matrix,
	pixel_value_of_destination_image,
	cv::ADAPTIVE_THRESH_MEAN_C,
	cv::THRESH_BINARY,
	pixel_radius_for_gaussian,
	
	offset
	);
	
	cv::cvtColor(
	threshold_image.image_matrix,
	threshold_image.image_matrix,
	CV_GRAY2BGR
	);
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_star_image() -> ";
		std::cout<<"adaptive threshold was applied.";
		std::cout<<"block size ";
		std::cout<<radius_of_one_sigma_region_for_star_detection_in_pixels;
		std::cout<<"px"<<std::endl;
		if(show_control_images)
			threshold_image.
			disp_simple_image("threshold image");	
	}
	//==================================================================
	//spatial clustering
	//==================================================================
	std::vector<list_of_pixel_positions>  list_of_clusters;
	list_of_clusters =threshold_image.
	spatial_clustering_of_threshold_mask();

	

	list_of_pixel_positions list_of_star_locations_in_pixels;
	for(uint cluster_itterator=0;
	cluster_itterator<list_of_clusters.size();
	cluster_itterator++
	){
		list_of_clusters.at(cluster_itterator).
		toggle_verbosity(verbosity);		
		
		list_of_star_locations_in_pixels.push_back(
		list_of_clusters.at(cluster_itterator).
		get_mean_of_pixel_distribution()
		);	
	}
	//==================================================================
	// list of stars in image
	//==================================================================
	std::vector<star_in_image> list_of_stars_in_image;

	for(int star_itterator=0;
	star_itterator<list_of_star_locations_in_pixels.size();
	star_itterator++
	){
		if(verbosity){
			std::cout<<"sccan_image -> ";
			std::cout<<"find_bright_star_in_star_image() -> ";		
			std::cout<<"calculate light flux and pointing direction for ";
			std::cout<<"star "<<star_itterator+1<<" of ";
			std::cout<<list_of_star_locations_in_pixels.size();
			std::cout<<std::endl;
		}
		
		cv::Vec3b intensity_in_pixel_values = 
		star_minus_median.image_matrix.at<cv::Vec3b>(
		list_of_star_locations_in_pixels.at(star_itterator));
		
		double light_flux_of_star_in_pixel_values = (
		double(intensity_in_pixel_values[0])+
		double(intensity_in_pixel_values[1])+
		double(intensity_in_pixel_values[2]))
		/3.0;
		
		if(verbosity){
			std::cout<<"sccan_image -> ";
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<"star light flux BGR ";
			std::cout<<double(intensity_in_pixel_values[0])<<"pxv, ";
			std::cout<<double(intensity_in_pixel_values[1])<<"pxv, ";
			std::cout<<double(intensity_in_pixel_values[2])<<"pxv";				
			std::cout<<std::endl;
		}		
		
		
		if(verbosity){
			std::cout<<"sccan_image -> ";
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<"star at (";
			std::cout<<list_of_star_locations_in_pixels.at(star_itterator).x;
			std::cout<<"|";
			std::cout<<list_of_star_locations_in_pixels.at(star_itterator).y;
			std::cout<<") ";
			std::cout<<"light flux "<<light_flux_of_star_in_pixel_values;
			std::cout<<"pxv";	
			std::cout<<std::endl;
		}		
		
		pointing_direction direction_of_star = 
		intrinsic_parameters.calculate_pointing_direction(
		list_of_star_locations_in_pixels.at(star_itterator));
		
		double absolute_light_flux_in_standard_bulbs = 
		intrinsic_parameters.calculate_absolute_light_flux(
		exposure_time_in_ms,
		light_flux_of_star_in_pixel_values/255.0
		);
		
		star_in_image star
		(absolute_light_flux_in_standard_bulbs,
		direction_of_star,
		list_of_star_locations_in_pixels.at(star_itterator)
		);
		list_of_stars_in_image.push_back(star);
	}


	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"find_bright_star_in_image() -> "<<std::endl;
		std::cout<<"||====================  END  ==================||"<<std::endl;
	}
	return list_of_stars_in_image;
}
//======================================================================
double sccan_image::mean_light_flux_in_bulbs_of_complex_image_region(
list_of_pixel_positions *pointer_to_pixels_of_complex_region){
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"mean_light_flux_in_bulbs_of_complex_image_region() -> ";
		std::cout<<pointer_to_pixels_of_complex_region->size()<<" pixels to check.";
		std::cout<<std::endl;	
	}
	
	double mean_light_flux_in_bulbs = 0.0;
	
	for(
	int pixel_itterator = 0; 
	pixel_itterator<pointer_to_pixels_of_complex_region->size(); 
	pixel_itterator++){
		
		cv::Vec3b intensity_in_pixel_values = image_matrix.
		at<cv::Vec3b>(pointer_to_pixels_of_complex_region->
		at(pixel_itterator));
		
		double light_flux_in_pixel_values = (
		double(intensity_in_pixel_values[0])+
		double(intensity_in_pixel_values[1])+
		double(intensity_in_pixel_values[2]))
		/3.0;
		
		double normalized_light_flux = light_flux_in_pixel_values/255.0;
		
		mean_light_flux_in_bulbs += intrinsic_parameters.
		calculate_absolute_light_flux(
		exposure_time_in_ms,
		normalized_light_flux
		);
	}
	
	mean_light_flux_in_bulbs = mean_light_flux_in_bulbs/
	pointer_to_pixels_of_complex_region->size();
	
	if(verbosity){
		std::cout<<"sccan_image -> ";
		std::cout<<"mean_light_flux_in_bulbs_of_complex_image_region() -> ";
		std::cout<<" flux "<<mean_light_flux_in_bulbs<<" bulbs.";
		std::cout<<std::endl;	
	}
	
	return mean_light_flux_in_bulbs;
	
}
//======================================================================
