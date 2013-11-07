//=================================
// include guard
#ifndef __PROGRAMM_VERBOSITY_H_INCLUDED__
#define __PROGRAMM_VERBOSITY_H_INCLUDED__

//=================================
// forward declared dependencies

//=================================
// included dependencies

//=================================
class programm_verbosity{
public:
	bool verbosity;
//======================================================================
programm_verbosity();
//======================================================================
virtual void toggle_verbosity();
//======================================================================
virtual void toggle_verbosity(bool new_verbosity);
//======================================================================
bool verbosity_status();
//======================================================================
};

#endif // __PROGRAMM_VERBOSITY_H_INCLUDED__ 
