#include "verbosity_handler.h"
//======================================================================
verbosity_handler::verbosity_handler(
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
std::string verbosity_handler::get_verbosity_menu_prompt(){
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
void verbosity_handler::interaction(){
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
void verbosity_handler::toggle_all_verbosity(bool new_verbosity){
	
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
