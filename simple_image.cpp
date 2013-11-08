#include "simple_image.h"
//======================================================================
simple_image::simple_image(){
	//verbosity = true;
	image_format	= CV_8UC3;
}
//======================================================================
simple_image::simple_image(bool new_verbosity){
	verbosity = new_verbosity;
	image_format	= CV_8UC3;
}
//======================================================================
void simple_image::disp_simple_image(std::string window_name){
	disp_simple_image(window_name,0);
}
//======================================================================
void simple_image::disp_simple_image
(std::string window_name,int time_to_display_in_ms){
		
		if(verbosity){
			std::cout<<"simple_image -> ";
			std::cout<<"disp_simple_image() -> ";
			std::cout<<window_name<<" for "<<time_to_display_in_ms<<"ms.";
			std::cout<<std::endl;
		}
		
		// show number of mega pixels in window name
		double number_of_mega_pixels=
		ceil(get_number_of_pixels()/1.0e6);
		
		std::stringstream window_name_stream;
		window_name_stream<<window_name<<" ";
		window_name_stream<<std::setprecision(2);
		window_name_stream<<number_of_mega_pixels<<"Mpx";
		window_name = window_name_stream.str();
		
		// Create a window for display.
		cv::namedWindow( 
		window_name.c_str(),
		CV_WINDOW_NORMAL 
		);
		
		// Show our sccan_image inside window
		imshow( 
		window_name.c_str(),
		image_matrix
		);  
		
		// wait for key
		std::stringstream out;
		out<<"disp_simple_image() ->";
		out<<" press any key to leave ";
		out<<window_name<<std::endl;
		
		cv::waitKey(time_to_display_in_ms);
		
		// close window
		cv::destroyWindow
		(window_name.c_str());  
}
//======================================================================
bool simple_image::read_in_image
(std::string file_name_of_image_to_load){
	// load simple image
	image_matrix = 
	cv::imread(file_name_of_image_to_load,CV_LOAD_IMAGE_COLOR);
	
	// test wether loading was successfull or not
	if(! image_matrix.data )// Check for invalid input
	{
		std::cout<<"simple_image -> read_in_image()->";
		std::cout<<"Could not open or find:";
		std::cout<<file_name_of_image_to_load<<std::endl;
		return false;
	}else{
		if(verbosity){
			std::cout<<"simple_image -> read_in_image()->";
			std::cout<<"loading successful: ";
			std::cout<<file_name_of_image_to_load<<std::endl;
		}
		return true;	
	}
}
//======================================================================
bool simple_image::save_simple_image
(std::string file_name_of_image_to_save){
	//save simple image
	try {
		cv::imwrite(file_name_of_image_to_save,image_matrix);
	}
	catch (std::runtime_error& ex) {
		std::cout<<"simple_image -> ";
		std::cout<<"save_simple_image() -> ";
		std::cout<<"ERROR -> ";
		std::cout<<file_name_of_image_to_save;
		std::cout<<"Exception converting simple_image to PNG format: ";
		std::cout<<ex.what()<<std::endl;
		return false;
	}
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"save_simple_image() -> ";
		std::cout<<"simple_image ";
		std::cout<<file_name_of_image_to_save<<" exported."<<std::endl;
	}
	
	return true;	
}
//======================================================================
int simple_image::get_number_of_pixels(){
	return image_matrix.rows*image_matrix.cols; 
}
//======================================================================
bool simple_image::check_exposure_time
(	double lower_limit_for_brightest_pixel,
	double upper_limit_for_brightest_pixel,
	bool *expose_longer)
{

	if(lower_limit_for_brightest_pixel>=
	 upper_limit_for_brightest_pixel){
		std::cout<<"check_exposure_time() -> ";
		std::cout<<"limits are messed up! -> ";
		std::cout<<"lower limit"<<lower_limit_for_brightest_pixel;	
		std::cout<<" upper limit"<<upper_limit_for_brightest_pixel<<std::endl;	
	}
	
	double minimal_pixel_value;
	double maximal_pixel_value;
	cv::Point minimal_pixel_location;
	cv::Point maximal_pixel_location;
	
	cv::Mat	image_to_check;
	image_matrix.copyTo(image_to_check);
	
	if(image_to_check.type()==CV_8UC3){
		cv::cvtColor(image_to_check,image_to_check, CV_BGR2GRAY);
	}
	
	cv::minMaxLoc(
	image_to_check,
	&minimal_pixel_value,
	&maximal_pixel_value,
	&minimal_pixel_location,
	&maximal_pixel_location
	);

	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"check_exposure_time() -> ";
		std::cout<<"min value: "<<minimal_pixel_value<<" at ";
		std::cout<<"("<<minimal_pixel_location.x<<"|";
		std::cout<<minimal_pixel_location.y<<") ";
		std::cout<<"max value: "<<maximal_pixel_value<<" at ";
		std::cout<<"("<<maximal_pixel_location.x<<"|";
		std::cout<<maximal_pixel_location.y<<") ";
		std::cout<<std::endl;
	}
	

	
	if(maximal_pixel_value < upper_limit_for_brightest_pixel &&
	maximal_pixel_value > lower_limit_for_brightest_pixel)
	{
		if(verbosity){
			std::cout<<"simple_image -> ";
			std::cout<<"check_exposure_time() -> ";
			std::cout<<"exposure time is good: ";
			std::cout<<lower_limit_for_brightest_pixel<<"<";
			std::cout<<maximal_pixel_value<<"<";
			std::cout<<upper_limit_for_brightest_pixel<<std::endl;
		}
		return true;
	}else if(maximal_pixel_value >= upper_limit_for_brightest_pixel)
	{
		std::cout<<"simple_image -> ";
		std::cout<<"check_exposure_time() -> ";
		std::cout<<"exposure time is too long: ";
		std::cout<<lower_limit_for_brightest_pixel<<"<";
		std::cout<<maximal_pixel_value<<"<";
		std::cout<<upper_limit_for_brightest_pixel<<std::endl;
		*expose_longer = false; 
		return false;
	}else if(maximal_pixel_value <= lower_limit_for_brightest_pixel){
		std::cout<<"check_exposure_time() -> ";
		std::cout<<"exposure time is too short: ";
		std::cout<<lower_limit_for_brightest_pixel<<"<";
		std::cout<<maximal_pixel_value<<"<";
		std::cout<<upper_limit_for_brightest_pixel<<std::endl;
		*expose_longer = true; 
		return false;	
	}else{
		std::cout<<"simple_image -> ";
		std::cout<<"check_exposure_time() -> ";
		std::cout<<"THIS CASE MUST NEVER HAPPEN: ";
		std::cout<<lower_limit_for_brightest_pixel<<"<";
		std::cout<<maximal_pixel_value<<"<";
		std::cout<<upper_limit_for_brightest_pixel<<std::endl;
		*expose_longer = true; 
		return true;		
	}
}
//======================================================================
void simple_image::operator=(simple_image eq){
	//~ if(verbosity){
		//~ std::cout<<"simple_image -> operator=() ->"<<std::endl;
	//~ }
	verbosity 	= eq.verbosity;
	eq.image_matrix.copyTo(image_matrix);
	image_format		= eq.image_format;
}
//======================================================================
// image processing 
//======================================================================
cv::Mat simple_image::create_mean_image(){
	cv::Mat mean_image = cv::Mat::ones
	(image_matrix.rows,image_matrix.cols,image_format);
	
	cv::Scalar mean_of_image = mean(image_matrix);
	if(verbosity){
		std::cout<<"simple_image -> create_mean_image() -> mean ";
		std::cout<<"R:"<<mean_of_image[0];
		std::cout<<" B:"<<mean_of_image[1];
		std::cout<<" G:"<<mean_of_image[2]<<std::endl;
	}
	
	std::vector<cv::Mat> split_mean_image;
	
	split_mean_image.push_back(
	cv::Mat::ones(image_matrix.rows,image_matrix.cols,CV_8UC1)*
	mean_of_image[0]);
	split_mean_image.push_back(
	cv::Mat::ones(image_matrix.rows,image_matrix.cols,CV_8UC1)*
	mean_of_image[1]);
	split_mean_image.push_back(
	cv::Mat::ones(image_matrix.rows,image_matrix.cols,CV_8UC1)*
	mean_of_image[2]);
	cv::merge(split_mean_image,mean_image);
	
	return mean_image;
}
//======================================================================
cv::Mat simple_image::create_median_image
(double kernel_radius_one_sigma_in_pixels){
	
	int odd_kernel_radius_one_sigma_in_pixels = (int)
	(round(round(kernel_radius_one_sigma_in_pixels)/2.0))*2.0+1.0;
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"create_median_image() -> ";
		std::cout<<"kernel one sigma radius ";
		std::cout<<kernel_radius_one_sigma_in_pixels<<"px ";
		std::cout<<"rounded to odd number ";
		std::cout<<odd_kernel_radius_one_sigma_in_pixels<<"px";
		std::cout<<std::endl;
	}
		
	cv::Mat median_smoothed_image;
	median_smoothed_image.
	create(image_matrix.rows,image_matrix.cols,image_format);
	
	cv::medianBlur(
	image_matrix,median_smoothed_image,
	odd_kernel_radius_one_sigma_in_pixels
	);
	
	return median_smoothed_image;
}
//======================================================================
double simple_image::calculate_pixelvalue_standard_deviation(){

	double fraction_of_pixels_to_test = 0.0001;
	
	int number_of_pixels_in_image = image_matrix.rows*image_matrix.cols;
	int number_of_pixels_to_test = (int)
	ceil((double)number_of_pixels_in_image*fraction_of_pixels_to_test);
//======================================================================	
	if(verbosity){
		std::cout<<"simple_image -> calculate_pixelvalue_standard_deviation() ->";
		std::cout<<"testing "<<number_of_pixels_to_test<<" pixels of ";
		std::cout<<image_matrix.rows<<"x"<<image_matrix.cols<<"pixels.";
		std::cout<<std::endl;
	}
	
	cv::Mat image_to_check;
	image_matrix.copyTo(image_to_check);
	cv::cvtColor(image_to_check,image_to_check, CV_RGB2GRAY);
	
	std::vector<double> list_of_pixel_differences;
	
	cv::RNG OpenCVPseudoRandomNumberGenerator( 0xFFFFFFFF );
	
	for(
	int pixel_itterator = 0;
	pixel_itterator<number_of_pixels_to_test;
	pixel_itterator++)
	{
		cv::Point point_1;
		point_1.y = OpenCVPseudoRandomNumberGenerator.
		uniform(0,image_matrix.rows);
		
		point_1.x = OpenCVPseudoRandomNumberGenerator.
		uniform(0,image_matrix.cols);
		
		cv::Point point_2;
		point_2.y = OpenCVPseudoRandomNumberGenerator.
		uniform(0,image_matrix.rows);
		
		point_2.x = OpenCVPseudoRandomNumberGenerator.
		uniform(0,image_matrix.cols);	
		
		cv::Scalar intensity_1 = 
		image_matrix.at<uchar>(point_1);
		cv::Scalar intensity_2 = 
		image_matrix.at<uchar>(point_2);
		
		//~ std::cout<<"test ("<<point_1.x<<"|"<<point_1.y<<")";
		//~ std::cout<<":("<<intensity_1[0]<<"|"<<intensity_1[1]<<"|"<<intensity_1[2]<< ")";
		//~ std::cout<<" and ("<<point_2.x<<"|"<<point_2.y<<")";
		//~ std::cout<<":("<<intensity_2[0]<<"|"<<intensity_2[1]<<"|"<<intensity_2[2]<< ")";		
		//~ std::cout<<std::endl;
		
		double pixel_flux_differences = 
		abs((double)intensity_1[0]-(double)intensity_2[0]);

		//~ std::cout<<"intensity diff R:"<<pixel_flux_difference_in_R;
		//~ std::cout<<"B:"<<pixel_flux_difference_in_B;
		//~ std::cout<<"G:"<<pixel_flux_difference_in_G;
		//~ std::cout<<std::endl;
		
		list_of_pixel_differences.
		push_back(pixel_flux_differences);
		
		//~ std::cout<<"intensity diff R:";
		//~ std::cout<<list_of_pixel_differences_in_R.at(pixel_itterator);
		//~ std::cout<<"B:"<<list_of_pixel_differences_in_B.at(pixel_itterator);
		//~ std::cout<<"G:"<<list_of_pixel_differences_in_G.at(pixel_itterator);
		//~ std::cout<<std::endl;
		
	}	
	
	// Automatic recognition and calibration of
	// astronomical images
	// thesis of Dustin Lang
	double variance_of_pixel_differences = 
	calculate_variance(&list_of_pixel_differences);
	
	double sigma_in_pxv = 
	sqrt(variance_of_pixel_differences/2.0);
	
	if(verbosity){
		std::cout<<"simple_image -> calculate_pixelvalue_standard_deviation() ->";
		std::cout<<"sigma "<<sigma_in_pxv<<"pxv"<<std::endl;
	}	
	
	return sigma_in_pxv;
}
//======================================================================
double simple_image::calculate_variance
(std::vector<double> *pointer_to_list_of_data){
	double number_of_datii = (double)pointer_to_list_of_data->size();
	double sum = 0.0;
	double mean = 0.0;
	
	for(int data_itterator = 0;
	data_itterator<pointer_to_list_of_data->size();
	data_itterator++)
	{
		mean = mean + pointer_to_list_of_data->at(data_itterator);
	}
	mean = mean/number_of_datii;
	//std::cout<<"mean "<<mean<<std::endl;

	for(int data_itterator = 0;
	data_itterator<pointer_to_list_of_data->size();
	data_itterator++)
	{
		sum = sum + 
		(pointer_to_list_of_data->at(data_itterator) - mean)*
		(pointer_to_list_of_data->at(data_itterator) - mean);
	}	
	
	double variance = 1.0/(number_of_datii-1.0) * sum;	
	
	if(verbosity){
		std::cout<<"calculate_variance() -> "<<number_of_datii;
		std::cout<<" data points, mean "<<mean<<" standart deviation ";
		std::cout<<variance<<std::endl;
	}
	return variance;
}
//======================================================================
list_of_pixel_positions simple_image::create_list_of_Points_of_non_zero_pixels(){
	
	list_of_pixel_positions list_of_Points_of_non_zero_pixels;
	
	cv::Scalar intensity;
	cv::Point temporary_point;
	for(int row=0;
		row<image_matrix.rows;
		row++)
	{
		for(int column = 0;
			column<image_matrix.cols;
			column++)
		{
			// get the pixel intensity of the mask at specific
			// row and column
			intensity=
			image_matrix.at<uchar>(row,column);
			
			// check wether the mask value is set to (255) or (0)
			if(	(int)intensity.val[0] > 0  )
			{
				temporary_point.y = row;
				temporary_point.x = column;
				list_of_Points_of_non_zero_pixels.
				push_back(temporary_point);
			}
		}
	}
	if(verbosity)
	{
		std::cout<<"simple_image -> ";
		std::cout<<"create_list_of_Points_of_non_zero_pixels()";
		std::cout<<" -> ";
		std::cout<< list_of_Points_of_non_zero_pixels.size();
		std::cout<<" pixels found in mask."<<std::endl;
	}
	return list_of_Points_of_non_zero_pixels;
}
//======================================================================
void simple_image::get_brightest_pixel_of_image
(cv::Point *maximal_pixel_location,double *maximal_pixel_value){

	double minimal_pixel_value;
	cv::Point minimal_pixel_location;
	
	cv::Mat	image_to_check;
	image_matrix.copyTo(image_to_check);
	
	if(image_to_check.type()==CV_8UC3){
		cv::cvtColor(image_to_check,image_to_check, CV_BGR2GRAY);
		if(verbosity){
			std::cout<<"simple_image -> ";
			std::cout<<"get_brightest_pixel_of_image() -> ";
			std::cout<<"color -> gray scale"<<std::endl;
		}
	}
	
	cv::minMaxLoc(
	image_to_check,
	&minimal_pixel_value,
	maximal_pixel_value,
	&minimal_pixel_location,
	maximal_pixel_location
	);		
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"get_brightest_pixel_of_image() -> ";
		std::cout<<"("<<maximal_pixel_location->x;
		std::cout<<"|"<<maximal_pixel_location->y<<") ";
		std::cout<<*maximal_pixel_value<<"pxv"<<std::endl;
	}
}
//======================================================================
void simple_image::get_relative_min_and_max_pixel_value(
double *min_relative_pixel_value,double *max_relative_pixel_value){
	


	cv::Mat	image_to_check;
	image_matrix.copyTo(image_to_check);
	
	if(image_to_check.type()==CV_8UC3){
		cv::cvtColor(image_to_check,image_to_check, CV_BGR2GRAY);
		if(verbosity){
			std::cout<<"simple_image -> ";
			std::cout<<"get_relative_min_and_max_pixel_value() -> ";
			std::cout<<"color -> gray scale"<<std::endl;
		}
	}
	
	cv::Point minimal_pixel_location;
	cv::Point maximal_pixel_location;
	
	cv::minMaxLoc(
	image_to_check,
	min_relative_pixel_value,
	max_relative_pixel_value,
	&minimal_pixel_location,
	&maximal_pixel_location
	);		
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"get_relative_min_and_max_pixel_value() -> ";
		std::cout<<"("<<maximal_pixel_location.x;
		std::cout<<"|"<<maximal_pixel_location.y<<") ";
		std::cout<<*max_relative_pixel_value<<"pxv"<<std::endl;
	}
	
	*min_relative_pixel_value = *min_relative_pixel_value/255.0;
	*max_relative_pixel_value = *max_relative_pixel_value/255.0;
}
//======================================================================
simple_image simple_image::get_overlay_with_stars(std::vector<star_in_image> list_of_stars,
cv::Scalar color_of_ring){
	
	list_of_pixel_positions list_of_star_positions;
	
	for(int star_itterator=0;
	star_itterator<list_of_stars.size();
	star_itterator++){
		list_of_star_positions.
		push_back(list_of_stars.at(star_itterator).get_position_in_pixels());
	}
	
	return get_overlay_with_stars(list_of_star_positions,color_of_ring);
	
}
//======================================================================
simple_image simple_image::get_overlay_with_stars
(list_of_pixel_positions list_of_star_positions,cv::Scalar color_of_ring){
	
	simple_image image_with_star_overlay;
	image_with_star_overlay = *this;
	
	// to normalize the lines and text fonts to be drawed the image size
	// is taken into account
	int number_of_pixels_of_image = get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);
	
	double line_thickness_per_image_line = 2.0/1e3;
	double circle_radius_in_pixel_per_image_line = 15.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 1.5/1e3;
	double text_thickness_per_image_line  = 2/1e3;	

	// line specific information
	int line_thickness = (int)
	ceil(line_thickness_per_image_line*mean_number_of_image_lines);
	int line_Type = 8;		
	 
	// mirror ID spcific information
	cv::Point location_of_mirror_ID;
	int 	text_thickness = (int)
	ceil(text_thickness_per_image_line*mean_number_of_image_lines);
	double 	text_fontScale = 1.5;
	int 	text_fontFace = CV_FONT_HERSHEY_SIMPLEX;
	
	// circle spcific information
	double circle_radius_in_pixel = 
	circle_radius_in_pixel_per_image_line*mean_number_of_image_lines;
	int circle_thickness_in_pixel = 
	(int)ceil(
	circle_thickness_in_pixel_per_image_line*mean_number_of_image_lines
	);
	int circle_lineType	=8;	
	
	if(verbosity){
		std::cout<<"simple_image -> get_overlay_with_stars() ->";
		std::cout<<"circle_radius_in_pixel "<<circle_radius_in_pixel<<std::endl;
	}
	//==================================================================
	// for all stars
	//==================================================================
	for(int star_itterator =0;
	star_itterator<list_of_star_positions.size();
	star_itterator++){
		
		if(verbosity){
			std::cout<<"simple_image -> get_overlay_with_stars() ->";
			std::cout<<"star "<<star_itterator+1;
			std::cout<<" at ("<<list_of_star_positions.at(star_itterator).x;
			std::cout<<"|"<<list_of_star_positions.at(star_itterator).y;
			std::cout<<")px."<<std::endl;
		}
		
		//circle
		cv::circle( 
		image_with_star_overlay.image_matrix,
		list_of_star_positions.at(star_itterator),
		circle_radius_in_pixel,
		color_of_ring,
		circle_thickness_in_pixel,
		circle_lineType );	
	}
	return image_with_star_overlay;	
}
//======================================================================
simple_image simple_image::get_resized_image
(int height_of_alignment_image,double* scaling_factor_for_image){
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"get_resized_image() -> ";
		std::cout<<"start"<<std::endl;		
	}
	
	cv::Size size_of_image = image_matrix.size();
	
	*scaling_factor_for_image = 
	(double)height_of_alignment_image/
	(double)size_of_image.height;
	
	cv::Size new_size_of_image; 
	new_size_of_image.height = height_of_alignment_image;
	new_size_of_image.width  = (int)round
	((*scaling_factor_for_image)*size_of_image.width);
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"get_resized_image() -> ";
		std::cout<<"new size of image";
		std::cout<<"("<<new_size_of_image.height;
		std::cout<<"|"<<new_size_of_image.width<<") ";
		std::cout<<std::endl;
	}
	
	simple_image resized_image;
	
	cv::resize(
	image_matrix,
	resized_image.image_matrix,
	new_size_of_image
	);
	
	return resized_image;
}
//======================================================================
std::vector<list_of_pixel_positions> simple_image::spatial_clustering_of_threshold_mask(){
	
	bool show_controll_images = false;
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"spatial_clustering_of_threshold_mask() -> ";
		std::cout<<std::endl;
		if(show_controll_images)
		disp_simple_image("spatial_clustering(threshold image)");
	}
	
	std::vector<list_of_pixel_positions> list_of_clusters;
	list_of_clusters.clear();
	int cluster_itterator = 0;
	const int cluster_limit = 25;
	bool there_are_still_bright_pixel_not_part_of_a_cluster;
	do{
		there_are_still_bright_pixel_not_part_of_a_cluster=false;
		
		cv::Point 	brightest_pixel_point; 
		double 		brightest_pixel_value;
	
		get_brightest_pixel_of_image
		(&brightest_pixel_point,&brightest_pixel_value);
		
		if(verbosity){
			std::cout<<"simple_image -> ";
			std::cout<<"spatial_clustering_of_threshold_mask() -> ";
			std::cout<<"brightest pixel checked ";
			std::cout<<"("<<brightest_pixel_point.x<<"|";
			std::cout<<brightest_pixel_point.y<<") ";
			std::cout<<brightest_pixel_value<<"pxv."<<std::endl;
		}		
		
		// check whether the brightest Point is valid or not
		if(brightest_pixel_value > 0.0){
			if(verbosity){
				std::cout<<"simple_image -> ";
				std::cout<<"spatial_clustering_of_threshold_mask() -> ";
				std::cout<<"there is still at least one pixel ";
				std::cout<<"not part of a cluster because the brightest ";
				std::cout<<"pixel is > 0.0"<<std::endl;
			}
			there_are_still_bright_pixel_not_part_of_a_cluster = true;
			
			// flood the remaining pixels!
			cv::Point seed_point_for_flood = brightest_pixel_point;
		
			if(verbosity){
				std::cout<<"simple_image -> ";
				std::cout<<"spatial_clustering_of_threshold_mask() -> ";
				std::cout<<"seed point for next flood ";
				std::cout<<"("<<seed_point_for_flood.x;
				std::cout<<"|"<<seed_point_for_flood.y<<")px"<<std::endl;
			}			
			
			simple_image flood_mask;
			flood_mask.image_matrix.create(
			image_matrix.rows+2,
			image_matrix.cols+2,
			CV_8UC1
			);
			
			flood_mask.image_matrix = 
			cv::Mat::zeros(
			image_matrix.rows+2,
			image_matrix.cols+2,
			CV_8UC1);
			
			if(show_controll_images){
				flood_mask.disp_simple_image
				("flood mask before flood +2 rows and columns");
			}

			/*
			flood_mask = threshold_mask;
			cv::cvtColor(
			threshold_mask.image_matrix,
			flood_mask.image_matrix,
			CV_BGR2GRAY);
			*/
			//cv::Scalar new_intensity;
			//new_intensity[0]=255;
			uchar fillValue = 128;
			cv::floodFill(
			image_matrix,
			flood_mask.image_matrix,
			seed_point_for_flood,
			cv::Scalar(255),
			0,
			cv::Scalar(),
			cv::Scalar(),
			4 | cv::FLOODFILL_MASK_ONLY | (fillValue << 8)
			);
			
			if(show_controll_images){
				flood_mask.disp_simple_image
				("flood mask after flood +2 rows and columns");
			}
			
			flood_mask.image_matrix = flood_mask.image_matrix(
			cv::Range(1,image_matrix.rows-1),
			cv::Range(1,image_matrix.cols-1)
			);

			if(show_controll_images){
				flood_mask.disp_simple_image(
				"flood mask after flood rows and columns as star image"
				);
			}
			
			// create list of all pixels part of new cluster
			list_of_pixel_positions list_of_points_of_next_cluster = 
			flood_mask.create_list_of_Points_of_non_zero_pixels();
			
			list_of_clusters.push_back(list_of_points_of_next_cluster);
			
			// increase cluster itterator
			cluster_itterator++;
			
			// remove new cluseter from threshold mask
			cv::Vec3b zero_intensity;
			zero_intensity[0]=0;	
			zero_intensity[1]=0;
			zero_intensity[2]=0;
				
			for(int point_itterator = 0;
			point_itterator<list_of_points_of_next_cluster.size();
			point_itterator++){
				image_matrix.at<cv::Vec3b>(
				list_of_points_of_next_cluster.at(point_itterator)
				) 
				= zero_intensity;
			}
		
			if(verbosity){
				std::cout<<"simple_image -> ";
				std::cout<<"spatial_clustering_of_threshold_mask() -> ";
				std::cout<<"update threshold mask"<<std::endl;
			}
		}else{
		
			if(verbosity){
				std::cout<<"simple_image -> ";
				std::cout<<"spatial_clustering_of_threshold_mask() -> ";
				std::cout<<"all clusters have been found"<<std::endl;
			}
			there_are_still_bright_pixel_not_part_of_a_cluster = false;		
		}
	}while(there_are_still_bright_pixel_not_part_of_a_cluster &&
	cluster_limit>=cluster_itterator);
	
		
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"spatial_clustering_of_threshold_mask() -> ";
		std::cout<<list_of_clusters.size()<<" cluster(s) have been found.";
		std::cout<<std::endl;
	}	
	
	return list_of_clusters;
}
//======================================================================
// image overlays
//======================================================================
void simple_image::text_overlay(
std::string text_to_overlay,
double relative_position_horizontal,
double relative_position_vertical,double relative_font_size){
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"text_overlay() -> ";
		std::cout<<text_to_overlay<<std::endl;
	}
	
	if(	relative_position_horizontal < 0.0 || 
		relative_position_horizontal >1.0){
		std::cout<<"simple_image -> ";
		std::cout<<"text_overlay() -> ";
		std::cout<<"relative_position_horizontal not in range [0,1] !"<<std::endl;		
		return;
	}

	if(	relative_position_vertical < 0.0 || 
		relative_position_vertical >1.0){
		std::cout<<"simple_image -> ";
		std::cout<<"text_overlay() -> ";
		std::cout<<"relative_position_horizontal not in range [0,1] !"<<std::endl;		
		return;
	}

	cv::Point text_location;
	
	text_location.x = (int)
	((double)image_matrix.cols*relative_position_horizontal);
	
	text_location.y = (int)
	((double)image_matrix.rows*relative_position_vertical);
	
	//==================================================================
	// add text
	//==================================================================
	text_overlay(text_to_overlay,text_location,relative_font_size);

	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"text_overlay() -> ";
		std::cout<<"text position: relative ";
		std::cout<<"("<<relative_position_horizontal<<"|";
		std::cout<<relative_position_vertical<<"), absolute ";
		std::cout<<"("<<text_location.x<<"|";
		std::cout<<text_location.y<<")px"<<std::endl;
	}

}
//======================================================================
void simple_image::text_overlay(std::string text_to_overlay,cv::Point text_position,
double relative_font_size){

	if(relative_font_size<=0.0){
		std::cout<<"simple_image -> ";
		std::cout<<"text_overlay() -> ";
		std::cout<<"relative_font_size "<<relative_font_size<<"<=0.0 !";
		std::cout<<std::endl;
		return;
	}

	//==================================================================
	// add text
	//==================================================================
	
	int number_of_pixels_of_image = get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);

	double text_thickness_per_image_line  = 1/1e3;	
	double text_font_scale_per_image_line  = 1/1e3;
	 
	int 	text_thickness = (int)
	ceil(text_thickness_per_image_line*mean_number_of_image_lines);
	
	double 	text_fontScale = 
	text_font_scale_per_image_line*mean_number_of_image_lines;
	
	int 	text_fontFace = CV_FONT_HERSHEY_SIMPLEX;	

	//exposure reflector image
	cv::putText(
	image_matrix,
	text_to_overlay,
	text_position,
	text_fontFace,// fontFace
	text_fontScale,// fontScale
	cv::Scalar( 64, 64, 255 ),//Blue,Green,Red
	text_thickness,
	8
	);

	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"text_overlay() -> ";
		std::cout<<"text position absolute ";
		std::cout<<"("<<text_position.x<<"|";
		std::cout<<text_position.y<<")px"<<std::endl;
	}
	
}
//======================================================================
void simple_image::horizontal_concatenate_with_delimiter_line(
simple_image* pointer_to_left_image,
simple_image* pointer_to_right_image){
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"horizontal_concatenate_with_delimiter_line -> ";
		std::cout<<"start";
		std::cout<<std::endl;
	}

	simple_image delimiter_line;
	delimiter_line.image_matrix = cv::Mat::zeros(
	pointer_to_left_image->image_matrix.rows,
	3,
	image_format
	);
	delimiter_line.image_matrix = cv::Scalar(0,0,255);//BGR
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"horizontal_concatenate_with_delimiter_line -> ";
		std::cout<<"left image ";
		std::cout<<pointer_to_left_image->image_matrix.cols<<"X";
		std::cout<<pointer_to_left_image->image_matrix.rows<<"px, ";
		std::cout<<"right image ";
		std::cout<<pointer_to_right_image->image_matrix.cols<<"X";
		std::cout<<pointer_to_right_image->image_matrix.rows<<"px, ";
		std::cout<<std::endl;
	}

	// concatenate left image with delimiter line 
	cv::hconcat(
	pointer_to_left_image->image_matrix,
	delimiter_line.image_matrix,
	image_matrix);
	
	//concatenate left image and delimiter line with right image
	cv::hconcat(
	image_matrix,
	pointer_to_right_image->image_matrix,
	image_matrix);
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"horizontal_concatenate_with_delimiter_line -> ";
		std::cout<<"end";
		std::cout<<std::endl;
	}
}
//======================================================================
/*
void circle_overlay(cv::Point circle_position,
double relative_circle_size){
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"circle_overlay() -> ";
		std::cout<<"start";
		std::cout<<std::endl;
	}
	
	if(relative_circle_size<=0.0){
		std::cout<<"simple_image -> ";
		std::cout<<"circle_overlay() -> ";
		std::cout<<"relative_circle_size "<<relative_circle_size<<"<=0.0 !";
		std::cout<<std::endl;		
		return;
	}
	
	double circle_radius_in_pixel_per_image_line = 
	relative_circle_size*25.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 
	relative_circle_size*1.0/1e3;
	
	// circle spcific information
	double circle_radius_in_pixel = 
	circle_radius_in_pixel_per_image_line*image_matrix.rows;
	
	int circle_thickness_in_pixel = (int)ceil(
	circle_thickness_in_pixel_per_image_line*image_matrix.rows
	);
	
	int circle_lineType	=8;		
	
	// star position

	cv::Point circle_position_openCV_bug_x_y_switched;
	circle_position_openCV_bug_x_y_switched.x = 
	circle_position.y;
	circle_position_openCV_bug_x_y_switched.y = 
	circle_position.x;
	
	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"circle_overlay() -> ";
		std::cout<<"position ("<<circle_position.x<<"|";
		std::cout<<circle_position.y<<"), ";
		std::cout<<"radius "<<circle_radius_in_pixel<<"px, ";
		std::cout<<"thickness "<<circle_thickness_in_pixel<<"px";
		std::cout<<std::endl;
	}
	
	//star circle
	cv::circle( 
	image_matrix,
	circle_position_openCV_bug_x_y_switched,
	circle_radius_in_pixel,
	cv::Scalar( 0, 0, 255),
	circle_thickness_in_pixel,
	circle_lineType 
	);	

	if(verbosity){
		std::cout<<"simple_image -> ";
		std::cout<<"circle_overlay() -> ";
		std::cout<<"end";
		std::cout<<std::endl;
	}		
}*/
//======================================================================
