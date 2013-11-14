//=================================
// include guard
#ifndef __USER_INTERACTION_H_INCLUDED__
#define __USER_INTERACTION_H_INCLUDED__

//=================================
// forward declared dependencies


//=================================
// included dependencies
#include "programm_verbosity.h"

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <vector>
#include <dirent.h>
#include <list>
#include <algorithm>

//=================================
class user_interaction 			:public programm_verbosity{
public:
	std::string			menu_name;
	std::string 			status_prompt;
	std::vector<std::string> 	list_of_keys;
	std::vector<std::string> 	list_of_descriptions;
	std::string			valid_user_input;
	int 			command_line_columns;
//======================================================================
user_interaction();
//======================================================================
void add_control(std::string new_key,std::string new_description);
//======================================================================
std::string get_control_options_prompt();
//======================================================================
void update_status_prompt(std::string new_status_prompt);
//======================================================================
void get_user_input();
//======================================================================
bool compare_user_input_and_keys(std::string user_input);
//======================================================================
std::string get_list_of_options();
//======================================================================
int ask_user_for_integer
(std::string description,int lower_limit,int upper_limit);
//======================================================================
int ask_user_for_integer
(std::string description,std::vector<int> *pointer_to_list_of_valid_integer);
//======================================================================
bool isInteger(const std::string & user_input);
//======================================================================
bool integer_is_part_of_list
(int candidate,std::vector<int> *pointer_to_list_of_valid_integer);
//======================================================================
std::string get_alphanumeric_string_by_user();
//======================================================================
std::string get_menu_header();
//======================================================================
std::string get_line_of_specific_character
(std::string string_to_repeat,int number_of_repeatations);
//======================================================================
double ask_user_for_non_zero_float
(std::string description,double lower_limit,double upper_limit);
//======================================================================
std::vector <std::string> get_list_of_files_in_directory( const std::string& path );
//======================================================================
std::string make_nice_line_with_dots(std::string left_text,std::string right_text);
//======================================================================
void export_text(std::string filename,std::string text);
};
#endif // __USER_INTERACTION_H_INCLUDED__ 
