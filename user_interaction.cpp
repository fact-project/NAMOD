#include "user_interaction.h"
//======================================================================
user_interaction::user_interaction(){
	//verbosity = false;
	command_line_columns = 72;
	menu_name = "";
}
//======================================================================
void user_interaction::add_control(std::string new_key,
std::string new_description){
	if(compare_user_input_and_keys(new_key)==false){
		
		list_of_keys.push_back(new_key);
		list_of_descriptions.push_back(new_description);
		
		if(verbosity){
			std::cout<<"user_interaction -> add_control() -> Key >";
			std::cout<<new_key<<"< was added to control key list\n";
		}
	}else{
		std::cout<<"user_interaction -> add_control() -> Key >";
		std::cout<<new_key<<"< does already exist in control key list!"<<std::endl;
	}
}
//======================================================================
std::string user_interaction::get_control_options_prompt(){
	std::stringstream out;
	out<<"| "<<std::endl;
	
	for(uint key_itterator = 0;
	key_itterator<list_of_keys.size();
	key_itterator++){
	out<<"| ["<<list_of_keys.at(key_itterator)<<"]";
	out<<get_line_of_specific_character(
	".",
	command_line_columns-
	list_of_keys.at(key_itterator).size()-
	list_of_descriptions.at(key_itterator).size()-
	4
	);

	out<<list_of_descriptions.at(key_itterator)<<std::endl;
	}
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
void user_interaction::
update_status_prompt(std::string new_status_prompt){
	status_prompt = new_status_prompt;
	status_prompt += get_control_options_prompt();
}
//======================================================================
void user_interaction::get_user_input(){
	//==============================================================
	// get input of user via command line
	//==============================================================
	std::stringstream out;
	std::string user_input;
	
	int unused_return_value = system("clear");
	std::cout<<get_menu_header();
	std::cout<<status_prompt<<"->";

	std::cin>>user_input;
	while( !compare_user_input_and_keys(user_input) )
	{
		int sys_return = system("clear");
		std::cout<<get_menu_header();
		std::cout<<status_prompt;
		
		out.str("");
		out<<"Choose between ";
		out<<get_list_of_options()<<std::endl;
		out<<"Your input ["<<user_input<<"] is not valid!"<<std::endl;
		std::cout<<out.str()<<"->";
		std::cin>>user_input;		
	}
	valid_user_input = user_input;
}
//======================================================================
bool user_interaction::
compare_user_input_and_keys(std::string user_input){
	for(int key_itterator = 0;
	key_itterator<list_of_keys.size();
	key_itterator++){
		if(list_of_keys.at(key_itterator).compare(user_input)==0){
			return true;
		}
	}	
	return false;
}
//======================================================================
std::string user_interaction::get_list_of_options(){
	std::stringstream out;
	for(int key_itterator = 0;
	key_itterator<list_of_keys.size();
	key_itterator++){
		if(key_itterator+1 == list_of_keys.size()){
			out<<"["<<list_of_keys.at(key_itterator)<<"]";
		}else if((key_itterator+2) == list_of_keys.size()){
			out<<"["<<list_of_keys.at(key_itterator)<<"] or ";			
		}else{
			out<<"["<<list_of_keys.at(key_itterator)<<"], ";	
		}
	}	
	return out.str();
}
//======================================================================
int user_interaction::ask_user_for_integer
(std::string description,int lower_limit,int upper_limit){
	if(lower_limit>=upper_limit){
		std::cout<<"user_interaction -> ask_user_for_integer() -> ";
		std::cout<<"lower:"<<lower_limit<<" and upper:"<<upper_limit;
		std::cout<<"limit are messed up!"<<std::endl;
	}
	//==============================================================
	// get integer input of user via command line
	//==============================================================
	std::stringstream out;
	std::stringstream last_warning;
	std::string user_input;
	
	bool flag_valid_input;
	do{
		flag_valid_input = true;
		
		int sys_return = system("clear");
		std::cout<<get_menu_header();
		std::cout<<status_prompt;
		std::cout<<last_warning.str();
		std::cout<<description<<": "<<std::endl;
		std::cout<<"->";
		
		std::cin>>user_input;
		
		last_warning.str("");
		if(!isInteger(user_input))
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is not an integer!"<<std::endl;
		}
		if(atoi(user_input.c_str())<lower_limit)
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is below the lower limit: ";
			last_warning<<lower_limit<<std::endl;
		}
		if(atoi(user_input.c_str())>upper_limit)
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is above the upper limit: ";
			last_warning<<upper_limit<<std::endl;
		}
		
	}while(!flag_valid_input);

	return atoi(user_input.c_str());
}
//======================================================================
int user_interaction::
ask_user_for_integer(
std::string description,
std::vector<int> *pointer_to_list_of_valid_integer){
	if(pointer_to_list_of_valid_integer->size()==0){
		std::cout<<"user_interaction -> ask_user_for_integer() -> ";
		std::cout<<"the list of valid integers is empty!"<<std::endl;
	}
	//==============================================================
	// get integer input of user via command line
	//==============================================================
	std::stringstream out;
	std::stringstream last_warning;
	std::string user_input;
	
	bool flag_valid_input;
	do{
		flag_valid_input = true;
		
		int sys_return = system("clear");
		std::cout<<get_menu_header();
		std::cout<<status_prompt;
		std::cout<<last_warning.str();
		std::cout<<description<<": "<<std::endl;
		std::cout<<"->";
		
		std::cin>>user_input;
		
		last_warning.str("");
		if(!isInteger(user_input))
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is not an integer!"<<std::endl;
		}
		if
		(!	integer_is_part_of_list( 
			atoi(user_input.c_str()),
			pointer_to_list_of_valid_integer)
		)
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is not part ";
			last_warning<<"of the valid input list:"<<std::endl;
			for(int integer_itterator = 0;
			integer_itterator<pointer_to_list_of_valid_integer->size();
			integer_itterator++)
			{
				last_warning<<"["<<
				pointer_to_list_of_valid_integer->at(integer_itterator)
				<<"], ";
			}
			std::cout<<std::endl;
		}
		
	}while(!flag_valid_input);

	return atoi(user_input.c_str());
}
//======================================================================
bool user_interaction::isInteger(const std::string & user_input){
	if
	(
	user_input.empty() || 
	((!isdigit(user_input[0])) && 
	(user_input[0] != '-') && 
	(user_input[0] != '+'))
	){ 
		if(verbosity){
			std::cout<<"user_interaction -> isInteger() -> ";
			std::cout<<user_input<<" is no integer."<<std::endl;
		}	
		return false ;
	}
	char * first_character_which_is_not_part_of_a_number ;
	long unused_return_value_of_strtol = strtol(
	user_input.c_str(), 
	&first_character_which_is_not_part_of_a_number, 
	10) ;

	if(*first_character_which_is_not_part_of_a_number == 0){
		if(verbosity){
			std::cout<<"user_interaction -> isInteger() -> ";
			std::cout<<user_input<<" is an integer."<<std::endl;
		}
	}else{
		if(verbosity){
			std::cout<<"user_interaction -> isInteger() -> ";
			std::cout<<user_input<<" is no integer."<<std::endl;
		}	
	}
	
	return (*first_character_which_is_not_part_of_a_number == 0) ;
}
//======================================================================
bool user_interaction::integer_is_part_of_list
(int candidate,std::vector<int> *pointer_to_list_of_valid_integer){

	for(int integer_itterator = 0;
	integer_itterator<pointer_to_list_of_valid_integer->size();
	integer_itterator++)
	{
		if(pointer_to_list_of_valid_integer->
		at(integer_itterator)==candidate){
			return true;
		}
	}
	return false;
}
//======================================================================
std::string user_interaction::get_alphanumeric_string_by_user(){
	// no points or whitespaces!
	std::string user_input;
	std::stringstream last_warning;	
	
	bool user_input_is_valid;
	do{
		user_input_is_valid = true;
		int sys_return =  system("clear");
		std::cout<<get_menu_header();
		std::cout<<status_prompt;
		std::cout<<"Enter a string (no points or whitespaces!)";
		std::cout<<std::endl;
		std::cout<<last_warning.str();		
		last_warning.str("");
		std::cout<<"->";

		std::cin>>user_input;
		
		for(
		int character_itterator=0;
		character_itterator<user_input.size();
		character_itterator++)
		{
			if(ispunct(user_input[character_itterator]) &&
			user_input[character_itterator] != '_' ){
				last_warning<<"Your input [";
				last_warning<<user_input[character_itterator];
				last_warning<<"] at position "<<character_itterator+1;
				last_warning<<" in ["<<user_input<<"]";
				last_warning<<" is a punctation!"<<std::endl;
				user_input_is_valid=false;
			}
			if(isspace(user_input[character_itterator])){
				last_warning<<"Your input [";
				last_warning<<user_input[character_itterator];
				last_warning<<"] at position "<<character_itterator+1;
				last_warning<<" in ["<<user_input<<"]";
				last_warning<<" is a whitespace!"<<std::endl;
				user_input_is_valid=false;
			}
		}
	}while(!user_input_is_valid);
	
	return user_input;
}
//======================================================================
std::string user_interaction::get_string_by_user(){
	std::string user_input;
	
		int sys_return =  system("clear");
		std::cout<<get_menu_header();
		std::cout<<status_prompt;
		std::cout<<"Enter a string ";
		std::cout<<std::endl;	
		std::cout<<"->";

		std::cin>>user_input;
	
	return user_input;
}
//======================================================================
std::string user_interaction::get_menu_header(){
	std::stringstream out;
	out<<"O";
	out<<get_line_of_specific_character("=",(command_line_columns-2));
	out<<"O"<<std::endl;
	out<<"| "<<menu_name;
	out<<get_line_of_specific_character
	(" ",command_line_columns-menu_name.size()-3);
	out<<"|"<<std::endl;
	out<<"O";
	out<<get_line_of_specific_character("=",(command_line_columns-2));
	out<<"O"<<std::endl;
return out.str();
}
//======================================================================
std::string user_interaction::
get_line_of_specific_character(
std::string string_to_repeat,int number_of_repeatations){
	std::stringstream out;
	for(
	int string_itterator=0;
	string_itterator<number_of_repeatations;
	string_itterator++
	)
	{
		out<<string_to_repeat;
	}
	return out.str();
}
//======================================================================
double user_interaction::
ask_user_for_non_zero_float(
std::string description,double lower_limit,double upper_limit){
	
	if(lower_limit>=upper_limit){
		std::cout<<"user_interaction -> ask_user_for_float() -> ";
		std::cout<<"lower:"<<lower_limit<<" and upper:"<<upper_limit;
		std::cout<<"limit are messed up!"<<std::endl;
	}
	if(lower_limit<=0.0){
		std::cout<<"user_interaction -> ask_user_for_float() -> ";
		std::cout<<"lower:"<<lower_limit;
		std::cout<<"limit is <= 0.0 and 0.0 is not valid!"<<std::endl;		
	}
	
	std::stringstream out;
	std::stringstream last_warning;
	std::string user_input;
	bool flag_valid_input;
	do{
		flag_valid_input = true;
		
		int sys_return =  system("clear");
		std::cout<<get_menu_header();
		std::cout<<status_prompt;
		std::cout<<last_warning.str();
		std::cout<<description<<": "<<std::endl;
		std::cout<<"->";
		
		std::cin>>user_input;
		
		last_warning.str("");
		if(atof(user_input.c_str())==0.0)
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is either not a float or 0.0";
			last_warning<<" and 0.0 is not valid!"<<std::endl;
		}
		if(atof(user_input.c_str())<lower_limit)
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is below the lower limit: ";
			last_warning<<lower_limit<<std::endl;
		}
		if(atof(user_input.c_str())>upper_limit)
		{
			flag_valid_input =false;
			last_warning<<"Your input ["<<user_input;
			last_warning<<"] is above the upper limit: ";
			last_warning<<upper_limit<<std::endl;
		}
	}while(!flag_valid_input);
	
	return atof(user_input.c_str());	
}
//======================================================================
std::vector <std::string> user_interaction::
get_list_of_files_in_directory
( const std::string& path = std::string() ){
	
	if(verbosity){
		std::cout<<"user_interaction ->";
		std::cout<<" get_list_of_files_in_directory()"<<std::endl;
	}
	std::vector <std::string> list_of_all_files_in_directory;
	dirent* entry_in_directory_to_look_at;
	DIR* directory_to_look_at;
	int errornumber = 0;
	directory_to_look_at = opendir( path.empty() ? "." : path.c_str() );
	if (directory_to_look_at)
	{
		while (true)
		{
			errornumber = 0;
			entry_in_directory_to_look_at = 
			readdir( directory_to_look_at );
			if (entry_in_directory_to_look_at == NULL) break;
			list_of_all_files_in_directory.push_back
			(std::string( entry_in_directory_to_look_at->d_name ) );
		}
		closedir( directory_to_look_at );
		sort( 
		list_of_all_files_in_directory.begin(),
		list_of_all_files_in_directory.end() 
		);
	}
	return list_of_all_files_in_directory;
}
//======================================================================
std::string user_interaction::
make_nice_line_with_dots(std::string left_text,std::string right_text){
	
	std::stringstream out;
	out<<left_text;
	
	int number_of_dots;
	if(command_line_columns-left_text.size()-right_text.size()>0){
		
		number_of_dots = 
		command_line_columns-
		left_text.size()-
		right_text.size();
		
	}else{
		number_of_dots = 1;
	}
	out<<get_line_of_specific_character(".",number_of_dots);
	out<<right_text;
	out<<std::endl;
	
	return out.str();
}
//======================================================================
void user_interaction::
export_text(std::string filename,std::string text){

	if(verbosity){
		std::cout << "user_interaction -> export_text() -> ";
		std::cout << "creating file:  >"<<filename<<"<\n";
	}
	
	std::ofstream myfile (filename.c_str());
	if (myfile.is_open()){
		
		myfile << text;
		myfile.close();
		
		std::cout << "user_interaction -> export_text() -> ";
		std::cout << "created file >"<<filename<<"<\n";
	
	}else{
		std::cout << "user_interaction -> export_text() -> ";
		std::cout << "unable to create file >"<<filename<<"<\n";
	}
}
//======================================================================
