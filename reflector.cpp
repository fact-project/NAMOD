#include "reflector.h"
//======================================================================
reflector::reflector(ueye_camera *new_pointer_to_reflector_camera){
	verbosity = true;
	
	//mirror_list
	initialize_mirror_list();
	
	// reflector configuration text file
	file_name_extension_for_configuration_file = ".cfg";
	key_list_of_mirrors 					="mirrors";
	key_reflector 							="reflector";
	key_reflector_name			 			="reflector_name";
	key_reflector_calibration_image			="calibration_image";
	key_tripod_radius_in_m					="tripod_radius_in_m";
	key_pitch_of_bolt_in_m_per_revolution	=
	"pitch_of_bolt_in_m_per_revolution";
	
	initialize_default_reflector();
	menu_name = "reflector manipulation";
	
	reflector_calibration_image.
	toggle_verbosity(verbosity);
	reflector_calibration_image_with_polygons.
	toggle_verbosity(verbosity);
	
	pointer_to_reflector_camera = new_pointer_to_reflector_camera;
	show_tripods = false;
}
//======================================================================
void reflector::initialize_default_reflector(){
	pointer_to_current_mirror_to_work_with = NULL;

	list_of_pointers_to_mirrors.clear();
	
	reflector_name = "default_reflector";
	reflector_configuration_file_name =  reflector_name;
	reflector_configuration_file_name += ".cfg";
	
	name_of_calibration_image = "";
	
	name_of_calibration_window_with_highlighted_poygons = 
	"calibration sccan_image with polygons";
	
	tripod_radius_in_m = 0.295;
	pitch_of_bolt_in_m_per_revolution = 0.001;
	flag_calibration_image_with_polygons_created = false;
}
//======================================================================
void reflector::interaction(){	

	std::string key_manipulate_mirror = "m";
	add_control(key_manipulate_mirror,"manipulate current mirror ID");
	
	std::string key_choose_mirror_ID = "c";	
	add_control(key_choose_mirror_ID,"choose mirror ID to manipulate");

	std::string key_release_mirror_ID = "r";	
	add_control(key_release_mirror_ID,"release mirror ID to manipulate");
	
	std::string key_show_calibration_image = "s";	
	add_control(key_show_calibration_image,
	"show calibration sccan_image");
	
	std::string key_show_calibration_image_with_polygons = "sp";
	add_control(key_show_calibration_image_with_polygons,
	"show calibration image with mirror polygons");

	std::string key_toggle_tripods_in_images = "tt";
	add_control(key_toggle_tripods_in_images,
	"toggle tripods");

	std::string key_check_polygon_position_in_live_reflector_display = "cp";
	add_control(key_check_polygon_position_in_live_reflector_display,
	"check polygon position in live reflector display");
	
	std::string key_calibration_image = "i";	
	add_control(key_calibration_image,"choose calibration sccan_image");

	std::string key_change_name = "name";	
	add_control(key_change_name,"change reflector name");
	
	std::string key_show_mirror_status = "ms";	
	add_control(key_show_mirror_status,"show mirror status");
	
	std::string key_delete_current_mirror = "delete";	
	add_control(key_delete_current_mirror,"delete current mirror ID");	

	std::string key_save_reflector = "save";	
	add_control(key_save_reflector,"save reflector setup to file");
	
	std::string key_load_reflector = "load";	
	add_control(key_load_reflector,"load reflector setup from file");	
	
	std::string key_quit = "back";	
	add_control(key_quit,"back to main menu");
	
	std::stringstream out;
	bool user_wants_to_quit_mirror_manipulation = false;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_reflector_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==================================================================
	if(valid_user_input.compare(key_load_reflector)==0){	
		if(read_configuration(get_alphanumeric_string_by_user())){
			std::cout<<"reflector -> interaction() ->";
			std::cout<<" loading reflector successful."<<std::endl;
		}else{
			std::cout<<"reflector -> interaction() ->";
			std::cout<<" loading reflector failed."<<std::endl;		
		}
	}
	//==================================================================
	if(valid_user_input.compare(key_show_calibration_image)==0){	
		reflector_calibration_image.disp_image();
	}
	//==================================================================
	if(valid_user_input.compare(
	key_show_calibration_image_with_polygons)==0){
		update_calibration_image_with_polygons(false);
		reflector_calibration_image_with_polygons.disp_image();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_tripods_in_images)==0){
		show_tripods = !show_tripods; 
	}	
	//==================================================================
	if(valid_user_input.compare(
	key_check_polygon_position_in_live_reflector_display)==0){
		
		double exposure_time_for_reflector_in_ms = 
		ask_user_for_non_zero_float(
		"exposure time on reflector",0.1,180e3);
		
		check_mirror_polygon_position_in_live_reflector_display(
		exposure_time_for_reflector_in_ms);
	}
	//==================================================================
	if(valid_user_input.compare(key_calibration_image)==0){	
		set_reflector_calibration_image
		(get_alphanumeric_string_by_user());
	}
	//==================================================================
	if(valid_user_input.compare(key_change_name)==0){	
		set_reflector_name(get_alphanumeric_string_by_user());
	}
	//==================================================================
	if(valid_user_input.compare(key_save_reflector)==0){
		write_configuration_to_disk();
		save_calibration_image_with_mirror_polygons();
	}
	//==================================================================
	if(valid_user_input.compare(key_manipulate_mirror)==0){
		if(pointer_to_current_mirror_to_work_with==NULL){
			std::cout<<"reflector -> interaction() -> [";
			std::cout<<key_manipulate_mirror<<"] manipulate_mirror ";
			std::cout<<"but mirror ID is not defined yet!"<<std::endl;
		}else{
			if(reflector_calibration_image.get_init_status()){
				manipulate_mirror_polygon();
			}else{
				std::cout<<"reflector -> interaction() -> [";
				std::cout<<key_manipulate_mirror<<"] manipulate_mirror but ";
				std::cout<<"reflector calibration image is not defined yet!";
				std::cout<<std::endl;
			}
		}
	}
	//==================================================================
	if(valid_user_input.compare(key_delete_current_mirror)==0){
		// delete current mirror from list
		if(pointer_to_current_mirror_to_work_with == NULL){
			std::cout << "reflector -> interaction -> delete mirror ->";
			std::cout << " no current mirror ID to delete!\n";
		}else{		
			pop_mirror
			(pointer_to_current_mirror_to_work_with->get_mirror_ID());
		}

	}
	//==================================================================
	if(valid_user_input.compare(key_choose_mirror_ID)==0){	
		uint user_chosen_mirror_ID;
		user_chosen_mirror_ID = 
		(uint)ask_user_for_integer("Enter mirror ID",0,1E6);
		
		if(choose_mirror_ID_to_work_with(user_chosen_mirror_ID)){
			// the mirror does exist already
		}else{
			// the mirror does not exist yet and is therefore created
			add_mirror(user_chosen_mirror_ID);
			choose_mirror_ID_to_work_with(user_chosen_mirror_ID);
		}
	}
	//==================================================================
	if(valid_user_input.compare(key_release_mirror_ID)==0){	
		pointer_to_current_mirror_to_work_with = NULL;
	}
	//==================================================================	
	if(valid_user_input.compare(key_quit)==0){	
		//==========================================================
		// chose to quit mirror manipulation
		//==========================================================
		user_wants_to_quit_mirror_manipulation = true;
	}
	//==================================================================	
	if(valid_user_input.compare(key_show_mirror_status)==0){	
		//==========================================================
		// show mirror status
		//==========================================================
		show_mirror_status();
	}	
	//==================================================================
	update_status_prompt(get_reflector_prompt());
	}while(user_wants_to_quit_mirror_manipulation == false);	
}
//======================================================================
void reflector::set_reflector_name(std::string new_reflector_name){
	reflector_name = new_reflector_name;
	reflector_configuration_file_name =  new_reflector_name;
	reflector_configuration_file_name += ".cfg";
}	
//======================================================================
bool reflector::set_reflector_calibration_image(std::string image_file_name){
	if(reflector_calibration_image.load_image(image_file_name)){
		if(verbosity){
			std::cout<<"reflector -> ";
			std::cout<<"set_reflector_calibration_image() -> ";
			std::cout<<"successful: ";
			std::cout<<reflector_calibration_image.get_file_name()<<std::endl;
		}
		name_of_calibration_image = image_file_name;
		reflector_calibration_image_with_polygons = 
		reflector_calibration_image;
		return true;
	}else{
		name_of_calibration_image = "failed to load image!";
		
		reflector_calibration_image_with_polygons.image_initialized = 
		reflector_calibration_image.image_initialized;
		
		std::cout<<"reflector -> ";
		std::cout<<"set_reflector_calibration_image() -> ";
		std::cout<<"failed to load: ";
		std::cout<<image_file_name<<std::endl;
		return false;
	}
}
//======================================================================

