#include "mirror.h"
//======================================================================
mirror::mirror(uint new_mirror_ID,bool new_verbosity){
	initialize(new_mirror_ID,new_verbosity);
}
//======================================================================
void mirror::set_mirror_ID(uint new_mirror_ID){
	mirror_ID = new_mirror_ID;
}
//======================================================================
uint mirror::get_mirror_ID(){
	return mirror_ID;
}
//======================================================================
void mirror::initialize(uint new_mirror_ID,bool new_verbosity){
	verbosity = new_verbosity;
	mirror_ID = new_mirror_ID;
	
	flag_misalignment_angles_have_been_calculated = false;
	flag_manipulation_distances_have_been_calculated = false;
	tripod_orientation_z_in_rad =0.0;
	
	// mask sccan_image
	flag_mask_has_been_initialized = false;
	list_of_points_defining_mirror_polygon.clear();
	list_of_Points_inside_mirror_polygon.clear();
		
	// configuration file
	key_ID = "ID";
	key_tripod_orientation_z_in_deg ="tripod_orientation_z_in_deg";	
	key_list_of_points_defining_mirror_polygon = "polygon";
	key_polygon_point_pixel_coordinate_x = "x";
	key_polygon_point_pixel_coordinate_y = "y";
}
//======================================================================
std::string mirror::get_string(){
	std::stringstream out; out.str("");
	out<<" _________________________________________________________\n";
	out<<"| Mirror ID: "<<mirror_ID<<std::endl;
	out<<"|\n";
	out<<"|  _tripod________________________________________________\n";
	out<<"| | tripod rotation in z: "<<tripod_orientation_z_in_rad;
	out<<"rad / "<<tripod_orientation_z_in_rad*(360/(2.0*M_PI))<<"deg\n";
	out<<"| | tripod radius: "<<tripod_radius_in_m<<"m\n";
	out<<"| | bolt pitch  : "<<pitch_of_bolt_in_m_per_revolution<<"m/revolution"<<"\n";
	out<<"| | initial positions of tripod legs\n";
	out<<"| | 1st leg : "<<position_of_first_tripod_leg.get_string()<<"\n";	
	out<<"| | 2nd leg : "<<position_of_second_tripod_leg.get_string()<<"\n";
	out<<"| | 3rd leg : "<<position_of_third_tripod_leg.get_string()<<"\n";
	out<<"| |_______________________________________________________\n";
	out<<"|\n";
	out<<"|  _mirror_mask_polygon___________________________________\n";
	out<<"| |\n";
	out<<"| | number of points: "<<list_of_points_defining_mirror_polygon.size()<<std::endl;
	out<<"| | polygon list: ";
	if(list_of_points_defining_mirror_polygon.size()==0){
		out<<"empty list";
	}else{
		for(int knot=0;
		knot<list_of_points_defining_mirror_polygon.size();
		knot++)
		{
			out<<" (";
			out<<list_of_points_defining_mirror_polygon.at(knot).x<<"|";
			out<<list_of_points_defining_mirror_polygon.at(knot).y<<")";
		}
		out<<" [pixels]\n";
	}
	if(flag_mask_has_been_initialized)
	{
	out<<"| | number of pixels enclosed by polygon: ";
	out<<list_of_Points_inside_mirror_polygon.size()<<std::endl;
	}else{
	out<<"| | mirror mask has not been initialized yet."<<std::endl;
	}
	out<<"| |_______________________________________________________\n";
	out<<"|\n";
	out<<"|  _misalignment__________________________________________\n";
	if(flag_misalignment_angles_have_been_calculated){
	out<<"| | mirror misalignment: "<<std::endl;
	out<<"| | missalignment angle in x: ";
	out<<MirrorMisalignmentDirection.
	get_x_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| | missalignment angle in y: ";
	out<<MirrorMisalignmentDirection.
	get_y_tilt_prompt_in_deg_min_sec()<<std::endl;
	}else{
	out<<"| | mirror misalignment has not been calculated yet.\n";
	}
	out<<"| |_______________________________________________________\n";
	out<<"|_________________________________________________________\n";
	return out.str();
}	
//======================================================================
void mirror::disp(){std::cout<<get_string();}
//======================================================================
void mirror::export_configuration_setting(
libconfig::Setting *Mirror_file_handle){
	
	//mirror ID
	Mirror_file_handle->add(key_ID, libconfig::Setting::TypeInt) = (int)mirror_ID;
	
	//tripod orientation
	Mirror_file_handle->
	add(key_tripod_orientation_z_in_deg, libconfig::Setting::TypeFloat) = 
	tripod_orientation_z_in_rad*(360.0/(2.0*M_PI));
	
	//list_of_pixes_defining_polygon
	if(
		! Mirror_file_handle->exists
		(key_list_of_points_defining_mirror_polygon)
	){
		Mirror_file_handle->add
		(key_list_of_points_defining_mirror_polygon, libconfig::Setting::TypeList);	
	}
	
	libconfig::Setting &polygon_file_handle = 
	(*Mirror_file_handle)[key_list_of_points_defining_mirror_polygon];
	
	for(
	int polygon_point_itterator = 0;
	polygon_point_itterator<
	list_of_points_defining_mirror_polygon.size();
	polygon_point_itterator++
	){
		libconfig::Setting &polygon_point_handle = polygon_file_handle.add
		(libconfig::Setting::TypeGroup);
		
		polygon_point_handle.add
		(key_polygon_point_pixel_coordinate_x,libconfig::Setting::TypeInt) = 
		list_of_points_defining_mirror_polygon.at
		(polygon_point_itterator).x;
		
		polygon_point_handle.add
		(key_polygon_point_pixel_coordinate_y,libconfig::Setting::TypeInt) = 
		list_of_points_defining_mirror_polygon.at
		(polygon_point_itterator).y;
		
	}
	
	if(verbosity){
		std::cout<<"mirror -> export_configuration_setting() ->";
		std::cout<<" mirror exorted. "<<std::endl;
	}
}
//======================================================================
bool mirror::import_configuration(
libconfig::Setting *Mirror_file_handle){
	
	bool flag_mirror_valid = true;
	
	int new_mirror_ID;
	if(Mirror_file_handle->lookupValue(key_ID, new_mirror_ID)){
		// set all mirror properties to default values
		initialize((uint)new_mirror_ID,verbosity);
		if(verbosity){
			std::cout<<"mirror -> import_configuration() -> ";
			std::cout<<"initialize ID ";
			std::cout<<new_mirror_ID<<std::endl;
		}
	}else{
		flag_mirror_valid = false;
	}
	//==================================================================
	if(Mirror_file_handle->lookupValue(
	key_tripod_orientation_z_in_deg,
	 tripod_orientation_z_in_rad)){
		
		tripod_orientation_z_in_rad = 
		tripod_orientation_z_in_rad*((2.0*M_PI)/360.0);
		
		if(verbosity){
			std::cout<<"mirror -> import_configuration() ->";
			std::cout<<" tripod orientation loaded ";
			std::cout<<tripod_orientation_z_in_rad<<"rad."<<std::endl;
		}	
		
		init_tripod();
		
	}else{
		flag_mirror_valid = false;
	}
	//==================================================================
	const libconfig::Setting &polygon_file_handle = 
	(*Mirror_file_handle)[key_list_of_points_defining_mirror_polygon];
	
	int number_of_polygon_points_in_file = 
	polygon_file_handle.getLength();
	
	if(verbosity){
		std::cout<<"mirror -> import_configuration() -> There are ";
		std::cout<<number_of_polygon_points_in_file;
		std::cout<<" polygon points to be loaded."<<std::endl;
	}	
	
	for(
	int polygon_point_itterator = 0;
	polygon_point_itterator<number_of_polygon_points_in_file;
	polygon_point_itterator++
	){
		libconfig::Setting &polygon_point_handle = 
		polygon_file_handle[polygon_point_itterator];
		
		cv::Point new_Point_from_file;
		
		if(polygon_point_handle.lookupValue(
		key_polygon_point_pixel_coordinate_x,
		new_Point_from_file.x)){
			if(verbosity){
				std::cout<<"mirror -> import_configuration() ->";
				std::cout<<" polygon point "<<(polygon_point_itterator+1);
				std::cout<<" of "<<number_of_polygon_points_in_file;
				std::cout<<" x coordinate "<<new_Point_from_file.x;
				std::cout<<"px was loaded"<<std::endl;
			}
		}else{
			flag_mirror_valid = false;
			std::cout<<"mirror -> import_configuration() ->";
			std::cout<<" polygon point "<<(polygon_point_itterator+1);
			std::cout<<" of "<<number_of_polygon_points_in_file;
			std::cout<<" loading x-coordinate failed!"<<std::endl;
		}
		
		if(polygon_point_handle.lookupValue(
		key_polygon_point_pixel_coordinate_y,
		new_Point_from_file.y)){
			if(verbosity){
				std::cout<<"mirror -> import_configuration() ->";
				std::cout<<" polygon point "<<(polygon_point_itterator+1);
				std::cout<<" of "<<number_of_polygon_points_in_file;
				std::cout<<" y coordinate "<<new_Point_from_file.y;
				std::cout<<"px was loaded"<<std::endl;
			}
		}else{
			flag_mirror_valid = false;
			std::cout<<"mirror -> import_configuration() ->";
			std::cout<<" polygon point "<<(polygon_point_itterator+1);
			std::cout<<" of "<<number_of_polygon_points_in_file;
			std::cout<<" loading y-coordinate failed!"<<std::endl;
		}
		
		list_of_points_defining_mirror_polygon.
		push_back(new_Point_from_file);
	}
	return flag_mirror_valid;
}
//======================================================================
void mirror::initialize_list_of_points_inside_mirror_polygon
(cv::Size image_size_of_reflector_camera){
	
	create_polygon_mask(image_size_of_reflector_camera);
	create_list_of_Points_inside_mirror_polygon();
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"initialize_list_of_points_inside_mirror_polygon()";
		std::cout<<std::endl;
	}
}
//======================================================================
void mirror::create_polygon_mask(
cv::Size image_size_of_reflector_camera){

	// create temporary sccan_image mask
	cv::Mat temporary_mask = 
			cv::Mat::zeros(
			image_size_of_reflector_camera.height,
			image_size_of_reflector_camera.width,
			CV_8UC1);
	
	// create an array of polygon points	
	cv::Point polygon[1][list_of_points_defining_mirror_polygon.size()];
	
	for(int point_itterator = 0;
			point_itterator<
			list_of_points_defining_mirror_polygon.size();
			point_itterator++)
	{
		polygon[0][point_itterator] = 
		list_of_points_defining_mirror_polygon.at(point_itterator);
		if(verbosity){
			std::cout<<"mirror -> create_polygon_mask()->";
			std::cout<<"copy Point ("<<polygon[0][point_itterator].x;
			std::cout<<"|"<<polygon[0][point_itterator].y<<")";
			std::cout<<std::endl;
		}
	}
	
	const cv::Point* ppt[1] = { polygon[0] };
	
	int npt[] = {int(list_of_points_defining_mirror_polygon.size())};
	
	if(verbosity){
		std::cout<<"mirror -> create_polygon_mask() -> ";
		std::cout<<"number of Polygon points "<< npt[0]<<std::endl;
	}
	
	int line_type = 8;
	
	// fill polygon
	cv::fillPoly(
	temporary_mask,
	ppt,
	npt,
	1,
	cv::Scalar(255),
	line_type
	);
	
	// store temporary sccan_image mask
	temporary_mask.copyTo(image_with_mirror_mask.image_matrix);

	//create_list_of_Points_inside_mirror_polygon();
	
	// set flag in mirror class
	flag_mask_has_been_initialized = true;
	
	if(verbosity){
		std::stringstream out;
		out<<"mirror -> create_polygon_mask() -> ";
		std::cout<<"display sccan_image with mask."<<std::endl;
		std::cout<<out.str();
		image_with_mirror_mask.disp_simple_image("polygon mask");
		disp();
	}
}
//======================================================================
void mirror::create_list_of_Points_inside_mirror_polygon(){
	list_of_Points_inside_mirror_polygon = 
	image_with_mirror_mask.create_list_of_Points_of_non_zero_pixels();
}
//======================================================================
list_of_pixel_positions* mirror::
get_pointer_to_list_of_Points_inside_mirror_polygon(){
	return &list_of_Points_inside_mirror_polygon;
}
//======================================================================
sccan_image mirror::highlight_mirror_in_reflector_image
(sccan_image* pointer_to_reflector_image){

	// calculate ROI
	cv::Rect mirror_region_of_interest= boundingRect(
	list_of_points_defining_mirror_polygon);

	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"highlight_mirror_in_reflector_image() -> ";	
		std::cout<<"mirror ID "<<mirror_ID<<" rectangle found at";
		std::cout<<" ("<<mirror_region_of_interest.x<<"|";
		std::cout<<mirror_region_of_interest.y <<"), ";
		std::cout<<"width "<<mirror_region_of_interest.width<<", ";
		std::cout<<"height "<<mirror_region_of_interest.height;
		std::cout<<std::endl;					
	}
		
	sccan_image highlited_image = *pointer_to_reflector_image;
	
	// highlight pixels
	draw_mirror_polygon(&highlited_image,true);
	
	sccan_image highlited_image_roi;
	highlited_image_roi.image_matrix =
	highlited_image.image_matrix(mirror_region_of_interest).clone();
	return highlited_image_roi;
}
//======================================================================
void mirror::
draw_mirror_polygon(
sccan_image *image_to_draw_poygons_in,bool highlight){

	// to normalize the lines and text fonts to be drawed the image size
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

	int number_of_points = 
	list_of_points_defining_mirror_polygon.size();
	
	if(number_of_points==0){
		if(verbosity){
			std::cout<<"mirror -> ";
			std::cout<<"update_calibration_image_with_polygons -> ";
			std::cout<<"No polygon of mirror ID:";
			std::cout<<get_mirror_ID()<<std::endl;
		}
	}else{
		if(verbosity){
			std::cout<<"mirror -> ";
			std::cout<<"update_calibration_image_with_polygons -> ";
			std::cout<<"Draw polygon of mirror ID:";
			std::cout<<get_mirror_ID()<<std::endl;
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
				list_of_points_defining_mirror_polygon.
				at(point_itterator);
				
				// init end point of line
				cv::Point end_point;
				if((point_itterator+1) == 
					list_of_points_defining_mirror_polygon.size())
				{
					end_point = 
					list_of_points_defining_mirror_polygon.at(0);
				}else{
					end_point = 
					list_of_points_defining_mirror_polygon.
					at(point_itterator+1);
				}
				// draw line
				if(verbosity){
					std::cout<<"mirror -> ";
					std::cout<<"update_calibration_image_with_polygons";
					std::cout<<" -> ";
					std::cout<<"draw line "<<point_itterator+1;
					std::cout<<": ("<<start_point.x<<"|"<<start_point.y;
					std::cout<<")->("<<end_point.x;
					std::cout<<"|"<<end_point.y<<")"<<std::endl;
				}
				
				if(highlight){
					
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
				if(highlight){
					
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
		text_to_put_in_image<<get_mirror_ID();
		
		// put text to sccan_image
		if(highlight){
			
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
	
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"update_calibration_image_with_polygons -> ";
		std::cout<<"end"<<std::endl;	
	}

}
//======================================================================
void mirror::
draw_mirror_tripod(sccan_image *image_to_draw_tripod_in,bool highlight){
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"draw_mirror_tripod -> ";
		std::cout<<std::endl;	
	}
	
	// to normalize the lines and text fonts to be drawed the image size
	// is taken into account
	int number_of_pixels_of_image = 
	image_to_draw_tripod_in->get_number_of_pixels();
	
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
	
	// center position of tripod is the mean position of the
	// enclosing polygon
	cv::Point center_of_tripod = list_of_Points_inside_mirror_polygon.
	get_mean_of_pixel_distribution();
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"draw_mirror_tripod -> ";
		std::cout<<"center of tripod ";
		std::cout<<"x: "<<center_of_tripod.x<<"px, ";
		std::cout<<"y: "<<center_of_tripod.y<<"px, ";
		std::cout<<std::endl;	
	}

	//length of tripod leg
	double tripod_leg_lenght_in_pixels = 
	sqrt(list_of_Points_inside_mirror_polygon.size());
	
	tripod_leg_lenght_in_pixels = tripod_leg_lenght_in_pixels*0.45;
	
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"draw_mirror_tripod -> ";
		std::cout<<"tripod_leg_lenght_in_pixels =  ";
		std::cout<<tripod_leg_lenght_in_pixels<<"px";
		std::cout<<std::endl;	
	}	
	
	double two_third_pi = M_PI*2.0/3.0;
	 
	// the x coordinates in the image are flipped to orientate 
	// x axis as in star camera frame
	 
	//first leg
	cv::Point first_leg;
	first_leg.x = center_of_tripod.x - 
	cos(tripod_orientation_z_in_rad)*tripod_leg_lenght_in_pixels;
	first_leg.y = center_of_tripod.y + 
	sin(tripod_orientation_z_in_rad)*tripod_leg_lenght_in_pixels;
	
	cv::line(
	image_to_draw_tripod_in->image_matrix,
	center_of_tripod,
	first_leg,
	cv::Scalar( 128, 255, 0),//Blue,Green,Red
	line_thickness,
	line_Type );

	cv::putText(
	image_to_draw_tripod_in->image_matrix,
	"1",
	first_leg,
	text_fontFace,// fontFace
	text_fontScale,// fontScale
	cv::Scalar( 0, 255, 128 ),//Blue,Green,Red
	text_thickness/2.0,
	4);
	
	//second leg
	cv::Point second_leg;
	second_leg.x = center_of_tripod.x - 
	cos(two_third_pi+tripod_orientation_z_in_rad)*
	tripod_leg_lenght_in_pixels;
	second_leg.y = center_of_tripod.y + 
	sin(two_third_pi+tripod_orientation_z_in_rad)*
	tripod_leg_lenght_in_pixels;	

	cv::line(
	image_to_draw_tripod_in->image_matrix,
	center_of_tripod,
	second_leg,
	cv::Scalar( 128, 255, 0),//Blue,Green,Red
	line_thickness,
	line_Type );

	cv::putText(
	image_to_draw_tripod_in->image_matrix,
	"2",
	second_leg,
	text_fontFace,// fontFace
	text_fontScale,// fontScale
	cv::Scalar( 0, 255, 128 ),//Blue,Green,Red
	text_thickness/2.0,
	4);

	//third leg
	cv::Point third_leg;
	third_leg.x = center_of_tripod.x - 
	cos(2.0*two_third_pi+tripod_orientation_z_in_rad)*
	tripod_leg_lenght_in_pixels;
	third_leg.y = center_of_tripod.y + 
	sin(2.0*two_third_pi+tripod_orientation_z_in_rad)*
	tripod_leg_lenght_in_pixels;	
	
	cv::line(
	image_to_draw_tripod_in->image_matrix,
	center_of_tripod,
	third_leg,
	cv::Scalar( 128, 255, 0),//Blue,Green,Red
	line_thickness,
	line_Type );
	
	cv::putText(
	image_to_draw_tripod_in->image_matrix,
	"3",
	third_leg,
	text_fontFace,// fontFace
	text_fontScale,// fontScale
	cv::Scalar( 0, 255, 128 ),//Blue,Green,Red
	text_thickness/2.0,
	4);
	
	//==================================================================
		
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"draw_mirror_tripod -> ";
		std::cout<<"end"<<std::endl;	
	}
}
//======================================================================
std::string mirror::
get_manipulation_instructions(pointing_direction
DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse){
	
	calculate_mirror_missalignment(
	DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse);
	
	if(flag_misalignment_angles_have_been_calculated){
		
		calculate_bolt_manipulation_distances();
		
		if(flag_manipulation_distances_have_been_calculated){
			
			calculate_bolt_manipulation_revolutions();
			
			std::stringstream manual;
			manual<<"=================================================";
			manual<<"========================\n";
			manual<<"Mirror ID "<<mirror_ID<<"\n";
			//==========================================================
			// bolts
			//==========================================================			
			manual<<"Bolt 1: ";
			
			if(manipulation_revolutions_of_first_tripod_leg_in_revs>0.0)
			manual<<"+";
			
			manual<<manipulation_revolutions_of_first_tripod_leg_in_revs;
			manual<<"revs / ";
			manual<<manipulation_revolutions_of_first_tripod_leg_in_revs
			*360.0;
			manual<<"deg\n";
			
			manual<<"Bolt 2: ";
			
			if(manipulation_revolutions_of_second_tripod_leg_in_revs>0.0)
			manual<<"+";
			
			manual<<manipulation_revolutions_of_second_tripod_leg_in_revs;
			manual<<"revs / ";
			manual<<manipulation_revolutions_of_second_tripod_leg_in_revs
			*360.0;
			manual<<"deg\n";
			
			manual<<"Bolt 3: ";
			
			if(manipulation_revolutions_of_third_tripod_leg_in_revs>0.0)
			manual<<"+";
			
			manual<<manipulation_revolutions_of_third_tripod_leg_in_revs;
			manual<<"revs / ";
			manual<<manipulation_revolutions_of_third_tripod_leg_in_revs
			*360.0;
			manual<<"deg\n";
			manual<<"\n";
			//==========================================================
			// directions
			//==========================================================
			manual<<"___direction___\n";
			manual<<"direction of star relative to telescope for ";
			manual<<"brightest mirror response:\n";
			manual<<"x: "<<
			DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse.
			get_x_tilt_prompt_in_deg_min_sec();
			manual<<"\n";
			manual<<"y: "<<
			DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse.
			get_y_tilt_prompt_in_deg_min_sec();	
			manual<<"\n";
			manual<<"\n";
			manual<<"direction of mirror relative to telescope which ";
			manual<<"will be compensated:\n";				
			manual<<"x: "<<
			MirrorMisalignmentDirection.
			get_x_tilt_prompt_in_deg_min_sec();
			manual<<"\n";
			manual<<"y: "<<
			MirrorMisalignmentDirection.
			get_y_tilt_prompt_in_deg_min_sec();	
			manual<<"\n";		
			//==========================================================
			// tripod
			//==========================================================
			manual<<"\n";	
			manual<<"__tripod___\n";	
			manual<<"radius     : "<<tripod_radius_in_m<<"m\n";
			manual<<"z rotation : "<<tripod_orientation_z_in_rad<<"rad";			
			manual<<" / "<<tripod_orientation_z_in_rad*(360.0/(2.0*M_PI))<<"deg\n";
			manual<<"bolt pitch : "<<pitch_of_bolt_in_m_per_revolution<<"m/rev\n";
			manual<<"\n";
			manual<<"leg 1 : "<<position_of_first_tripod_leg.get_string()<<"\n";
			manual<<"leg 2 : "<<position_of_second_tripod_leg.get_string()<<"\n";
			manual<<"leg 3 : "<<position_of_third_tripod_leg.get_string()<<"\n";			
			manual<<"\n";
			manual<<"leg 1 offset : "<<manipulation_distance_of_first_tripod_leg_in_m<<"m\n";			
			manual<<"leg 2 offset : "<<manipulation_distance_of_second_tripod_leg_in_m<<"m\n";	
			manual<<"leg 3 offset : "<<manipulation_distance_of_third_tripod_leg_in_m<<"m\n";				
	
			return manual.str();
			
		}else{
			
			std::cout<<"mirror "<<mirror_ID;
			std::cout<<"-> get_manipulation_instructions() -> ";
			std::cout<<"manipulation distances/revolutions ";
			std::cout<<"have not been calculated yet!";
			std::cout<<"\n";
			
			std::stringstream out;
			out<<"mirror -> get_manipulation_instructions() -> ";
			out<<"manipulation distances/revolutions have not been ";
			out<<"calculated yet!\n";
			return out.str();
		}
	
	}else{
		
		std::cout<<"mirror "<<mirror_ID;
		std::cout<<"-> get_manipulation_instructions() -> ";
		std::cout<<"misalignment angles have not been calculated yet!";
		std::cout<<"\n";
		
		std::stringstream out;
		out<<"mirror -> get_manipulation_instructions() -> ";
		out<<"misalignment angles have not been calculated yet!\n";
		return out.str();
	}
}
//======================================================================
void mirror::
calculate_mirror_missalignment(pointing_direction
DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse){
	
	MirrorMisalignmentDirection =
	DirectionOfStarRelativeToTelescopeForBrightesetMirrorResponse*0.5;

	flag_misalignment_angles_have_been_calculated = true;
}
//======================================================================
void mirror::
calculate_bolt_manipulation_distances(){
	
	if(flag_misalignment_angles_have_been_calculated){
		
		// WARNINGS
		if(MirrorMisalignmentDirection.
		direction_in_x_in_radiant > (2.0*M_PI/360.0)*10.0){
			std::cout<<"calculate_bolt_manipulation_distances()->";
			std::cout<<"WARNING missalignment_angle_in_x_in_rad";
			std::cout<<" is above approximation limit 10deg!\n";
		}
		if(MirrorMisalignmentDirection.
		direction_in_y_in_radiant > (2.0*M_PI/360.0)*10.0){
			std::cout<<"calculate_bolt_manipulation_distances()->";
			std::cout<<"WARNING missalignment_angle_in_y_in_rad";
			std::cout<<" is above approximation limit 10deg!\n";
		}
		if(verbosity){
			std::cout<<" __calculate_bolt_manipulation_distances(x:";
			std::cout<<MirrorMisalignmentDirection.
			get_x_tilt_prompt_in_deg_min_sec()<<",y:";
			std::cout<<MirrorMisalignmentDirection.
			get_y_tilt_prompt_in_deg_min_sec()<<")\n";
		}
		
		/** approximation
		 lets assume the initial mirror orientation is always zero in x and y
		 so the initial surface normal vector of the center of the mirror
		 is \vec{n}=(0,0,1)^T
		 **/
		Vector3D ez; ez.set_unit_vector_z();
		
	
		Vector3D old_mirror_x; old_mirror_x.set_unit_vector_x();
		Vector3D old_mirror_y; old_mirror_y.set_unit_vector_y();
		 
		Vector3D new_mirror_x = old_mirror_x + 
		ez*MirrorMisalignmentDirection.direction_in_y_in_radiant;
		
		new_mirror_x = new_mirror_x/new_mirror_x.norm2();
		
		Vector3D new_mirror_y = old_mirror_y + 
		ez*MirrorMisalignmentDirection.direction_in_x_in_radiant;
		
		new_mirror_y = new_mirror_y/new_mirror_y.norm2();
		
		Vector3D new_mirror_surface_normal = 
		new_mirror_x.cross_product(new_mirror_y);
		
		if(verbosity){
			std::cout<<"| new pseudo mirror plane x vector     : ";
			std::cout<<new_mirror_x.get_string()<<std::endl;
			std::cout<<"| new pseudo mirror plane y vector     : ";
			std::cout<<new_mirror_y.get_string()<<std::endl;
			std::cout<<"| new pseudo mirror plane normal vector: ";
			std::cout<<new_mirror_surface_normal.get_string()<<std::endl;
		}	
		
		/// calculate intersection parameter of line 
		/// tripod_orientation_i_in_rad + ez * lambda: lambda.
		
		manipulation_distance_of_first_tripod_leg_in_m = 
		(position_of_first_tripod_leg*(-1.0))*
		new_mirror_surface_normal/
		(ez*new_mirror_surface_normal);

		manipulation_distance_of_second_tripod_leg_in_m = 
		(position_of_second_tripod_leg*(-1.0))*
		new_mirror_surface_normal/
		(ez*new_mirror_surface_normal);
		
		manipulation_distance_of_third_tripod_leg_in_m = 
		(position_of_third_tripod_leg*(-1.0))*
		new_mirror_surface_normal/
		(ez*new_mirror_surface_normal);
		
		if(verbosity){
			std::cout<<std::endl;
			std::cout<<"| manipulation distance 1: ";
			std::cout.precision(10);
			std::cout<<manipulation_distance_of_first_tripod_leg_in_m;
			std::cout<<" [m]"<<std::endl;
			std::cout<<"| manipulation distance 2: ";
			std::cout.precision(10);
			std::cout<<manipulation_distance_of_second_tripod_leg_in_m;
			std::cout<<" [m]"<<std::endl;	
			std::cout<<"| manipulation distance 3: ";
			std::cout.precision(10);
			std::cout<<manipulation_distance_of_third_tripod_leg_in_m;
			std::cout<<" [m]"<<std::endl;
			std::cout<<"|__________________________________"<<std::endl;
		}
	
		tripod_orientation_z_in_rad;
		manipulation_distance_of_first_tripod_leg_in_m;
		manipulation_distance_of_second_tripod_leg_in_m;
		manipulation_distance_of_third_tripod_leg_in_m;

		flag_manipulation_distances_have_been_calculated = true;
		
	}else{
		std::cout<<"mirror "<<mirror_ID;
		std::cout<<"-> calculate_bolt_manipulation_distances() -> ";
		std::cout<<"misalignment angles have not been calculated yet!";
		std::cout<<"\n";
	}
	
}
//======================================================================
void mirror::
set_mirror_tripod(double new_tripod_radius_in_m,
double new_pitch_of_bolt_in_m_per_revolution){
	
	tripod_radius_in_m = new_tripod_radius_in_m;
	
	pitch_of_bolt_in_m_per_revolution = 
	new_pitch_of_bolt_in_m_per_revolution;
	
	init_tripod();
}
//======================================================================
void mirror::
init_tripod(){
	
	double two_third_PI = M_PI*2.0/3.0;
	
	position_of_first_tripod_leg.set_vec3D(
	tripod_radius_in_m*cos(tripod_orientation_z_in_rad),
	tripod_radius_in_m*sin(tripod_orientation_z_in_rad),
	0.0);
	
	position_of_second_tripod_leg.set_vec3D(
	tripod_radius_in_m*cos(tripod_orientation_z_in_rad+two_third_PI),
	tripod_radius_in_m*sin(tripod_orientation_z_in_rad+two_third_PI),
	0.0);
	
	position_of_third_tripod_leg.set_vec3D(
	tripod_radius_in_m*cos(tripod_orientation_z_in_rad+2.0*two_third_PI),
	tripod_radius_in_m*sin(tripod_orientation_z_in_rad+2.0*two_third_PI),
	0.0);
	
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"initialize_tripod() -> ";
		std::cout<<"radius: "<<tripod_radius_in_m<<"m "; 
		std::cout<<"rot z: ";
		std::cout<<tripod_orientation_z_in_rad*360/(2.0*M_PI)<<"deg"; 
		std::cout<<std::endl;
		std::cout<<" first leg pos: "<<position_of_first_tripod_leg.
		get_string();
		std::cout<<", second leg pos: "<<position_of_second_tripod_leg.
		get_string();
		std::cout<<", third leg pos: "<<position_of_third_tripod_leg.
		get_string();
		std::cout<<std::endl;
	}	
}
//======================================================================
void mirror::
calculate_bolt_manipulation_revolutions(){
	if(flag_manipulation_distances_have_been_calculated){
		
		if(verbosity){
			std::cout<<"mirror -> ";
			std::cout<<"calculate_bolt_manipulation_revolutions() -> ";
			std::cout<<"bolt pitch: ";
			std::cout<<pitch_of_bolt_in_m_per_revolution<<" m/revs, ";
			std::cout<<"1) ";
			std::cout<<manipulation_distance_of_first_tripod_leg_in_m<<"m, ";
			std::cout<<"2) ";
			std::cout<<manipulation_distance_of_second_tripod_leg_in_m<<"m, ";
			std::cout<<"3) ";
			std::cout<<manipulation_distance_of_third_tripod_leg_in_m<<"m";
		}
		
		manipulation_revolutions_of_first_tripod_leg_in_revs = 
		manipulation_distance_of_first_tripod_leg_in_m/
		pitch_of_bolt_in_m_per_revolution;

		manipulation_revolutions_of_second_tripod_leg_in_revs = 
		manipulation_distance_of_second_tripod_leg_in_m/
		pitch_of_bolt_in_m_per_revolution;
		
		manipulation_revolutions_of_third_tripod_leg_in_revs = 
		manipulation_distance_of_third_tripod_leg_in_m/
		pitch_of_bolt_in_m_per_revolution;	

		if(verbosity){
			std::cout<<"mirror -> ";
			std::cout<<"calculate_bolt_manipulation_revolutions() -> ";
			std::cout<<"1) ";
			std::cout<<manipulation_revolutions_of_first_tripod_leg_in_revs<<"revs, ";
			std::cout<<"2) ";
			std::cout<<manipulation_revolutions_of_second_tripod_leg_in_revs<<"revs, ";
			std::cout<<"3) ";
			std::cout<<manipulation_revolutions_of_third_tripod_leg_in_revs<<"revs";
		}			
		
	}else{
		std::cout<<"mirror -> ";
		std::cout<<"calculate_bolt_manipulation_revolutions() -> ";	
		std::cout<<"manipulation distances have been calculated\n";		
	}
}

