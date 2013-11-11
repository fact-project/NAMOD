// basic include
#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <math.h>
#include <vector>
#include <list>
#include <algorithm>
// time
#include <time.h>

// programm includes
//#include "global_variables.h"
#include "programm_verbosity.h"
#include "user_interaction.h"
#include "time_stamp_manager.h"
#include "list_of_pixel_positions.h"
#include "pointing_direction.h"
#include "star_in_image.h"
#include "intrinsic_camera_parameter.h"
#include "simple_image.h"
#include "sccan_image.h"
#include "ueye_camera.h"
#include "sccan_analysis_point.h"
#include "sccan_point_pair.h"
#include "mirror.h"
#include "reflector.h"
#include "snapshot.h"
#include "sccan_point_pair_handler.h"
#include "quick_align.h"
#include "sccan_point_analysis.h"
#include "verbosity_handler.h"
#include "main_menu.h"
// plotting
#include <mgl2/mgl.h>


//======================================================================
int main(){
	int system_call_return_value;
	system_call_return_value = system("clear"); 
	system_call_return_value = system("clear"); 
	
	//testing plotting environment
	mglData dat(30,40);
	// data to for plotting
	for(long i=0;i<30;i++)
	for(long j=0;j<40;j++)
	dat.a[i+30*j] = 1/(1+(i-15)*(i-15)/225.+(j-20)*(j-20)/400.);
	mglGraph gr;			// class for plot drawing
	gr.Rotate(50,60);		// rotate axis
	gr.Light(true);			// enable lighting
	gr.Surf(dat);			// plot surface
	gr.Cont(dat,"y");		// plot yellow contour lines
	gr.Axis();				// draw axis
	gr.WriteFrame("sample.eps");	// save it

	// python for plotting
	//Py_Initialize();
	
	//==================================================================
	// star camera
	//==================================================================	
		
		//==============================================================
		// intrinsic paramters for star camera
		//==============================================================
	intrinsic_camera_parameter parameters_for_star_camera;
	
	parameters_for_star_camera.set_names(
	"ueye 5MPx CMOS",
	"Carl Zeiss Flektogon F2.4 / f35mm");

	parameters_for_star_camera.set_FoV_to_pixel_mapping(3.34375E-3);
	
	parameters_for_star_camera.
	set_coefficients_for_radiometric_correction_plane(
	-1.2185,
	1.2021,
	0.99303
	);	
	
	ueye_camera star_camera(13,parameters_for_star_camera);
	//==================================================================
	// reflector camera
	//==================================================================	
		
		//==============================================================
		// intrinsic paramters for reflector camera
		//==============================================================
	intrinsic_camera_parameter parameters_for_reflector_camera;
	
	parameters_for_reflector_camera.set_names(
	"Thor Labs 1.3MPx CCD",
	"M12 the imageing source F2.0 / f4mm"
	);
	
	parameters_for_reflector_camera.
	set_coefficients_for_radiometric_correction_plane(
	-1.1527,
	1.0283,	
	-0.18637
	);
	
	ueye_camera reflector_camera(42,parameters_for_reflector_camera);


	star_camera.display_camera_information();
	reflector_camera.display_camera_information();
	//==================================================================
	// hanldes
	//==================================================================	
	
	sccan_point_pair_handler sccan_handle;
	sccan_handle.set_cameras(&star_camera,&reflector_camera);

	//sccan_handle.acquire_sccan_points(5);
	snapshot snap;
	snap.add_camera(&star_camera);
	snap.add_camera(&reflector_camera);

	reflector reflector_instance;
	quick_align quick(&reflector_instance,&sccan_handle);
	

	sccan_point_analysis analysis(
	&sccan_handle,&reflector_instance,&star_camera);
	
	verbosity_handler verbosity_interaction(
	&global_time_stamp_manager_instance,
	&star_camera,
	&reflector_camera,
	&reflector_instance,
	&snap,
	&sccan_handle,
	&quick,
	&analysis
	);
	
	main_menu menu(
	&snap,
	&reflector_instance,
	&sccan_handle,
	&quick,
	&analysis,
	&verbosity_interaction,
	&star_camera,
	&reflector_camera);
	
	menu.interaction();	
	
	return 0;
}
	