void reflector::disp_status(){
	std::cout<<get_reflector_prompt();
}
//======================================================================
bool reflector::read_configuration(
std::string file_name_of_reflector_configuration_to_load){
	libconfig::Config reflector_configuration_handle;
	//==================================================================
	// Read the file. If there is an error, report it and exit.
	//==================================================================
	file_name_of_reflector_configuration_to_load +=
	file_name_extension_for_configuration_file;
	std::cout<<"reflector -> read_configuration() -> ";
	std::cout<<file_name_of_reflector_configuration_to_load<<std::endl;
	
	try
	{
		reflector_configuration_handle.
		readFile(file_name_of_reflector_configuration_to_load.c_str());
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
	const libconfig::Setting& root = reflector_configuration_handle.getRoot();
	
	bool reading_is_valid = true;
	
	// Output a list of all books in the inventory.
	try
	{
		const libconfig::Setting &Reflector = root[key_reflector];
		const libconfig::Setting &Mirrors = Reflector[key_list_of_mirrors];
		
		int number_of_mirrors_in_file = Mirrors.getLength();
		if(verbosity){
			std::cout<<"reflector -> read_configuration() -> There are ";
			std::cout<<number_of_mirrors_in_file;
			std::cout<<" mirror(s) to be loaded."<<std::endl;
		}
		
		
		// delete all old reflector information
		initialize_default_reflector();
	//read in new information
	
	//==================================================================
	std::string new_reflector_name;
	if( Reflector.lookupValue(key_reflector_name, new_reflector_name) ){
		set_reflector_name(new_reflector_name);
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_name<<" is set to "<<reflector_name<<std::endl;
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_name<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	if( Reflector.lookupValue
	(key_reflector_calibration_image, name_of_calibration_image) 	){
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_calibration_image<<" is set to ";
		std::cout<<name_of_calibration_image<<std::endl;
		
		if(set_reflector_calibration_image(name_of_calibration_image)){
			std::cout<<"reflector -> read_configuration() -> ";
			std::cout<<key_reflector_calibration_image;
			std::cout<<" loading complete."<<std::endl;
		}else{
			std::cout<<"reflector -> read_configuration() -> ";
			std::cout<<key_reflector_calibration_image;
			std::cout<<" failed to load sccan_image."<<std::endl;
			reading_is_valid = false;
		}
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_calibration_image<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	double new_tripod_radius_in_m;
	if( Reflector.lookupValue
	(key_tripod_radius_in_m, new_tripod_radius_in_m) 	){
		tripod_radius_in_m = new_tripod_radius_in_m;
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_tripod_radius_in_m<<" set to ";
		std::cout<<tripod_radius_in_m<<std::endl;
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_tripod_radius_in_m<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	double new_pitch_of_bolt_in_m_per_revolution;	
	if( Reflector.lookupValue
	(key_pitch_of_bolt_in_m_per_revolution, 
	new_pitch_of_bolt_in_m_per_revolution) ){
		pitch_of_bolt_in_m_per_revolution = 
		new_pitch_of_bolt_in_m_per_revolution;
		
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_pitch_of_bolt_in_m_per_revolution<<" set to ";
		std::cout<<pitch_of_bolt_in_m_per_revolution<<std::endl;
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_pitch_of_bolt_in_m_per_revolution<<" fail."<<std::endl;
		reading_is_valid = false;
	}		
	//==================================================================
	for(
	int mirror_itterator = 0;
	mirror_itterator<number_of_mirrors_in_file;
	mirror_itterator++){
		libconfig::Setting &Mirror_file_handle = Mirrors[mirror_itterator];
		
		mirror *pointer_to_a_new_mirror_read_form_file;
		pointer_to_a_new_mirror_read_form_file = 
		new mirror(0,verbosity);
		
		if(pointer_to_a_new_mirror_read_form_file->
		import_configuration(&Mirror_file_handle))
		{
			if(verbosity){
				std::cout<<"reflector -> read_configuration() -> ";
				std::cout<<"mirror "<<(mirror_itterator+1)<<" of ";
				std::cout<<number_of_mirrors_in_file;
				std::cout<<" has been loaded successful."<<std::endl;
			}	
			//==========================================================
			// init mirror with reflector data
			//==========================================================
			pointer_to_a_new_mirror_read_form_file -> set_mirror_tripod(
			tripod_radius_in_m,pitch_of_bolt_in_m_per_revolution);
			
			
			
			//==========================================================
			// update mirror masks
			//==========================================================
			pointer_to_a_new_mirror_read_form_file->
			initialize_list_of_points_inside_mirror_polygon(
			reflector_calibration_image_with_polygons.image_matrix.
			size());
			
			list_of_pointers_to_mirrors.
			push_back(pointer_to_a_new_mirror_read_form_file);
				
		}else{
			reading_is_valid  = false;
			std::cout<<"reflector -> read_configuration() -> ";
			std::cout<<"mirror "<<(mirror_itterator+1)<<" of ";
			std::cout<<number_of_mirrors_in_file;
			std::cout<<" failed to load from file!"<<std::endl;
		}
	}
	
	}
	catch(const libconfig::SettingNotFoundException &nfex)
	{
		std::cout<<"reflector -> read_configuration() ->";
		std::cout<<"loading failed! Settings not found!"<<std::endl;
		reading_is_valid  = false;
	}
	

	
	return reading_is_valid;
}
//======================================================================
bool reflector::write_configuration_to_disk(){
	
	libconfig::Config 	reflector_configuration_handle;
	libconfig::Setting &root = reflector_configuration_handle.getRoot();

	// add reflector
	if(! root.exists(key_reflector))
    root.add(key_reflector, libconfig::Setting::TypeGroup);

	libconfig::Setting &Reflector = root[key_reflector];
	
	Reflector.add(key_reflector_name,libconfig::Setting::TypeString)=
	reflector_name;
	
	Reflector.add
	(key_reflector_calibration_image, libconfig::Setting::TypeString) = 
	name_of_calibration_image;
	
	Reflector.add(key_tripod_radius_in_m, libconfig::Setting::TypeFloat) = 
	tripod_radius_in_m;

	Reflector.add
	(key_pitch_of_bolt_in_m_per_revolution, libconfig::Setting::TypeFloat) = 
	pitch_of_bolt_in_m_per_revolution;
	
	// add mirrors

	if(! Reflector.exists(key_list_of_mirrors))
    Reflector.add(key_list_of_mirrors, libconfig::Setting::TypeList);

	libconfig::Setting &Mirrors = Reflector[key_list_of_mirrors];

	for(int mirror_itterator = 0;
	mirror_itterator<list_of_pointers_to_mirrors.size();
	mirror_itterator++)
	{
		// Create the new mirror entry.
		libconfig::Setting &Mirror = Mirrors.add(libconfig::Setting::TypeGroup);

		list_of_pointers_to_mirrors.
		at(mirror_itterator)->export_configuration_setting(&Mirror);
	}
	// Write out the new configuration.
	try
	{
		reflector_configuration_handle.
		writeFile(reflector_configuration_file_name.c_str());
		std::cout << "reflector -> ";
		std::cout << "write_configuration_to_disk() -> ";
		std::cout << "successfull: ";
		std::cout << reflector_configuration_file_name << std::endl;
	}
	catch(const libconfig::FileIOException &fioex)
	{
		std::cout << "reflector -> ";
		std::cout << "write_configuration_to_disk() -> ";
		std::cerr << "I/O error while writing file: " ;
		std::cerr << reflector_configuration_file_name << std::endl;
		return(false);
	}
	return(true);	
}
//======================================================================
bool reflector::save_calibration_image_with_mirror_polygons(){
	
	std::string calibration_image_with_polygons_filename = 
	reflector_name+"_mirror_polygons.png";	
	
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"save_calibration_image_with_mirror_polygons() -> ";
		std::cout<<calibration_image_with_polygons_filename;
	}

	update_calibration_image_with_polygons(false);
	reflector_calibration_image_with_polygons.save_simple_image(
	calibration_image_with_polygons_filename);
	
	if(verbosity){
		std::cout<<" has been saved"<<std::endl;
	}
	
}
//======================================================================
std::string reflector::get_reflector_prompt(){
	std::stringstream out, info;
	out<<"|"<<std::endl;
	
	// reflector name
	info.str("");
	info<<reflector_name;
	out<<make_nice_line_with_dots("| reflector name",info.str());
	
	// number of mirrors
	info.str("");
	info<<list_of_pointers_to_mirrors.size();
	out<<make_nice_line_with_dots("| number of mirrors",info.str());
	
	// list of mirrors
	info.str("");
	if(list_of_pointers_to_mirrors.size()>0){
		for(
		int mirror_itterator=0;
		mirror_itterator<list_of_pointers_to_mirrors.size();
		mirror_itterator++)
		{
			info<<list_of_pointers_to_mirrors.at(mirror_itterator)->
			get_mirror_ID()<<", ";
		}
	}else{
		info<<"no mirrors on reflector";
	}
	out<<make_nice_line_with_dots("| mirror IDs:",info.str());
	
	// current mirror ID to work with
	info.str("");
	if(pointer_to_current_mirror_to_work_with==NULL){
		info<<"has not been set yet";	
	}else{
		info<<pointer_to_current_mirror_to_work_with->
		get_mirror_ID();
	}
	out<<make_nice_line_with_dots
	("| current mirror ID to work with",info.str());
	
	out<<"|"<<std::endl;
	
	// reflector calibration sccan_image
	info.str("");
	if(reflector_calibration_image.get_init_status()){
		info<<name_of_calibration_image;
	}else{
		info<<"has not been set yet";
	}
	out<<make_nice_line_with_dots
	("| reflector calibration sccan_image",info.str());
	
	out<<"|"<<std::endl;
	
	// mirror tripod radius
	info.str("");
	info<<tripod_radius_in_m<<"m";
	out<<make_nice_line_with_dots("| mirror tripod radius",info.str());
		
	// pitch of mirror bolts
	info.str("");
	info<<pitch_of_bolt_in_m_per_revolution<<"m/revolution";
	out<<make_nice_line_with_dots("| pitch of mirror bolts",info.str());
	
	// toggle tripod displaying
	info.str("");
	if(show_tripods){info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| show tripods",info.str());
	
	out<<"|";
	
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
std::string reflector::get_reflector_name(){
	return reflector_name;
}
//======================================================================
// mirror polygon manipulation 
//======================================================================
void reflector::update_calibration_image_with_polygons(bool highlight){
	
	reflector_calibration_image_with_polygons=
	reflector_calibration_image;		
	
	update_image_with_polygons(
	&reflector_calibration_image_with_polygons,
	highlight);
}
//======================================================================
void reflector::update_image_with_polygons(
sccan_image *image_to_draw_poygons_in,bool highlight){	

	if(verbosity){
		std::stringstream out;
		out<<"reflector -> ";
		out<<"update_image_with_polygons -> ";
		out<<"highlighting ";	
		if(highlight){out<<"TRUE";}else{out<<"FALSE";}
	}
	
	uint mirror_ID_of_mirror_to_highlight;
	if(pointer_to_current_mirror_to_work_with == 0){
		mirror_ID_of_mirror_to_highlight = 0;
	}else{
		mirror_ID_of_mirror_to_highlight = 
		pointer_to_current_mirror_to_work_with->get_mirror_ID();
	}
	
	if(!highlight)
	mirror_ID_of_mirror_to_highlight = 0;
	
	//==================================================================
	// for each mirror
	//==================================================================
	for(	int mirror_itterator = 0;
			mirror_itterator<list_of_pointers_to_mirrors.size();
			mirror_itterator++)
	{
		bool highlight_specific_mirror = false;
		
		if(highlight && 
		pointer_to_current_mirror_to_work_with->
		get_mirror_ID() 
		== 
		list_of_pointers_to_mirrors.
		at(mirror_itterator)->
		get_mirror_ID())
		highlight_specific_mirror = true;
		
		if(show_tripods){
			list_of_pointers_to_mirrors.
			at(mirror_itterator)->
			draw_mirror_tripod(
			image_to_draw_poygons_in,
			highlight_specific_mirror);		
		}
		
		list_of_pointers_to_mirrors.
		at(mirror_itterator)->
		draw_mirror_polygon(
		image_to_draw_poygons_in,
		highlight_specific_mirror);
				
	}
	flag_calibration_image_with_polygons_created = true;
	
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"update_calibration_image_with_polygons -> ";
		std::cout<<"finished"<<std::endl;	
	}
}
//======================================================================
/*
void reflector::update_image_with_polygons(
sccan_image *image_to_draw_poygons_in,bool highlight){	
	
	uint mirror_ID_of_mirror_to_highlight;
	if(pointer_to_current_mirror_to_work_with == 0){
		mirror_ID_of_mirror_to_highlight = 0;
	}else{
		mirror_ID_of_mirror_to_highlight = 
		pointer_to_current_mirror_to_work_with->get_mirror_ID();
	}
	
	if(!highlight)
	mirror_ID_of_mirror_to_highlight = 0;
	
	// to normalize zhe lines and text fonts to be drawed the image size
	// is taken into account
	int number_of_pixels_of_image = 
	image_to_draw_poygons_in->get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);
	
	double line_thickness_per_image_line = 2.0/1e3;
	double circle_radius_in_pixel_per_image_line = 5.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 2.0/1e3;
	double text_thickness_per_image_line  = 2/1e3;	
	
	// line specific information
	int line_thickness = (int)
	ceil(line_thickness_per_image_line*mean_number_of_image_lines);
	int line_Type = 8;		
	 
	// mirror ID spcific information
	cv::Point location_of_mirror_ID;
	int 	text_thickness = (int)
	ceil(text_thickness_per_image_line*mean_number_of_image_lines);
	double 	text_fontScale = 0.75;
	int 	text_fontFace = CV_FONT_HERSHEY_SIMPLEX;
	
	// circle spcific information
	double circle_radius_in_pixel = 
	circle_radius_in_pixel_per_image_line*mean_number_of_image_lines;
	int circle_thickness_in_pixel = 
	(int)ceil(
	circle_thickness_in_pixel_per_image_line*mean_number_of_image_lines
	);
	int circle_lineType	=8;	
	//==================================================================
	// for each mirror polygon group
	//==================================================================
	for(	int mirror_itterator = 0;
			mirror_itterator<list_of_pointers_to_mirrors.size();
			mirror_itterator++)
	{
		int number_of_points = 
		list_of_pointers_to_mirrors.at(mirror_itterator)->
		list_of_points_defining_mirror_polygon.size();
		
		if(number_of_points==0){
			if(verbosity){
				std::cout<<"reflector -> ";
				std::cout<<"update_calibration_image_with_polygons -> ";
				std::cout<<"No polygon of mirror ID:";
				std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()<<std::endl;
			}
		}else{
			if(verbosity){
				std::cout<<"reflector -> ";
				std::cout<<"update_calibration_image_with_polygons -> ";
				std::cout<<"Draw polygon of mirror ID:";
				std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()<<std::endl;
			}
			
			// reset mirror ID location
			location_of_mirror_ID.x=0;
			location_of_mirror_ID.y=0;
			//==========================================================
			// for each line to draw
			//==========================================================
			for(int point_itterator=0; 
				point_itterator<number_of_points;
				point_itterator++)
			{
					// init start point of line
					cv::Point start_point = 
					list_of_pointers_to_mirrors.at(mirror_itterator)->
					list_of_points_defining_mirror_polygon.
					at(point_itterator);
					
					// init end point of line
					cv::Point end_point;
					if((point_itterator+1) == 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.size())
					{
						end_point = 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.at(0);
					}else{
						end_point = 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.
						at(point_itterator+1);
					}
					// draw line
					if(verbosity){
						std::cout<<"reflector -> ";
						std::cout<<"update_calibration_image_with_polygons";
						std::cout<<" -> ";
						std::cout<<"draw line "<<point_itterator+1;
						std::cout<<": ("<<start_point.x<<"|"<<start_point.y;
						std::cout<<")->("<<end_point.x;
						std::cout<<"|"<<end_point.y<<")"<<std::endl;
					}
					if(	list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						get_mirror_ID()
						==
						mirror_ID_of_mirror_to_highlight
					)
					{
						//colour highlighted
						//std::cout<<"highlighted !!!"<<std::endl;
						cv::line(
						image_to_draw_poygons_in->
						image_matrix,
						start_point,
						end_point,
						cv::Scalar( 0, 255, 0),//Blue,Green,Red
						line_thickness,
						line_Type 
						);
					}else{
						//colour default
						cv::line(
						image_to_draw_poygons_in->
						image_matrix,
						start_point,
						end_point,
						cv::Scalar( 0, 0, 255),//Blue,Green,Red
						line_thickness,
						line_Type 
						);
					}
					
					// calculate mirror ID location
					location_of_mirror_ID = location_of_mirror_ID +
					start_point;
					
					// draw a filled circle
					if(	list_of_pointers_to_mirrors.
						at(mirror_itterator)->get_mirror_ID()
						==
						mirror_ID_of_mirror_to_highlight
					)
					{
						//colour highlighted
						cv::circle( 
						image_to_draw_poygons_in->
						image_matrix,
						start_point,
						circle_radius_in_pixel,
						cv::Scalar( 128, 255, 0 ),
						circle_thickness_in_pixel,
						circle_lineType );
					}else{
						//colour default
						cv::circle( 
						image_to_draw_poygons_in->
						image_matrix,
						start_point,
						circle_radius_in_pixel,
						cv::Scalar( 128, 0, 255 ),
						circle_thickness_in_pixel,
						circle_lineType );
					}
				}
		
			// draw mirror ID
			// calculate mirror ID location
			location_of_mirror_ID.x = 
			(location_of_mirror_ID.x)/number_of_points;
			
			location_of_mirror_ID.y = 
			(location_of_mirror_ID.y)/number_of_points;
			
			//create text ID to display
			std::stringstream text_to_put_in_image;
			text_to_put_in_image<<"ID";
			text_to_put_in_image<<list_of_pointers_to_mirrors.
			at(mirror_itterator)->get_mirror_ID();
			
			// put text to sccan_image
			if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()
				==
				mirror_ID_of_mirror_to_highlight)
			{
				//colour highlighted
				cv::putText(
				image_to_draw_poygons_in->
				image_matrix,
				text_to_put_in_image.str(),
				location_of_mirror_ID,
				text_fontFace,// fontFace
				text_fontScale,// fontScale
				cv::Scalar( 64, 255, 64 ),//Blue,Green,Red
				text_thickness,
				8
				);
			}else{
				//colour default
				//colour highlighted
				cv::putText(
				image_to_draw_poygons_in->
				image_matrix,
				text_to_put_in_image.str(),
				location_of_mirror_ID,
				text_fontFace,// fontFace
				text_fontScale,// fontScale
				cv::Scalar( 64, 64, 255 ),//Blue,Green,Red
				text_thickness,
				8
				);
			}
		}
	}
	flag_calibration_image_with_polygons_created = true;
	
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"update_calibration_image_with_polygons -> ";
		std::cout<<"end"<<std::endl;	
	}
}*/
//======================================================================
/*
void reflector::update_calibration_image_with_polygons(bool highlight){
	
	reflector_calibration_image_with_polygons=
	reflector_calibration_image;	
	
	uint mirror_ID_of_mirror_to_highlight;
	if(pointer_to_current_mirror_to_work_with == 0){
		mirror_ID_of_mirror_to_highlight = 0;
	}else{
		mirror_ID_of_mirror_to_highlight = 
		pointer_to_current_mirror_to_work_with->get_mirror_ID();
	}
	
	if(!highlight)
	mirror_ID_of_mirror_to_highlight = 0;
	
	// to normalize zhe lines and text fonts to be drawed the image size
	// is taken into account
	int number_of_pixels_of_image = 
	reflector_calibration_image_with_polygons.get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);
	
	double line_thickness_per_image_line = 2.0/1e3;
	double circle_radius_in_pixel_per_image_line = 5.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 2.0/1e3;
	double text_thickness_per_image_line  = 2/1e3;	
	
	// line specific information
	int line_thickness = (int)
	ceil(line_thickness_per_image_line*mean_number_of_image_lines);
	int line_Type = 8;		
	 
	// mirror ID spcific information
	cv::Point location_of_mirror_ID;
	int 	text_thickness = (int)
	ceil(text_thickness_per_image_line*mean_number_of_image_lines);
	double 	text_fontScale = 0.75;
	int 	text_fontFace = CV_FONT_HERSHEY_SIMPLEX;
	
	// circle spcific information
	double circle_radius_in_pixel = 
	circle_radius_in_pixel_per_image_line*mean_number_of_image_lines;
	int circle_thickness_in_pixel = 
	(int)ceil(
	circle_thickness_in_pixel_per_image_line*mean_number_of_image_lines
	);
	int circle_lineType	=8;	
	//==================================================================
	// for each mirror polygon group
	//==================================================================
	for(	int mirror_itterator = 0;
			mirror_itterator<list_of_pointers_to_mirrors.size();
			mirror_itterator++)
	{
		int number_of_points = 
		list_of_pointers_to_mirrors.at(mirror_itterator)->
		list_of_points_defining_mirror_polygon.size();
		
		if(number_of_points==0){
			if(verbosity){
				std::cout<<"reflector -> ";
				std::cout<<"update_calibration_image_with_polygons -> ";
				std::cout<<"No polygon of mirror ID:";
				std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()<<std::endl;
			}
		}else{
			if(verbosity){
				std::cout<<"reflector -> ";
				std::cout<<"update_calibration_image_with_polygons -> ";
				std::cout<<"Draw polygon of mirror ID:";
				std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()<<std::endl;
			}
			
			// reset mirror ID location
			location_of_mirror_ID.x=0;
			location_of_mirror_ID.y=0;
			//==========================================================
			// for each line to draw
			//==========================================================
			for(int point_itterator=0; 
				point_itterator<number_of_points;
				point_itterator++)
			{
					// init start point of line
					cv::Point start_point = 
					list_of_pointers_to_mirrors.at(mirror_itterator)->
					list_of_points_defining_mirror_polygon.
					at(point_itterator);
					
					// init end point of line
					cv::Point end_point;
					if((point_itterator+1) == 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.size())
					{
						end_point = 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.at(0);
					}else{
						end_point = 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.
						at(point_itterator+1);
					}
					// draw line
					if(verbosity){
						std::cout<<"reflector -> ";
						std::cout<<"update_calibration_image_with_polygons";
						std::cout<<" -> ";
						std::cout<<"draw line "<<point_itterator+1;
						std::cout<<": ("<<start_point.x<<"|"<<start_point.y;
						std::cout<<")->("<<end_point.x;
						std::cout<<"|"<<end_point.y<<")"<<std::endl;
					}
					if(	list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						get_mirror_ID()
						==
						mirror_ID_of_mirror_to_highlight
					)
					{
						//colour highlighted
						//std::cout<<"highlighted !!!"<<std::endl;
						cv::line(
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						end_point,
						cv::Scalar( 0, 255, 0),//Blue,Green,Red
						line_thickness,
						line_Type 
						);
					}else{
						//colour default
						cv::line(
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						end_point,
						cv::Scalar( 0, 0, 255),//Blue,Green,Red
						line_thickness,
						line_Type 
						);
					}
					
					// calculate mirror ID location
					location_of_mirror_ID = location_of_mirror_ID +
					start_point;
					
					// draw a filled circle
					if(	list_of_pointers_to_mirrors.
						at(mirror_itterator)->get_mirror_ID()
						==
						mirror_ID_of_mirror_to_highlight
					)
					{
						//colour highlighted
						cv::circle( 
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						circle_radius_in_pixel,
						cv::Scalar( 128, 255, 0 ),
						circle_thickness_in_pixel,
						circle_lineType );
					}else{
						//colour default
						cv::circle( 
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						circle_radius_in_pixel,
						cv::Scalar( 128, 0, 255 ),
						circle_thickness_in_pixel,
						circle_lineType );
					}
				}
		
			// draw mirror ID
			// calculate mirror ID location
			location_of_mirror_ID.x = 
			(location_of_mirror_ID.x)/number_of_points;
			
			location_of_mirror_ID.y = 
			(location_of_mirror_ID.y)/number_of_points;
			
			//create text ID to display
			std::stringstream text_to_put_in_image;
			text_to_put_in_image<<"ID";
			text_to_put_in_image<<list_of_pointers_to_mirrors.
			at(mirror_itterator)->get_mirror_ID();
			
			// put text to sccan_image
			if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()
				==
				mirror_ID_of_mirror_to_highlight)
			{
				//colour highlighted
				cv::putText(
				reflector_calibration_image_with_polygons.
				image_matrix,
				text_to_put_in_image.str(),
				location_of_mirror_ID,
				text_fontFace,// fontFace
				text_fontScale,// fontScale
				cv::Scalar( 64, 255, 64 ),//Blue,Green,Red
				text_thickness,
				8
				);
			}else{
				//colour default
				//colour highlighted
				cv::putText(
				reflector_calibration_image_with_polygons.
				image_matrix,
				text_to_put_in_image.str(),
				location_of_mirror_ID,
				text_fontFace,// fontFace
				text_fontScale,// fontScale
				cv::Scalar( 64, 64, 255 ),//Blue,Green,Red
				text_thickness,
				8
				);
			}
		}
	}
	flag_calibration_image_with_polygons_created = true;
	
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"update_calibration_image_with_polygons -> ";
		std::cout<<"end"<<std::endl;	
	}
}
*/
//======================================================================
void reflector::manipulate_mirror_polygon(){
	
	// draw all existing polygons
	update_calibration_image_with_polygons(true);
	
	// display usage
	disp_status();
	std::cout<<"Press [ESC] to end Mirror manipulation"<<std::endl;
	std::cout<<"Press [ d ] to erase the last point"<<std::endl;
	std::cout<<"Press [ left mouse button ] to create new point"<<std::endl;
	
	if(	flag_calibration_image_with_polygons_created ){
		//create window for mirror polygon manipulation
		cv::namedWindow(
			name_of_calibration_window_with_highlighted_poygons.c_str(),
			CV_WINDOW_NORMAL 
		);
		
		// show sccan_image for mirror polygon manipulation
		cv::imshow( 
			name_of_calibration_window_with_highlighted_poygons.c_str(),
			reflector_calibration_image_with_polygons.image_matrix
		);  
		
		// mouse callback
		// on left mose click a new polygon point is added to the 
		// list
		reflector* p = this;
		cv::setMouseCallback(
		name_of_calibration_window_with_highlighted_poygons.c_str(),
		onMouse, (void *)p);
		
		//wait for command lin input
		int user_input_key;
		do{
			user_input_key = cv::waitKey(0); 
			
			switch( (char)user_input_key ){
				case 'd':
					pop_point_of_polygon_point_list_of_mirror();
					//std::cout<<"destroy last point"<<std::endl;
					// destroy the last polygon element
				break;
	//~ default:
	//~ std::cout<<"Press [ESC] to end Mirror manipulation"<<std::endl;
	//~ std::cout<<"Press [ d ] to erase the last point"<<std::endl;
	//~ std::cout<<"Press [ left mouse button ] to create new point"<<std::endl;
			}
		}while( (user_input_key & 255) != 27 ); // ESC key to exit
		
		//destroy window for sccan_image manipulation
		cv::destroyWindow(
			name_of_calibration_window_with_highlighted_poygons.c_str()
		);  
	}else{
		std::stringstream out;
		out<<"reflector -> manipulate_list_points_of_mirror() ->";
		out<<" the calibration sccan_image with polygons was not created!"<<std::endl;
		std::cout<<out.str();
	}
	//==================================================================
	// update mirror mask
	//==================================================================
	pointer_to_current_mirror_to_work_with->
	initialize_list_of_points_inside_mirror_polygon(
	reflector_calibration_image_with_polygons.image_matrix.size()
	);
}	
//======================================================================
void reflector::onMouse(int event, int x, int y, int flags, void *param){
		
		reflector* p = (reflector*)param;
		
		if( event != cv::EVENT_LBUTTONDOWN )
		return;
		p->add_point_to_polygon_point_list_of_mirror(x,y);
}
//======================================================================
void reflector::add_point_to_polygon_point_list_of_mirror(int pixel_x, int pixel_y){

	cv::Point point_to_add;
	point_to_add.x = pixel_x;
	point_to_add.y = pixel_y;
	
	pointer_to_current_mirror_to_work_with->
	list_of_points_defining_mirror_polygon.push_back(point_to_add);
	
	// update sccan_image
	update_calibration_image_with_polygons(true);
	
	// update display
	cv::imshow( 
		name_of_calibration_window_with_highlighted_poygons,
		reflector_calibration_image_with_polygons.image_matrix
	);
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"add_point_to_polygon_point_list_of_mirror -> ";
		std::cout<<"add point ("<<pixel_x<<"|"<<pixel_y<<") ";
		std::cout<<"to mirror polygon"<<std::endl;
	}
}
//======================================================================
void reflector::pop_point_of_polygon_point_list_of_mirror(){
		
	if(	pointer_to_current_mirror_to_work_with->
		list_of_points_defining_mirror_polygon.size() > 0)
	{
		// delete last point
		pointer_to_current_mirror_to_work_with->
		list_of_points_defining_mirror_polygon.pop_back();
		
		// update sccan_image
		update_calibration_image_with_polygons(true);
		
		// update display
		cv::imshow(
			name_of_calibration_window_with_highlighted_poygons,
			reflector_calibration_image_with_polygons.image_matrix
		);
			
		if(verbosity){
			std::cout<<"reflector -> ";
			std::cout<<"pop_point_of_polygon_point_list_of_mirror -> ";
			std::cout<<"pop last point"<<std::endl;
		}
	}else{
		if(verbosity)
		{
		std::cout<<"reflector -> ";
		std::cout<<"pop_point_of_polygon_point_list_of_mirror -> ";
		std::cout<<"no point to pop"<<std::endl;
		}
	} 
}
//======================================================================
void reflector::toggle_verbosity(){
	verbosity = !verbosity;
	
	reflector_calibration_image.
	toggle_verbosity(verbosity);
	
	reflector_calibration_image_with_polygons.
	toggle_verbosity(verbosity);
	
}
//======================================================================
void reflector::check_mirror_polygon_position_in_live_reflector_display(
double desired_exposure_time_for_reflector_camera_in_ms){
	
	sccan_image live_reflector_image;
	
	
	if(verbosity){
		std::cout << "reflector -> ";
		std::cout << "check_mirror_polygon_position_in_live_reflector_display() -> ";
		std::cout << "image name -> live reflector mirror polygon check";
		std::cout << "\n";
	}	
	
	//double 
	//iteration_start_exposure_time_for_reflector_image_in_ms = 10.0;
	
	double desired_relative_maximal_camera_response = 0.9;

	if(verbosity){
		std::cout << "reflector -> ";
		std::cout << "check_mirror_polygon_position_in_live_reflector_display() -> ";
		std::cout << "desired camera response: " << desired_relative_maximal_camera_response;			
		std::cout << "\n";
	}	

	//~ pointer_to_reflector_camera->acquire_image(
	//~ &iteration_start_exposure_time_for_reflector_image_in_ms,
	//~ desired_relative_maximal_camera_response);

	pointer_to_reflector_camera->acquire_image(
	&desired_exposure_time_for_reflector_camera_in_ms);
	
	live_reflector_image = 
	pointer_to_reflector_camera->get_latest_image();
	
	live_reflector_image.
	set_name("live reflector mirror polygon check");
	
	update_image_with_polygons(&live_reflector_image,false);

	if(verbosity){
		std::cout << "reflector -> ";
		std::cout << "check_mirror_polygon_position_in_live_reflector_display() -> ";
		std::cout << "image aquired, mirror polygons added, ready to display image";			
		std::cout << "\n";
	}	
	
	live_reflector_image.disp_image();
}
//======================================================================
// list of mirrors
//======================================================================
std::vector<mirror*> list_of_pointers_to_mirrors;
mirror 			*pointer_to_current_mirror_to_work_with;
//======================================================================
void reflector::initialize_mirror_list(){
	pointer_to_current_mirror_to_work_with = NULL;
	list_of_pointers_to_mirrors.clear();
}
//======================================================================
bool reflector::choose_mirror_ID_to_work_with(uint mirror_ID_to_work_with){
	bool mirror_found = false;
	
	for(int mirror_itterator=0;
	mirror_itterator<list_of_pointers_to_mirrors.size();
	mirror_itterator++)
	{
		if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
		get_mirror_ID() == mirror_ID_to_work_with)
		{
			mirror_found = true;
			pointer_to_current_mirror_to_work_with = 
			list_of_pointers_to_mirrors.at(mirror_itterator);
		}
	}
	if(verbosity){
		std::cout<<"mirror_list -> choose_mirror_ID_to_work_with() -> ";
		std::cout<<"ID "<<mirror_ID_to_work_with<<" ";
		if(mirror_found){std::cout<<"was set to be manipulated.";}else
		{std::cout<<"was not found in mirror list!";}
		std::cout<<std::endl;
	}
	return mirror_found;	
}
//======================================================================
bool reflector::add_mirror(uint new_mirror_ID){
	
	mirror *pointer_to_new_mirror;
	pointer_to_new_mirror = new mirror(0,verbosity);
	
	pointer_to_new_mirror->set_mirror_ID(new_mirror_ID);
	
	pointer_to_new_mirror->set_mirror_tripod(
	tripod_radius_in_m,
	pitch_of_bolt_in_m_per_revolution);
	
	if(choose_mirror_ID_to_work_with
	(pointer_to_new_mirror->get_mirror_ID())){
		// mirror already in list
		std::cout<<"mirror_list -> add_mirror() -> mirror ID ";
		std::cout<<pointer_to_new_mirror->get_mirror_ID();
		std::cout<<" already in list!"<<std::endl;
		return false;
	}else{
		// a new mirror
		list_of_pointers_to_mirrors.push_back(pointer_to_new_mirror);
		if(list_of_pointers_to_mirrors.size()==1)
		{
			pointer_to_current_mirror_to_work_with = 
			list_of_pointers_to_mirrors.at(0);
		}
		
		if(verbosity){
			std::cout<<"mirror_list -> add_mirror() -> mirror ID ";
			std::cout<<new_mirror_ID<<" was add to mirror list."<<std::endl;
		}
		return true;
	}
//======================================================================
}
//======================================================================
bool reflector::pop_mirror(uint old_mirror_ID){
	bool mirror_popped = false;
	// loop and destroy...
	for(int mirror_itterator=0;
	mirror_itterator<list_of_pointers_to_mirrors.size();
	mirror_itterator++)
	{
		if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
		get_mirror_ID() == old_mirror_ID)
		{
			list_of_pointers_to_mirrors.
			erase(list_of_pointers_to_mirrors.begin()+mirror_itterator);
			mirror_popped = true;
		}
	}
	if(verbosity){
		std::cout<<"mirror_list -> pop_mirror() -> ";
		std::cout<<"ID "<<old_mirror_ID<<" ";
		if(mirror_popped){std::cout<<"was popped.";}else
		{std::cout<<"was not found in mirror list!";}
		std::cout<<std::endl;
	}
	
	if(mirror_popped)
	pointer_to_current_mirror_to_work_with = NULL;
	
	return mirror_popped;		
}
//======================================================================
bool reflector::is_there_a_mirror_to_work_with(){
	if(pointer_to_current_mirror_to_work_with==NULL){
		if(verbosity){
			std::cout<<"mirror_list -> ";
			std::cout<<"is_there_a_mirror_to_work_with() -> ";
			std::cout<<"No its not."<<std::endl;
		}
		return false;
	}else{
		if(verbosity){
			std::cout<<"mirror_list -> ";
			std::cout<<"is_there_a_mirror_to_work_with() -> ";
			std::cout<<"Yes it is."<<std::endl;
		}
		return true;
	}
}
//======================================================================
int reflector::get_number_of_mirrors_on_reflector(){
	return list_of_pointers_to_mirrors.size();
}
//======================================================================
list_of_pixel_positions* reflector::get_pointer_to_list_of_Points_inside_mirror_polygon
(int mirror_itterator){
	
	if(mirror_itterator <list_of_pointers_to_mirrors.size()){
		
		if(verbosity){
			std::cout<<"mirror_list -> ";
			std::cout<<"get_pointer_to_list_of_Points_inside_mirror_polygon";
			std::cout<<" -> ";
			std::cout<<"return list of pixels of mirror ID";
			std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
			get_mirror_ID();
			std::cout<<std::endl;
		}
		
		return list_of_pointers_to_mirrors.at(mirror_itterator)->
		get_pointer_to_list_of_Points_inside_mirror_polygon();
	}else{
		std::cout<<"mirror_list -> ";
		std::cout<<"get_pointer_to_list_of_Points_inside_mirror_polygon";
		std::cout<<" -> ";
		std::cout<<"there is no mirror "<<mirror_itterator<<".";
		std::cout<<" There are/is only "<<list_of_pointers_to_mirrors.size();
		std::cout<<" mirror(s)!";
		std::cout<<std::endl;
		return NULL;
	}
}
//======================================================================
void reflector::show_mirror_status(){

	if(verbosity){
		std::cout<<"reflector -> show_mirror_status() of all ";
		std::cout<<list_of_pointers_to_mirrors.size();
		std::cout<<" mirrors"<<std::endl;
	}
	
	std::stringstream out;
	
	for(uint MIit=0;MIit<list_of_pointers_to_mirrors.size();MIit++){
		
		if(verbosity){
			std::cout<<"reflector -> show_mirror_status() -> ";
			std::cout<<"mirror No "<<MIit+1<<", ID ";
			std::cout<<list_of_pointers_to_mirrors.at(MIit)->
			get_mirror_ID()<<std::endl;
		}
		
		out << list_of_pointers_to_mirrors.at(MIit)->get_string();
		
	}
	
	std::cout << out.str();
	export_text("mirror_status.txt",out.str());
}
//======================================================================
