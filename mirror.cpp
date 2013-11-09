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
	flag_manipulations_have_been_calculated = false;
	tripod_orientation_z_in_rad =0.0;
	
	// mask sccan_image
	flag_mask_has_been_initialized = false;
	list_of_points_defining_mirror_polygon.clear();
	list_of_Points_inside_mirror_polygon.clear();
		
	// configuration file
	key_ID = "ID";
	key_tripod_orientation_z_in_rad ="tripod_orientation_z_in_rad";	
	key_list_of_points_defining_mirror_polygon = "polygon";
	key_polygon_point_pixel_coordinate_x = "x";
	key_polygon_point_pixel_coordinate_y = "y";
}
//======================================================================
std::string mirror::get_string(){
	std::stringstream out; out.str("");
	out<<"Mirror ID: "<<mirror_ID<<std::endl;
	out<<"| tripod rotation in z: "<<tripod_orientation_z_in_rad;
	out<<" [rad]"<<std::endl;
	out<<"| number of polygon knots: ";
	out<<list_of_points_defining_mirror_polygon.size()<<std::endl;
	out<<"| polygon list: ";
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
		out<<" [pixels]";
	}
	out<<std::endl;	
	if(flag_mask_has_been_initialized)
	{
	out<<"| mirror mask: "<<std::endl;
	out<<"| number of pixels in polygon: ";
	out<<list_of_Points_inside_mirror_polygon.size()<<std::endl;
	}else{
	out<<"| mirror mask has not been initialized yet."<<std::endl;
	}
	if(flag_misalignment_angles_have_been_calculated){
	out<<"| mirror misalignment: "<<std::endl;
	out<<"| missalignment angle in x: ";
	out<<missalignment_angle_in_x_in_rad<<" [rad]"<<std::endl;
	out<<"| missalignment angle in y: ";
	out<<missalignment_angle_in_y_in_rad<<" [rad]"<<std::endl;
	}else{
	out<<"| mirror misalignment has not been calculated yet."<<std::endl;
	}
	
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
	add(key_tripod_orientation_z_in_rad, libconfig::Setting::TypeFloat) = 
	tripod_orientation_z_in_rad;
	
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
			std::cout<<"mirror -> import_configuration() -> initialize ID ";
			std::cout<<new_mirror_ID<<std::endl;
		}
	}else{
		flag_mirror_valid = false;
	}
	//==================================================================
	if(Mirror_file_handle->lookupValue(
	key_tripod_orientation_z_in_rad,
	 tripod_orientation_z_in_rad)){
		if(verbosity){
			std::cout<<"mirror -> import_configuration() ->";
			std::cout<<" tripod orientation loaded ";
			std::cout<<tripod_orientation_z_in_rad<<"rad."<<std::endl;
		}	
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
		std::cout<<"initialize_list_of_points_inside_mirror_polygon()"<<std::endl;
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
			std::cout<<"|"<<polygon[0][point_itterator].y<<")"<<std::endl;
		}
	}
	
	const cv::Point* ppt[1] = { polygon[0] };
	
	int npt[] = {list_of_points_defining_mirror_polygon.size()};
	
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
list_of_pixel_positions* mirror::get_pointer_to_list_of_Points_inside_mirror_polygon(){
	return &list_of_Points_inside_mirror_polygon;
}
//======================================================================
simple_image mirror::highlight_mirror_in_reflector_image
(simple_image* pointer_to_reflector_image){

	// calculate ROI
	cv::Rect mirror_region_of_interest= boundingRect(
	list_of_points_defining_mirror_polygon);


	//image_with_mirror_mask.disp_simple_image("mask");
	
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
		
	simple_image highlited_image = *pointer_to_reflector_image;
	// highlight pixels
	for(int pixel_to_highlight_itterator = 0;
	pixel_to_highlight_itterator<
	list_of_Points_inside_mirror_polygon.size();
	pixel_to_highlight_itterator++)
	{
		cv::Vec3b intensity = highlited_image.image_matrix.at<cv::Vec3b>
		(list_of_Points_inside_mirror_polygon.
		at(pixel_to_highlight_itterator));
		// BGR

		intensity[2] = (int)( (float)intensity[2]*1.5 );
		if((int)intensity[2]>255)
			intensity[2] = (uchar)255;	
	
		highlited_image.image_matrix.at<cv::Vec3b>
		(list_of_Points_inside_mirror_polygon.
		at(pixel_to_highlight_itterator))=intensity;
	}
	//highlited_image.disp_simple_image("highlited mirror");
	
	simple_image highlited_image_roi;
	highlited_image_roi.image_matrix =
	highlited_image.image_matrix(mirror_region_of_interest).clone();
	return highlited_image_roi;
}
//======================================================================

