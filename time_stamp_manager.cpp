#include "time_stamp_manager.h"
//======================================================================
time_stamp_manager::time_stamp_manager(){
		time(&startup_time);
}
//======================================================================
std::string time_stamp_manager::get_startup_time_detail()const{
	return ctime(&startup_time);
}
//======================================================================
std::string time_stamp_manager::get_startup_time_stamp()const{
	return get_time_stamp(startup_time);
}
//======================================================================
std::string time_stamp_manager::get_current_time_stamp()const{
	time_t  current_time;
	time(&current_time);
	return get_time_stamp(current_time);
}
//======================================================================
std::string time_stamp_manager::get_time_stamp(time_t time_to_stamp)const{
	std::stringstream sstr_time;
	// create propper startup time string startup_time
	struct tm *ptm;
	ptm = gmtime(&time_to_stamp);
	//http://www.cplusplus.com/reference/ctime/tm/
	int year = ptm->tm_year + 1900;//beginning in year 1900!
	int month= ptm->tm_mon	+ 1;   //for what ever reason +1
	int mday = ptm->tm_mday;
	int hour = ptm->tm_hour;
	int min  = ptm->tm_min;
	//year
	sstr_time<<year;
	//month
	if(month<10){sstr_time<<"0"<<month;}else{sstr_time<<month;}
	//day of month
	if(mday<10){sstr_time<<"0"<<mday;}else{sstr_time<<mday;}
	//spacer
	sstr_time<<"_";
	//hour
	if(hour<10){sstr_time<<"0"<<hour;}else{sstr_time<<hour;}
	//minutes
	if(min<10){sstr_time<<"0"<<min;}else{sstr_time<<min;}	
	
	if(verbosity){
		std::cout<<"time_stamp_manager->get_time_stamp()->";
		std::cout<<sstr_time.str()<<std::endl;	
	}
	
	return sstr_time.str();
}
//======================================================================
double time_stamp_manager::get_number_of_seconds_since_startup()const{
	
	time_t  current_time;
	time(&current_time);
	
	double time_since_startup_in_seconds =
	difftime(current_time,startup_time);	
	
	if(verbosity){
		std::cout<<"time_stamp_manager -> ";
		std::cout<<"get_number_of_seconds_since_startup()";
		std::cout<<" -> "<<time_since_startup_in_seconds<<"s ";
		std::cout<<"since startup.";
		std::cout<<std::endl;
	}
	return difftime(current_time,startup_time);	
}
//======================================================================
