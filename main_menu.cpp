#include "main_menu.h"
//======================================================================
main_menu::main_menu(
snapshot *new_pointer_to_snapshot,
reflector *new_pointer_to_reflector,
sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
quick_align *new_pointer_to_quick_align,
sccan_point_analysis *new_pointer_to_sccan_point_analysis,
verbosity_handler *new_pointer_to_verbosity_handler,
ueye_camera *new_pointer_to_star_camera,
ueye_camera *new_pointer_to_reflector_camera,
star_recognition_test_environment *new_pointer_to_test_environment){
	
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
	
	pointer_to_test_environment = new_pointer_to_test_environment;
	
	menu_name = "Mirror alignment main menu";
}
//======================================================================
std::string main_menu::get_main_menu_status_prompt(){
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
void main_menu::interaction(){
	
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
	
	std::string key_start_test_environment ="t";
	add_control(key_start_test_environment,	"start test environment");
	
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
	if(valid_user_input.compare(key_start_test_environment)==0){
		pointer_to_test_environment->interaction();
	} 
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_quick_align_a_mirror)==0){
		
		if(
		pointer_to_star_camera->camera_status()
		&&
		pointer_to_reflector_camera->camera_status()
		){
			pointer_to_quick_align->interaction();
		}else{
			std::cout << "main_menu -> interaction -> ";
			std::cout << "camera(s) offline !\n";
		}
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_verbosity)==0){
		pointer_to_verbosity_handler->interaction();
	}
	//==================================================================
}while(flag_user_wants_to_align_mirrors);
}
//======================================================================
