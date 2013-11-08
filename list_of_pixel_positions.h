//=================================
// include guard
#ifndef __LIST_OF_POIXEL_POSITIONS_H_INCLUDED__
#define __LIST_OF_POIXEL_POSITIONS_H_INCLUDED__

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>

#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

#include "programm_verbosity.h"

//=================================
class list_of_pixel_positions			:public programm_verbosity{
	std::vector<cv::Point> list_of_points;
public:
//======================================================================
list_of_pixel_positions();
//======================================================================
cv::Point get_mean_of_pixel_distribution();
//======================================================================
int size();
//======================================================================
void push_back(cv::Point point_to_push_back);
//======================================================================
cv::Point at(int itterator);
//======================================================================
void clear();
//======================================================================
void operator=(list_of_pixel_positions eq);
//======================================================================
void disp();
//======================================================================
};
#endif // __LIST_OF_POIXEL_POSITIONS_H_INCLUDED__ 
