#include "star_recognition_test_environment.h"
star_recognition_test_environment::star_recognition_test_environment(){
	image_has_been_loaded = false;
	test_mapping_in_degrees_per_pixel = 0.003;
	test_exposure_time_in_ms = 10.0;
	test_threshold_in_sigmas = 8.0;
	test_radius_of_one_sigma_region_for_star_detection_in_degrees = 0.05;
}
//======================================================================
void star_recognition_test_environment::
interaction(){
	
	std::string load_imaget_to_be_tested  ="l";
	add_control(load_imaget_to_be_tested,"load image to be tested");
	
	std::string show_imaget_to_be_tested  ="show";
	add_control(show_imaget_to_be_tested,"show image to be tested");	
	
	std::string start_star_recognition ="s";
	add_control(start_star_recognition,"start star recognition");

	std::string edit_threshold ="eS";
	add_control(edit_threshold,"edit Sigma threshold for star recognition");
	
	std::string edit_radius ="eR";
	add_control(edit_radius,"edit 1 sigma Radius for star recognition");
	
	std::string back_to_main_menu ="b";
	add_control(back_to_main_menu,"back to main menu");	

	std::string user_input;
	std::stringstream out;
	
	bool continue_testing_star_recognition = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_star_recognition_test_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	if(valid_user_input.compare(back_to_main_menu)==0){
		continue_testing_star_recognition = false;
	}
	//==============================================================
	if(valid_user_input.compare(show_imaget_to_be_tested)==0){
		show_image();
	}
	//==============================================================
	if(valid_user_input.compare(start_star_recognition)==0){
		test_star_recognition();
	}
	//==============================================================
	if(valid_user_input.compare(edit_threshold)==0){
		test_threshold_in_sigmas = 
		ask_user_for_non_zero_float(
		"threshold in sigmas of image noise level",
		0.1,1000.0);
	}
	//==============================================================
	if(valid_user_input.compare(edit_radius)==0){
		test_radius_of_one_sigma_region_for_star_detection_in_degrees = 
		ask_user_for_non_zero_float(
		"radius of 1 sigma region in DEG",
		0.00003,90.0);
	}
	//==================================================================
	if(valid_user_input.compare(load_imaget_to_be_tested)==0){
		
		std::string filename_of_image_to_be_tested = 
		get_string_by_user();
		
		load_image(filename_of_image_to_be_tested);
	}	
	//==================================================================
	}while(continue_testing_star_recognition);
}
//======================================================================
std::string star_recognition_test_environment::
get_star_recognition_test_prompt(){
	std::stringstream out, info;
	
	// image to be tested
	info.str("");
	if(image_has_been_loaded){
		info<<name_of_image;	
	}else{
		info<<"no image loaded";	
	}
	out<<make_nice_line_with_dots("| image to test",info.str());
	
	// test mapping 
	info.str("");
	info<<test_mapping_in_degrees_per_pixel<<"[DEG/pixel]";
	out<<make_nice_line_with_dots("| test mapping",info.str());
	
	// threshold
	info.str("");
	info<<test_threshold_in_sigmas<<"[1]";
	out<<make_nice_line_with_dots(
	"| threshold in sigmas for star detection",info.str());	
	
	// recog radius
	info.str("");
	info<<test_radius_of_one_sigma_region_for_star_detection_in_degrees;
	info<<"[DEG]";
	out<<make_nice_line_with_dots(
	"| radius of 1 sigma region for star detection",info.str());	
	
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
void star_recognition_test_environment::
test_star_recognition(){
	if(image_has_been_loaded){
				
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"starting test..."<<"\n";
		
		set_test_intrinsic_parameters();
		
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"intrinsic_parameters have been set"<<"\n";
		parameters_of_image_to_be_tested.disp();

		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"create sccan image..."<<"\n";	
		sccan_image sccan_image_to_be_tested;	

		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"sccan image -> set name: "<<name_of_image<<"\n";			
		sccan_image_to_be_tested.set_name(name_of_image);
	
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"sccan image -> copy image matrix of ";
		std::cout<<name_of_image<<"\n";	
		sccan_image_to_be_tested = image_to_be_tested;

		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"sccan image -> set intrinsic parameters\n";		
		sccan_image_to_be_tested.
		set_intrinsic_parameters(parameters_of_image_to_be_tested);
		
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"sccan image -> set exposure time to ";
		std::cout<<test_exposure_time_in_ms<<"ms\n";	
		sccan_image_to_be_tested.
		set_exposure_time_in_ms(test_exposure_time_in_ms);

		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"activate verbosity..."<<"\n";	
		sccan_image_to_be_tested.toggle_verbosity(true);
		
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"verbosity is: ";
		if(sccan_image_to_be_tested.verbosity_status()){
			std::cout<<"true\n";	
		}else{
			std::cout<<"false\n";	
		}	
		
		std::vector<star_in_image> stars_found_in_test_image =
		sccan_image_to_be_tested.find_stars(
		test_threshold_in_sigmas,
		test_radius_of_one_sigma_region_for_star_detection_in_degrees,
		true
		);

		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"create star overlay..."<<"\n";	
		
		// display all stars found in image
		// star ring colour in BGR
		cv::Scalar color_of_ring(0.0,0.0,255.0);
		simple_image image_to_be_tested_with_star_overlay;
		
		image_to_be_tested_with_star_overlay =
		sccan_image_to_be_tested.
		get_overlay_with_stars(stars_found_in_test_image,color_of_ring);
		
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"show image to test with star overlay..."<<"\n";	
		
		image_to_be_tested_with_star_overlay.
		disp_simple_image("image to be tested with star overlay");
		
	}else{
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"no image loaded yet!"<<"\n";
	}
	
}
//======================================================================
void star_recognition_test_environment::
load_image(std::string filename_of_image_to_be_tested){
	
	std::cout<<"testing_star_recognition->load_image->";
	std::cout<<filename_of_image_to_be_tested<<"\n";
	
	if(image_to_be_tested.read_in_image(filename_of_image_to_be_tested))
	{
		image_has_been_loaded = true;
		name_of_image = filename_of_image_to_be_tested;
		set_test_intrinsic_parameters();
	}
	
}
//======================================================================
void star_recognition_test_environment::
show_image(){
	
	if(image_has_been_loaded){
		image_to_be_tested.disp_simple_image("image to be tested");
	}else{
		std::cout<<"testing_star_recognition->show_image->";
		std::cout<<"no image loaded yet!"<<"\n";
	}
}
//======================================================================
void star_recognition_test_environment::
set_test_intrinsic_parameters(){

	parameters_of_image_to_be_tested.
	set_camera_sensor_size(
	image_to_be_tested.image_matrix.cols,
	image_to_be_tested.image_matrix.rows);

	parameters_of_image_to_be_tested.
	set_FoV_to_pixel_mapping(test_mapping_in_degrees_per_pixel);

	parameters_of_image_to_be_tested.
	set_coefficients_for_radiometric_correction_plane(1.0,1.0,1.0);

	parameters_of_image_to_be_tested.
	set_names("test_camera","test_optics");

}
