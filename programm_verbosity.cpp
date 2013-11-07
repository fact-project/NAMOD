#include "programm_verbosity.h"
//======================================================================
programm_verbosity::programm_verbosity(){
	verbosity = false;
}
//======================================================================
void programm_verbosity::toggle_verbosity(){
	//~ if(verbosity){
		//~ cout<<"programm_verbosity -> toggle_verbosity() -> ";
		//~ cout<<"verbosity is set to FALSE."<<endl;
	//~ }
	verbosity = !verbosity;
}
//======================================================================
void programm_verbosity::toggle_verbosity(bool new_verbosity){
	//~ if(new_verbosity){
		//~ cout<<"programm_verbosity -> toggle_verbosity() -> ";
		//~ cout<<"verbosity is set to TRUE"<<endl;
	//~ }
	verbosity = new_verbosity;
}
//======================================================================
bool programm_verbosity::verbosity_status(){
	//~ if(verbosity){
		//~ cout<<"programm_verbosity -> verbosity_status() -> TRUE"<<endl;
	//~ }
	return verbosity;
}
//======================================================================
