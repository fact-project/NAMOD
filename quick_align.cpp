#include "quick_align.h"
//======================================================================
quick_align::quick_align(
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
void quick_align::start_alignment(){
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
void quick_align::run_alignment(){
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
			case 'h' : {
				display_help();
			};
			break;
			default: ;
		}
	}
	cv::destroyWindow(window_name_of_alignment_image.c_str());
}
//======================================================================
void quick_align::display_help(){
	std::cout<<"quick_align -> start_alignment() -> ";
	std::cout<<"help: Press ESC to end quick alignment."<<std::endl;
}
//======================================================================
void quick_align::update_alignment_window(){
	
	acquire_sccan_point_for_quick_alignment();
	
	cv::imshow(window_name_of_alignment_image.c_str(),
	alignment_image.image_matrix); 
}
//======================================================================
void quick_align::acquire_sccan_point_for_quick_alignment(){

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
void quick_align::update_alignment_window(sccan_point_pair current_sccan_point){

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
std::string quick_align::get_quick_alignment_prompt(){
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
void quick_align::interaction(){
	
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
void quick_align::toggle_verbosity(){
	verbosity = !verbosity;
	alignment_image.toggle_verbosity(verbosity);
}
//======================================================================
