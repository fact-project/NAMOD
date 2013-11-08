#include "list_of_pixel_positions.h"
//======================================================================
list_of_pixel_positions::list_of_pixel_positions(){
	verbosity = false;
	list_of_points.clear();
}
//======================================================================
cv::Point list_of_pixel_positions::get_mean_of_pixel_distribution(){
	
	if(verbosity){
		std::cout<<"list_of_pixel_positions -> ";
		std::cout<<"get_mean_of_pixel_distribution()";
		std::cout<<" calculate mean out of "<<size()<<"pixels";
		std::cout<<std::endl;
	}
	
	cv::Point mean;
	for(
	int point_itterator=0;
	point_itterator<list_of_points.size();
	point_itterator++)
	{
		mean = mean + list_of_points.at(point_itterator);
	}
	mean = mean*(1.0/list_of_points.size());
	
	if(verbosity){
		std::cout<<"list_of_pixel_positions -> ";
		std::cout<<"get_mean_of_pixel_distribution()";
		std::cout<<"-> mean("<<mean.x<<"|"<<mean.y<<")";
		std::cout<<std::endl;
	}	
	return mean;
}
//======================================================================
int list_of_pixel_positions::size(){
	return list_of_points.size();
}
//======================================================================
void list_of_pixel_positions::push_back(cv::Point point_to_push_back){
	list_of_points.push_back(point_to_push_back);
}
//======================================================================
cv::Point list_of_pixel_positions::at(int itterator){
	return list_of_points.at(itterator);
} 
//======================================================================
void list_of_pixel_positions::clear(){
	list_of_points.clear();
}
//======================================================================
void list_of_pixel_positions::operator=(list_of_pixel_positions eq){
	if(verbosity){
		std::cout<<"simple_image -> operator=() ->"<<std::endl;
	}	
	list_of_points = eq.list_of_points;
	verbosity = eq.verbosity;
}
//======================================================================
void list_of_pixel_positions::disp(){	
	std::cout<<"list_of_pixel_positions-> ";
	for(int point_itterator=0;
	point_itterator<list_of_points.size();
	point_itterator++){
		
		std::cout<<"("<<list_of_points.at(point_itterator).x;
		std::cout<<"|"<<list_of_points.at(point_itterator).y<<"), ";
	}
	std::cout<<std::endl;
}
//======================================================================
