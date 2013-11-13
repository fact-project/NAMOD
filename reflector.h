//=================================
// include guard
#ifndef __REFLECTOR_H_INCLUDED__
#define __REFLECTOR_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies
#include <vector>
#include "mirror.h"
#include "user_interaction.h"
#include "sccan_image.h"
#include "global_variables.h"

//=================================
class reflector 				:public user_interaction{
private:
	std::string		reflector_name;
		
	// general reflector properties
	double tripod_radius_in_m;
	double pitch_of_bolt_in_m_per_revolution;
	
	// reflector calibration sccan_image
	std::string		name_of_calibration_image;
	sccan_image		reflector_calibration_image;
	bool 			flag_calibration_image_with_polygons_created;
	sccan_image		reflector_calibration_image_with_polygons;
	std::string		name_of_calibration_window_with_highlighted_poygons;
	
	// reflector configuration text file
	std::string		file_name_extension_for_configuration_file;
	std::string 		reflector_configuration_file_name;
	std::string		key_list_of_mirrors;
	std::string		key_reflector;
	std::string		key_reflector_name;
	std::string		key_reflector_calibration_image;
	std::string		key_tripod_radius_in_m;
	std::string		key_pitch_of_bolt_in_m_per_revolution;
	
	// pointer for static mouse call back
	reflector* pObj;
//======================================================================
public:
//======================================================================
reflector();
//======================================================================
void initialize_default_reflector();
//======================================================================
void interaction();
//======================================================================
void set_reflector_name(std::string new_reflector_name);
//======================================================================
bool set_reflector_calibration_image(std::string image_file_name);
//======================================================================
void disp_status();
//======================================================================
bool read_configuration(
std::string file_name_of_reflector_configuration_to_load);
//======================================================================
bool write_configuration_to_disk();
//======================================================================
bool save_calibration_image_with_mirror_polygons();
//======================================================================
std::string get_reflector_prompt();
//======================================================================
std::string get_reflector_name();
//======================================================================
// mirror polygon manipulation 
//======================================================================
void update_calibration_image_with_polygons(bool highlight);
//======================================================================
void manipulate_mirror_polygon();
//======================================================================
static void onMouse(int event, int x, int y, int flags, void *param);
//======================================================================
void add_point_to_polygon_point_list_of_mirror(int pixel_x, int pixel_y);
//======================================================================
void pop_point_of_polygon_point_list_of_mirror();
//======================================================================
void toggle_verbosity();
//======================================================================
// list of mirrors
//======================================================================
std::vector<mirror*> list_of_pointers_to_mirrors;
mirror 			*pointer_to_current_mirror_to_work_with;
//======================================================================
void initialize_mirror_list();
//======================================================================
bool choose_mirror_ID_to_work_with(uint mirror_ID_to_work_with);
//======================================================================
bool add_mirror(uint new_mirror_ID);
//======================================================================
bool pop_mirror(uint old_mirror_ID);
//======================================================================
bool is_there_a_mirror_to_work_with();
//======================================================================
int get_number_of_mirrors_on_reflector();
//======================================================================
list_of_pixel_positions* get_pointer_to_list_of_Points_inside_mirror_polygon
(int mirror_itterator);
//======================================================================
void show_mirror_status();
//======================================================================
};

#endif // __REFLECTOR_H_INCLUDED__
