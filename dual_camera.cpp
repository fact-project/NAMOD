// minimal ueye example
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

// include for configuration file
#include <iomanip>
#include <cstdlib>
#include <libconfig.h++>

// look up directorys
#include <dirent.h>

// open CV
#include <cv.h>
#include <highgui.h>
#include <opencv2/opencv.hpp>

//======================================================================
// UEYE camera by IDS imaging
// http://www.ids-imaging.de
// Manual:
// http://www.ids-imaging.de/frontend/files/uEyeManuals/Manual_ger/uEye_Handbuch/index.html
#include <ueye.h>
//======================================================================

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

// matplotpp
//#include "matplotpp.h"
#include "Python.h"

using namespace libconfig;
// pre decleration
class mirror;

static void onMouse2(int event, int x, int y, int flags, void *param);

//======================================================================
class ueye_camera 				:public programm_verbosity{
public:
	sccan_image latest_image;
	HIDS 		ueye_camera_handle;
	CAMINFO 	ueye_camera_info;
	SENSORINFO 	ueye_sensor_info;
	int 		ueye_camera_id;
	int 		ueye_camera_sensor_number_of_pixels_in_width;
	int 		ueye_camera_sensor_number_of_pixels_in_hight;
	int 		ueye_color_mode;
	int 		ueye_number_of_coulor_channels;
	int 		ueye_bits_per_coulor_channel;
	int			ueye_bits_per_pixel;
	double 		ueye_exposure_time_in_ms; 
	uint 		ueye_pixel_clock_min_in_MHz;
	uint 		ueye_pixel_clock_max_in_MHz;
	uint 		ueye_pixel_clock_increment_in_MHz;
	uint 		ueye_current_pixel_clock_in_MHz;
	uint		ueye_default_pixel_clock_in_MHz;
	double 		ueye_current_framerate_in_fps;
	double 		ueye_default_framerate_in_fps;
	bool 		initialization_succesfull;
	bool 		flag_long_time_exposure;
	std::stringstream out;
	
	//intrinsic_camera_parameter
	intrinsic_camera_parameter intrinsic;
//======================================================================
ueye_camera(int camera_ID_to_initialize,
intrinsic_camera_parameter new_intrinsic){
	verbosity = true;
	intrinsic.toggle_verbosity(verbosity);
	latest_image.toggle_verbosity(verbosity);
	
	initialization_succesfull = false;
	flag_long_time_exposure = false;
	set_camera_ID(camera_ID_to_initialize);
	
	intrinsic = new_intrinsic;
	
	if(initialize()==true){
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" initialized using constructor";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to initialize using constructor";
		out<<std::endl; std::cout<<out.str();
	};	
	
	// camera has to be initialized to set intrinsic_parameters 
	// -> sensor size
	latest_image.set_camera_ID(camera_ID_to_initialize);
	latest_image.set_intrinsic_parameters(intrinsic);
}
//======================================================================
void set_camera_ID(int camera_ID_to_initialize){
	ueye_camera_handle = (HIDS)camera_ID_to_initialize;
	ueye_camera_id = camera_ID_to_initialize;	
}
//======================================================================
uint get_camera_ID(){
	return ueye_camera_id;
}
//======================================================================
~ueye_camera(){
	//================================
	// Exit Camera
	//================================
	if(is_ExitCamera(ueye_camera_handle) == IS_SUCCESS)
	{
	  	out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" exit was succesfull.";
		out<<std::endl; std::cout<<out.str();
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to exit camera!";
		out<<std::endl; std::cout<<out.str();
	}	
}
//======================================================================
bool initialize(){
	//==================================================================
	// initialize ueye camera
	//==================================================================
	// Null, 0 makes is_InitCamera() to choose the "first" camera 
	// avaliable
	if(is_InitCamera(&ueye_camera_handle, NULL) == IS_SUCCESS){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" has been initialized.";
		std::cout<<out.str()<<std::endl;
		initialization_succesfull = true;
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" has not been initialized.";
		std::cout<<out.str()<<std::endl;
		initialization_succesfull = false;
		return false;
	}
	//==================================================================
	// Camera Info
	//==================================================================
	//CAMINFO ueye_camera_info;
	
	if(is_GetCameraInfo(ueye_camera_handle,&ueye_camera_info)
	 == IS_SUCCESS){
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got camera info.";
			std::cout<<out.str()<<std::endl;
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get camera info.";
		std::cout<<out.str()<<std::endl;
		return false;
	}

	//==================================================================
	// get sensor info
	//==================================================================
	//SENSORINFO ueye_sensor_info;
	
	if(is_GetSensorInfo(ueye_camera_handle,&ueye_sensor_info ) 
	== IS_SUCCESS){
		ueye_camera_sensor_number_of_pixels_in_width =
		 ueye_sensor_info.nMaxWidth;
		ueye_camera_sensor_number_of_pixels_in_hight =
		 ueye_sensor_info.nMaxHeight;
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got sensor info.";
			std::cout<<out.str()<<std::endl;
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get sensor info.";
		std::cout<<out.str()<<std::endl;
		return false;
	}

	intrinsic.set_camera_sensor_size(
	ueye_camera_sensor_number_of_pixels_in_width,
	ueye_camera_sensor_number_of_pixels_in_hight);
	
	//==================================================================
	// set color mode
	//==================================================================
	//int ColorMode;
	ueye_number_of_coulor_channels = 3;
	ueye_bits_per_coulor_channel = 8;
	ueye_bits_per_pixel = 
	ueye_number_of_coulor_channels*ueye_bits_per_coulor_channel;
	// ueye_bits_per_pixel = 24;//3 x 8Bit
	// The return value of is_GetColorDepth() does not fit IS_SUCCESS.
	// I do not know why...
	// Anyway everything works fine.
	// Therefor the return value is ignored.
	
	int ret_GetColorDept = 
	is_GetColorDepth
	(ueye_camera_handle,&ueye_bits_per_pixel,&ueye_color_mode);
						
	if(			ret_GetColorDept == IS_SUCCESS){
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" GetColorDept was successfull.";
			out<<std::endl; std::cout<<out.str();
		}
	}else if(	ret_GetColorDept == IS_NO_SUCCESS){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" GetColorDept was not successfull.";
		out<<std::endl; std::cout<<out.str();
	}else if( 	ret_GetColorDept == IS_INVALID_PARAMETER){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" GetColorDept has invalid parameter.";
		out<<std::endl; std::cout<<out.str();
	}else if(	ret_GetColorDept == IS_INVALID_CAMERA_HANDLE){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" GetColorDept has invalid camera handle.";
		out<<std::endl; std::cout<<out.str();
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" GetColorDept retuned int value: ";
		out<<ret_GetColorDept;
		out<<std::endl; std::cout<<out.str();
	}
				
	if(is_SetColorMode( ueye_camera_handle,IS_CM_BGR8_PACKED)
	== IS_SUCCESS){
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" color mode is set.";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" Setting color mode failed.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}
	//==================================================================
	// get pixel clock range
	//==================================================================

	UINT pixel_clock_range_array_in_MHz[3] = {0, 0, 0};

	int return_of_get_pixel_clock = 
	is_PixelClock(	ueye_camera_handle, 
					IS_PIXELCLOCK_CMD_GET_RANGE, 
					(void*)pixel_clock_range_array_in_MHz, 
					sizeof(pixel_clock_range_array_in_MHz));

	if (return_of_get_pixel_clock == IS_SUCCESS)
	{
	  ueye_pixel_clock_min_in_MHz = pixel_clock_range_array_in_MHz[0];
	  ueye_pixel_clock_max_in_MHz = pixel_clock_range_array_in_MHz[1];
	  ueye_pixel_clock_increment_in_MHz = 
	  pixel_clock_range_array_in_MHz[2];
	  	if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got pixel clock range.";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get pixel clock range.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}
	
	//==================================================================
	// get current pixel clock
	//==================================================================
	
	int return_of_PixelClock =
	is_PixelClock(	ueye_camera_handle,
					IS_PIXELCLOCK_CMD_GET,
					(void*)&ueye_current_pixel_clock_in_MHz,
					sizeof(ueye_current_pixel_clock_in_MHz));
	
	if (return_of_PixelClock == IS_SUCCESS)
	{
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got current pixel clock.";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get current pixel clock.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}
	//==================================================================
	// get default pixel clock
	//==================================================================
	
	return_of_PixelClock =
	is_PixelClock(	ueye_camera_handle,
					IS_PIXELCLOCK_CMD_GET_DEFAULT,
					(void*)&ueye_default_pixel_clock_in_MHz,
					sizeof(ueye_default_pixel_clock_in_MHz));
	
	if (return_of_PixelClock == IS_SUCCESS)
	{
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got default pixel clock.";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get default pixel clock.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}
	//==================================================================
	// get current framerate
	//==================================================================
	int return_of_SetFrameRate =
	is_SetFrameRate(ueye_camera_handle,
					IS_GET_DEFAULT_FRAMERATE,
					&ueye_current_framerate_in_fps);
	
	if (return_of_PixelClock == IS_SUCCESS)
	{
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got current framerate: ";
			out<<ueye_current_framerate_in_fps<<"fps";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get current framerate.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}	
	//==================================================================
	// get default framerate
	//==================================================================
	return_of_SetFrameRate =
	is_SetFrameRate(ueye_camera_handle,
					IS_GET_DEFAULT_FRAMERATE,
					&ueye_default_framerate_in_fps);
	
	if (return_of_PixelClock == IS_SUCCESS)
	{
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" got default framerate: ";
			out<<ueye_default_framerate_in_fps<<"fps";
			out<<std::endl; std::cout<<out.str();
		}
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get default framerate.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}	
	//==================================================================
	// set initialization_succesfull
	//==================================================================	
	initialization_succesfull = true;
	
	// allocate memory for OpenCV sccan_image
	latest_image.initialize_image_matrix(
	ueye_camera_sensor_number_of_pixels_in_hight,
	ueye_camera_sensor_number_of_pixels_in_width);

}
//======================================================================
bool acquire_image(double *pointer_to_desired_exposure_time_in_ms){

	if(initialization_succesfull){
		//=================================
		// allocate memory for sccan_image
		//=================================
		char 	*pointer_to_image_memory;	// pointer to Image memory
		int  	ueye_memory_id; 			// memory ID

		if(is_AllocImageMem(ueye_camera_handle,
						ueye_camera_sensor_number_of_pixels_in_width,
						ueye_camera_sensor_number_of_pixels_in_hight,
						ueye_bits_per_pixel,
						&pointer_to_image_memory,
						&ueye_memory_id)
		== IS_SUCCESS){
			if(verbosity){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" allocated sccan_image memory";
				out<<std::endl; std::cout<<out.str();
			}
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" failed to allocate sccan_image memory!";
			out<<std::endl; std::cout<<out.str();
			return false;
		}
		
		//=================================
		// set sccan_image memory 
		//=================================
		if(is_SetImageMem(	ueye_camera_handle,
							pointer_to_image_memory,
							ueye_memory_id)
		==IS_SUCCESS){
			if(verbosity){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" set sccan_image memory active";
				out<<std::endl; std::cout<<out.str();
			}
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" failed to set sccan_image memory active!";
			out<<std::endl; std::cout<<out.str();
			return false;
		};
		
		//=================================
		// set trigger mode to software mode
		//=================================
		// the ueye cams have build in hardware triggers.
		// We do not want to use them for our FACT approach because 
		// the PC running Capture is used to activate the trigger.
		// Therefor we use the so called software trigger.
		
		if(is_SetExternalTrigger(	ueye_camera_handle,
									IS_SET_TRIGGER_SOFTWARE)
		== IS_SUCCESS){
			if(verbosity){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" trigger is set to software mode";
				out<<std::endl; std::cout<<out.str();
			}
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" failed to set software trigger mode!";
			out<<std::endl; std::cout<<out.str();
			return false;
		}

		//=================================
		// check wether we can set exposure time or not
		//=================================
		// not all cameras support arbirary exposure times
		bool exposure_time_can_be_modified;
		uint ueye_nCaps;
		
		int return_code_of_Exposure= 
		is_Exposure(ueye_camera_handle,
					IS_EXPOSURE_CAP_EXPOSURE,
					(void*)&ueye_nCaps, sizeof(ueye_nCaps));	
					
		if(return_code_of_Exposure== IS_SUCCESS){
			if(verbosity){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" exposure time can be modified.";
				out<<std::endl; std::cout<<out.str();
			}
			exposure_time_can_be_modified = true;
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" exposure time can not be modified!";
			out<<std::endl; std::cout<<out.str();	
			exposure_time_can_be_modified = false;
		}
		//=================================
		// set exposure time 
		//=================================
		// is_Exposure will use its argument to set the exposure time
		// and afterwards the argument is overwritten with the actual
		// exposure time! To avoid a drift of the exposure time, the 
		// argument is set to the value stored in
		// p_conf->settings_of_configuration_text_file
		// [ueye_exp_time_night] everytime again.
		
		if(exposure_time_can_be_modified)
		{	
			ueye_exposure_time_in_ms = 
			*pointer_to_desired_exposure_time_in_ms;
			
			int return_code_of_Exposure= 
			is_Exposure(	ueye_camera_handle,
							IS_EXPOSURE_CMD_SET_EXPOSURE,
							(void*)&ueye_exposure_time_in_ms,
							sizeof(ueye_exposure_time_in_ms));
			
			*pointer_to_desired_exposure_time_in_ms = 
			ueye_exposure_time_in_ms;
							
			if(return_code_of_Exposure== IS_SUCCESS){
				if(verbosity){
					out.str("");
					out<<"Ueye camera ID: "<<ueye_camera_id;
					out<<" exposure time was set to: ";
					out<<ueye_exposure_time_in_ms<<" [ms]";
					out<<std::endl; std::cout<<out.str();
				}
			}else if(return_code_of_Exposure== IS_CANT_OPEN_DEVICE){
				out.str("");
				out<<"Ueye cameraID: "<<ueye_camera_id;
				out<<" an attempt to initialize or select the camera";
				out<<" failed ";
				out<<"(no camera connected or initialization error)!";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== 
			IS_CANT_COMMUNICATE_WITH_DRIVER){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"Communication with the driver failed";
				out<<" because no driver has been loaded.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_INVALID_CAMERA_TYPE){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"The camera type defined in the .ini file";
				out<<" does not match the current camera model.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_INVALID_MODE){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"Camera is in standby mode, ";
				out<<"function not allowed UI-146x/UI-546x models: ";
				out<<"Function not allowed because the camera is in ";
				out<<"freerun synchronization mode.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_INVALID_PARAMETER){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"One of the submitted parameters is outside ";
				out<<"the valid range or is not supported for this ";
				out<<"sensor or is not available in this mode.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_IO_REQUEST_FAILED){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"An IO request from the uEye driver failed. ";
				out<<"Possibly the versions of the ueye_api.dll (API) ";
				out<<"and the driver file ";
				out<<"(ueye_usb.sys or ueye_eth.sys) do not match.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_NO_SUCCESS){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"General error message";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_NOT_CALIBRATED){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"The camera does not contain ";
				out<<"any calibration data.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_NOT_SUPPORTED){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"The camera model used here does not support ";
				out<<"this function or setting.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else if(return_code_of_Exposure== IS_TIMED_OUT){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"A timeout occurred. ";
				out<<"An sccan_image capturing process could not be ";
				out<<"terminated within the allowable period.";
				out<<std::endl; std::cout<<out.str();
				return false;
			}else{
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> set exposure time:"<<std::endl;
				out<<"Ueye camera failed to set exposure time, ";
				out<<"error code unknown!";
				out<<std::endl; std::cout<<out.str();
				return false;
			}
		}else{
			//==========================================================
			// get current exposure time when exposure time can not be 
			// modified
			//==========================================================
			// this depends on the current frame rate in free run mode
			// and current pixel clock.
			// low pixel clock && low frame rate ==> long exposure time
			// and vise versa
			int return_code_of_Exposure= 
			is_Exposure(	ueye_camera_handle,
							IS_EXPOSURE_CMD_GET_EXPOSURE,
							(void*)&ueye_exposure_time_in_ms,
							sizeof(ueye_exposure_time_in_ms));
							
			if(return_code_of_Exposure== IS_SUCCESS){
				if(verbosity){
					out.str("");
					out<<"Ueye camera ID: "<<ueye_camera_id;
					out<<" got current exposure time: ";
					out<<ueye_exposure_time_in_ms<<" [ms]";
					out<<std::endl; std::cout<<out.str();
				}
			}else{
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" unable to get current exposure time"<<std::endl;
				out<<std::endl; std::cout<<out.str();
			}
		}
		//=================================
		// capture frame
		//=================================
		// we want to write the sccan_image to the 
		// Hard-Disk so we use IS_WAIT 
		// instead of IS_DONT_WAIT which is used 
		// default and for live stream 
		// capturing.
		
		int return_of_FreezeVideo = 
		is_FreezeVideo(ueye_camera_handle,IS_WAIT);
		if(return_of_FreezeVideo==IS_SUCCESS){
			if(verbosity){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" -> is_FreezeVideo()->";
				out<<"stream stopped, waiting for trigger";
				out<<std::endl; std::cout<<out.str();
			}
		}else if(return_of_FreezeVideo==IS_BAD_STRUCTURE_SIZE){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_BAD_STRUCTURE_SIZE!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==
		IS_CANT_COMMUNICATE_WITH_DRIVER){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_CANT_COMMUNICATE_WITH_DRIVER!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_CANT_OPEN_DEVICE){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_CANT_OPEN_DEVICE!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_CAPTURE_RUNNING){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_CAPTURE_RUNNING!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_INVALID_BUFFER_SIZE){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_INVALID_BUFFER_SIZE!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_INVALID_CAMERA_TYPE){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_INVALID_CAMERA_TYPE!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_INVALID_EXPOSURE_TIME){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_INVALID_EXPOSURE_TIME!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_INVALID_CAMERA_HANDLE){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_INVALID_CAMERA_HANDLE!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_INVALID_MEMORY_POINTER){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_INVALID_MEMORY_POINTER!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_INVALID_PARAMETER){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_INVALID_PARAMETER!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_IO_REQUEST_FAILED){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_IO_REQUEST_FAILED!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_NO_ACTIVE_IMG_MEM){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_NO_ACTIVE_IMG_MEM!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_NO_USB20){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_NO_USB20!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_NO_SUCCESS){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_NO_SUCCESS!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_NOT_CALIBRATED){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_NOT_CALIBRATED!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_NOT_SUPPORTED){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_NOT_SUPPORTED!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_OUT_OF_MEMORY){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_OUT_OF_MEMORY!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_TIMED_OUT){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_TIMED_OUT!";
			out<<std::endl; std::cout<<out.str();
		}else if(return_of_FreezeVideo==IS_TRANSFER_ERROR){
			out.str("");
			out<<"ID: "<<ueye_camera_id;
			out<<" is_FreezeVideo()->IS_TRANSFER_ERROR!";
			out<<std::endl; std::cout<<out.str();
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" failed capture a frame -> FreezeVideo()!";
			out<<std::endl; std::cout<<out.str();
			return false;
		}
		
		//=================================
		// set file name
		//=================================
		// The name is created after the exposure time was set.
		// The ueye cam trys to set the chosen exposer time 
		// but there can be 
		// missmathes because of limited time resolution.
		// Therefore we use the actuel exposure time which is set by
		// is_Exposure.
		/*
			//=================================
			// create sccan_image file name
			//=================================	
			// filename identifier .png
			std::string image_file_type_string =".png"; 
			
			std::stringstream image_file_name_stream;
			image_file_name_stream<<desired_image_prefix;
			image_file_name_stream<<"_exposure_time_";
			image_file_name_stream.precision(0);
			image_file_name_stream<<fixed;
			image_file_name_stream<<
			ueye_exposure_time_in_ms*1000.0<<"us";
			image_file_name_stream<<	image_file_type_string;
			std::string image_file_name_string = 
			image_file_name_stream.str();
			
			// create an unicode filename using wide characters
			wstring image_file_name_wstr = wstring
			(image_file_name_string.begin(),
			image_file_name_string.end());
			wchar_t* image_file_name_wchar = 
			const_cast<wchar_t*>(image_file_name_wstr.c_str());
		*/
		//=================================
		// save frame
		//=================================
		
		memcpy(
		latest_image.image_matrix.ptr(),
		pointer_to_image_memory,
		(
		ueye_camera_sensor_number_of_pixels_in_width*
		ueye_camera_sensor_number_of_pixels_in_hight*
		ueye_number_of_coulor_channels
		)
		);
		
		latest_image.set_exposure_time_in_ms(ueye_exposure_time_in_ms);
		latest_image.set_time_stamp_in_seconds_since_startup();
		/*
		// set sccan_image parameters
		IMAGE_FILE_PARAMS ueye_img_file_param;
		ueye_img_file_param.pnImageID = NULL;
		ueye_img_file_param.ppcImageMem = NULL;
		ueye_img_file_param.nQuality = 100; 
		// 100 is best quality avaliable
		ueye_img_file_param.nFileType = IS_IMG_PNG; 
		//set data type to png
		ueye_img_file_param.pwchFileName = 
		image_file_name_wchar ; // set file name
		
		if(is_ImageFile(	ueye_camera_handle,
							IS_IMAGE_FILE_CMD_SAVE,
							(void*)&ueye_img_file_param,
							sizeof(ueye_img_file_param)
							)==IS_SUCCESS){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" -> ImageFile() -> sccan_image: "<<std::endl;
			out<<image_file_name_stream.str()<<" was written to disk";
			out<<std::endl; std::cout<<out.str();
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" failed to save sccan_image to disk!";
			out<<std::endl; std::cout<<out.str();
			return false;
		}
		*/
		 
		//=================================
		// free sccan_image memory
		//=================================
		if(is_FreeImageMem(	ueye_camera_handle,
							pointer_to_image_memory,
							ueye_memory_id)
		==IS_SUCCESS){
			if(verbosity){
				out.str("");
				out<<"Ueye camera ID: "<<ueye_camera_id;
				out<<" sccan_image memory was succesfully set free";
				out<<std::endl; std::cout<<out.str();
			}
		}else{
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id;
			out<<" failed to free sccan_image memory!";
			out<<std::endl; std::cout<<out.str();
			return false;
		}		
		
		// everything worked fine so return true
		return true;
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to acquire sccan_image! Camera not initialized!";
		out<<std::endl; std::cout<<out.str();
		return false;
	}
}
//======================================================================
bool acquire_image(double *pointer_to_desired_exposure_time_in_ms,
double desired_relative_maximal_camera_response){
	
	if(desired_relative_maximal_camera_response<0.0 || 
	desired_relative_maximal_camera_response>1.0){
		std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
		std::cout<<"acquire_image() -> ";
		std::cout<<"desired_relative_maximal_camera_response out of range ";
		std::cout<<"[0,1] -> "<<desired_relative_maximal_camera_response;
		std::cout<<std::endl;
		return false;
	}	
	
	if(*pointer_to_desired_exposure_time_in_ms<=0.0){
		std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
		std::cout<<"acquire_image() -> ";
		std::cout<<"desired_exposure_time_in_ms <= 0.0 ";
		std::cout<<*pointer_to_desired_exposure_time_in_ms<<"ms";
		std::cout<<std::endl;
		return false;
	}
	
	bool exposure_time_is_bad = true;
	bool image_acquisition_successfull = false;
	
	if(verbosity){
		std::cout<<"_________________________________________________"<<std::endl;
		std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
		std::cout<<"acquire_image() -> ";
		std::cout<<"with desired camera respone ";
		std::cout<<desired_relative_maximal_camera_response<<std::endl;
	}
	
	int exposure_itterations = 0;
	
	do{	
		std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
		std::cout<<"acquire_image() -> ";
		std::cout<<"exposure time itteration No.: ";
		std::cout<<exposure_itterations<<std::endl;
		
		if(verbosity){
			std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
			std::cout<<"acquire_image() -> ";
			std::cout<<"exposure time ";
			std::cout<<*pointer_to_desired_exposure_time_in_ms<<std::endl;
		}
		
		image_acquisition_successfull = acquire_image(
		pointer_to_desired_exposure_time_in_ms);
		
		double min_relative_camera_response = 0.5;
		double max_relative_camera_response = 0.5;
		
		latest_image.get_relative_min_and_max_pixel_value(
		&min_relative_camera_response,
		&max_relative_camera_response);
	
		if(verbosity){
			std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
			std::cout<<"acquire_image() -> ";
			std::cout<<"max_relative_camera_response ";
			std::cout<<max_relative_camera_response<<std::endl;
		}		
		
		double relative_camera_response_difference = 
		desired_relative_maximal_camera_response -
		max_relative_camera_response;
		
		if(abs(relative_camera_response_difference) <= 0.05 &&
		max_relative_camera_response != 1.0){
			exposure_time_is_bad = false;
			if(verbosity){
				std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
				std::cout<<"acquire_image() -> ";
				std::cout<<"relative_camera_response_difference ";
				std::cout<<relative_camera_response_difference;
				std::cout<<std::endl;
			}
		}else{
			
			exposure_time_is_bad = true;
			image_acquisition_successfull = false;
			exposure_itterations++;
			
			double exposure_time_scaling_factor;
			
			if(max_relative_camera_response == 1.0){
				exposure_time_scaling_factor = 0.5;
			}else if(max_relative_camera_response == 0.0){
				exposure_time_scaling_factor = 2.0;
			}else{
				exposure_time_scaling_factor = 
				pow(2.0,relative_camera_response_difference);
			}
		
			if(verbosity){
				std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
				std::cout<<"acquire_image() -> ";
				std::cout<<"relative_camera_response_difference ";
				std::cout<<relative_camera_response_difference;
				std::cout<<" scaling "<<exposure_time_scaling_factor<<std::endl;
			}
			
			(*pointer_to_desired_exposure_time_in_ms) =
			(*pointer_to_desired_exposure_time_in_ms)*
			exposure_time_scaling_factor;
		}
	}while(exposure_time_is_bad && exposure_itterations<=25);
	
	if(verbosity){
		std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
		std::cout<<"acquire_image() -> images taken with desired ";
		std::cout<<"camera response";
		std::cout<<std::endl;
		std::cout<<"_________________________________________________"<<std::endl;
	}
	return image_acquisition_successfull;
};
//======================================================================
bool long_time_exposure(bool long_time_exposure){
	flag_long_time_exposure = long_time_exposure;
	//==================================================================
	// set minimal framerate and minimal pixel clock
	//==================================================================	
	// the pixel clock has to be low to ensure good results using 25m 
	// active USB connection
	// get pixel clock range

	double ueye_desired_frames_per_second = 0.001;

	
	int return_of_SetFramerate = 
	is_SetFrameRate(ueye_camera_handle,
					ueye_desired_frames_per_second,
					&ueye_current_framerate_in_fps);
					
	if (return_of_SetFramerate == IS_SUCCESS)
	{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" framerate succesfully set to:";
		out<<ueye_current_framerate_in_fps<<" fps.";
		out<<std::endl; std::cout<<out.str();
	}else if(return_of_SetFramerate == IS_INVALID_MODE){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set framerate.";
		out<<" -> Function not allowed for this camera!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}else if(return_of_SetFramerate==IS_INVALID_PARAMETER){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set framerate -> Parameter out of range ";
		out<<"OR Function not allowed for this camera!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}else if(return_of_SetFramerate==IS_INVALID_CAMERA_HANDLE){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set framerate.";
		out<<" -> Invalid camera handle!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set minimal pixel clock.";
		out<<" -> arbitrary error!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}
	
	//==================================================================	
	// set minimal/default pixel clock
	//==================================================================
	uint ueye_desired_pixel_clock_in_MHz;
	if(flag_long_time_exposure==true){
		ueye_desired_pixel_clock_in_MHz = 
		ueye_pixel_clock_min_in_MHz;
	}else{
		ueye_desired_pixel_clock_in_MHz = 
		ueye_default_pixel_clock_in_MHz;
	}
	
	int return_of_PixelClock = 
	is_PixelClock(	ueye_camera_handle, 
					IS_PIXELCLOCK_CMD_SET, 
					(void*)&ueye_desired_pixel_clock_in_MHz, 
					sizeof(ueye_desired_pixel_clock_in_MHz));
					
	if (return_of_PixelClock == IS_SUCCESS)
	{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" pixel clock was succesfully set: ";
		out<<ueye_desired_pixel_clock_in_MHz<<"MHz";
		out<<std::endl; std::cout<<out.str();
	}else if(return_of_PixelClock == IS_INVALID_MODE){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set pixel clock.";
		out<<" -> Camera in standby, ";
		out<<"this action is not allowed in standby!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}else if(return_of_PixelClock == IS_INVALID_PARAMETER){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set pixel clock.";
		out<<" -> Parameter is out of range/ ";
		out<<"or is not supported by this camera-sensor!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}else if(return_of_PixelClock == IS_NOT_SUPPORTED){
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set pixel clock.";
		out<<" ->  Function is not supported by ";
		out<<"this camera-sensor!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to set pixel clock.";
		out<<" -> arbitrary error!"<<std::endl;
		out<<std::endl; std::cout<<out.str();
		return false;
	}	
	
	//==================================================================
	// get current pixel clock
	//==================================================================
	
	return_of_PixelClock =
	is_PixelClock(	ueye_camera_handle,
					IS_PIXELCLOCK_CMD_GET,
					(void*)&ueye_current_pixel_clock_in_MHz,
					sizeof(ueye_current_pixel_clock_in_MHz));
	
	if (return_of_PixelClock == IS_SUCCESS)
	{
	  	out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" got current pixel clock.";
		out<<std::endl; std::cout<<out.str();
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id;
		out<<" failed to get current pixel clock.";
		out<<std::endl; std::cout<<out.str();
		return false;
	}
	return true;
}
//======================================================================
void display_camera_information(){
	int sys_return = system("clear");
	std::stringstream out;
	out<<" ueye_camera_ID_"<<ueye_camera_id;
	out<<"____________________________________"<<std::endl;
	out<<intrinsic.get_status_prompt();
	out<<" ueye_camera__________________________________________"<<std::endl;
	out<<"| camera ID          : "<<ueye_camera_id<<std::endl;	
	if(initialization_succesfull){
	out<<"| initialized        : YES"<<std::endl;
	out<<"|"<<std::endl;
	out<<"| Serial number      : "<<	ueye_camera_info.SerNo	<<std::endl;
	out<<"| ID                 : "<<	ueye_camera_info.ID		<<std::endl;
	out<<"| Version            : "<< 	ueye_camera_info.Version<<std::endl;
	out<<"| Sensor             : ";
	out<<ueye_camera_sensor_number_of_pixels_in_width<<"x";
	out<<ueye_camera_sensor_number_of_pixels_in_hight<<" pixels"<<std::endl;
	out<<"| pixel clock        : ";
	out<<ueye_pixel_clock_min_in_MHz<<" MHz to ";
	out<<ueye_pixel_clock_max_in_MHz<<" MHz in ";
	out<<ueye_pixel_clock_increment_in_MHz<<" MHz steps"<<std::endl;
	out<<"| current pixel clock	: ";
	out<<ueye_current_pixel_clock_in_MHz<<" MHz"<<std::endl;
	out<<"| default pixel clock	: ";
	out<<ueye_default_pixel_clock_in_MHz<<" MHz"<<std::endl;
	out<<"| current frame rate	: ";
	out<<ueye_current_framerate_in_fps<<" fps"<<std::endl;
	out<<"| default frame rate	: ";
	out<<ueye_default_framerate_in_fps<<" fps"<<std::endl;
	if(flag_long_time_exposure){
	out<<"| long time exposure	: true"<<std::endl;
	}else{
	out<<"| long time exposure	: false"<<std::endl;
	}
	out<<"|"<<std::endl;
	out<<"| Final quality check date: "<< 	ueye_camera_info.Date<<std::endl;
	}else{
	out<<"| initialized        : NO"<<std::endl;	
	}
	out<<"|_____________________________________________________"<<std::endl;
	std::cout<<out.str();
}
//======================================================================
sccan_image get_latest_image(){
	if(verbosity){
		std::cout<<"ueye_camera->get_latest_image()"<<std::endl;
	}
	return latest_image;
}
//======================================================================
void export_latest_image(std::string filename_prefix){
	latest_image.set_name(filename_prefix);
	latest_image.export_image_to_disk();
}
//======================================================================
double get_current_exposure_time_in_ms(){
	return ueye_exposure_time_in_ms;
}
//======================================================================
void disp_latest_image(){
	if(verbosity){
		std::cout<<"ueye_camera -> disp_latest_image()"<<std::endl;
	}
	latest_image.disp_image();
}
//======================================================================
bool camera_status(){
	//==================================================================
	// try to take image
	//==================================================================
	double test_exposure_time_in_ms = 10.0;
	if (acquire_image(&test_exposure_time_in_ms ))
	{
		if(verbosity){
			out.str("");
			out<<"Ueye camera ID: "<<ueye_camera_id<<" -> ";
			out<<"camera_status() -> ";
			out<<"online";
			std::cout<<out.str()<<std::endl;	
		}
		return true;
	}else{
		out.str("");
		out<<"Ueye camera ID: "<<ueye_camera_id<<" -> ";
		out<<"camera_status() -> ";
		out<<"offline";
		std::cout<<out.str()<<std::endl;
		return false;
	}	
}
//======================================================================
void toggle_verbosity(){
	verbosity = !verbosity;
	intrinsic.toggle_verbosity(verbosity);
	latest_image.toggle_verbosity(verbosity);
}
//======================================================================
};
//======================================================================
class sccan_analysis_point		:public programm_verbosity{
private:
	pointing_direction star_position_relative_to_pointing_direction;
	double 	normalized_light_flux;
	uint 	mirror_ID;

public:	
//======================================================================
sccan_analysis_point(){
	verbosity = false;
	mirror_ID = 0;
	normalized_light_flux = 0.0;
}
//======================================================================
std::string get_info_prompt(){
	std::stringstream out;
	out<<" _sccan_analysis_point_mirror_ID_"<<mirror_ID;
	out<<"________"<<std::endl;
	out<<"| telescope tilt in x....: ";
	out<<star_position_relative_to_pointing_direction.
	get_x_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| telescope tilt in y....: ";
	out<<star_position_relative_to_pointing_direction.
	get_y_tilt_prompt_in_deg_min_sec()<<std::endl;
	out<<"| normalized light flux..: "<<normalized_light_flux;
	out<<" [mirror flux / star flux]";
	out<<std::endl;
	out<<"|_____________________________________________________"<<std::endl;
	return out.str();
}
//======================================================================
void set_mirror_ID(uint new_mirror_ID){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"set_mirror_ID() -> ";
		std::cout<<"new mirror ID of this sccan analysis point: ";
		std::cout<<new_mirror_ID<<std::endl;
	}
	mirror_ID = new_mirror_ID;
}
//======================================================================
uint get_mirror_ID(){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_mirror_ID() -> ";
		std::cout<<" mirror ID of this sccan analysis point: ";
		std::cout<<mirror_ID<<std::endl;
	}
	return mirror_ID;
}
//======================================================================
void set_normalized_light_flux(double new_normalized_light_flux){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"set_normalized_light_flux() -> ";
		std::cout<<"new_normalized_light_flux ";
		std::cout<<new_normalized_light_flux<<std::endl;
	}	
	normalized_light_flux = new_normalized_light_flux;
}
//======================================================================
double get_normalized_light_flux(){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_normalized_light_flux() -> ";
		std::cout<<"normalized_light_flux ";
		std::cout<<normalized_light_flux<<std::endl;
	}	
	return normalized_light_flux;
}
//======================================================================
void set_star_position_relative_to_pointing_direction
(pointing_direction new_star_position_relative_to_pointing_direction){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"set_star_position_relative_to_pointing_direction() -> ";
		std::cout<<std::endl;
	}	
	star_position_relative_to_pointing_direction = 
	new_star_position_relative_to_pointing_direction;
}
//======================================================================
pointing_direction get_star_position_relative_to_pointing_direction(){
	if(verbosity){
		std::cout<<"sccan_analysis_point -> ";
		std::cout<<"get_star_position_relative_to_pointing_direction() -> ";
		std::cout<<std::endl;
	}	
	return star_position_relative_to_pointing_direction;
}
//======================================================================
void toggle_verbosity(bool new_verbosity){
	
	verbosity = new_verbosity;
	
	star_position_relative_to_pointing_direction.
	toggle_verbosity(verbosity);
}
//======================================================================
};
//======================================================================
class sccan_point_pair			:public programm_verbosity{
public:
bool 			star_detection_verbosity;
sccan_image 	reflector_image;
sccan_image 	star_image;
std::string			reflector_image_name_postfix;
std::string			star_image_name_postfix;
uint 			run_number;
std::string 			sccan_point_name_prefix;

// caemras
ueye_camera 	*pointer_to_star_camera;
ueye_camera 	*pointer_to_reflector_camera;

// star detection

star_in_image	sccan_star;
bool			single_star_has_been_found;

// configuration text file
std::string 			sccan_point_configuration_file_name;
std::string			file_name_extension_for_sccan_point_configuration_file;
std::string 			key_sccan_point;
std::string			key_sccan_point_name_prefix;
std::string 			key_run_startup_time;
std::string 			key_sccan_run_number;
std::string 			key_star_camera_ID;
std::string 			key_star_image_filename;
std::string 			key_star_exposure_time_in_ms;
std::string 			key_reflector_camera_ID;
std::string 			key_reflector_image_filename;
std::string 			key_reflector_exposure_time_in_ms;
std::string 		 	key_single_star_has_been_found;
std::string			key_star_position_in_pixels_x;
std::string			key_star_position_in_pixels_y;
std::string			key_light_flux_of_star_in_pixel_values_per_milli_second;
std::string			key_absolute_light_flux_in_standard_bulbs;
//======================================================================
sccan_point_pair(uint new_run_number,bool new_verbosity){
	initialize(new_run_number,new_verbosity);
}
//======================================================================
void initialize(uint new_run_number,bool new_verbosity){
	verbosity = new_verbosity;
	star_detection_verbosity =false;
	
	reflector_image.toggle_verbosity(verbosity);
	star_image.toggle_verbosity(verbosity);
	sccan_star.toggle_verbosity(verbosity);
	
	//star_camera_intrinsic_parameter.toggle_verbosity(verbosity);

	//reflector_camera_intrinsic_parameter.toggle_verbosity(verbosity);
	
	reflector_image_name_postfix = "_reflector";
	star_image_name_postfix = "_star";	

	run_number = new_run_number;
	
	pointer_to_star_camera = NULL;
	pointer_to_reflector_camera = NULL;

	// star detection
	single_star_has_been_found = false;
	//star_position_in_pixel_x = -1;
	//star_position_in_pixel_y = -1;	
	create_sccan_point_name_prefix_using_current_timestamp();
	file_name_extension_for_sccan_point_configuration_file = ".cfg";
	
	set_sccan_point_name_prefix(sccan_point_name_prefix);
	
	key_sccan_point 					="sccan";
	key_run_startup_time 				="run_startup_time";
	key_sccan_point_name_prefix 		="sccan_point_name";
	key_sccan_run_number 				="sccan_run_number";
	key_star_camera_ID 					="star_camera_ID";
	key_star_image_filename 			="star_image_filename";
	key_star_exposure_time_in_ms 		="star_exposure_time_in_ms";
	key_reflector_camera_ID 			="reflector_camera_ID";
	key_reflector_image_filename 		="reflector_image_filename";
	key_reflector_exposure_time_in_ms	=
	"reflector_exposure_time_in_ms";
	key_single_star_has_been_found		="single_star_has_been_found";
	key_star_position_in_pixels_x		="star_X_px";
	key_star_position_in_pixels_y		="star_Y_px";
	//key_star_light_flux_is_valid		="light_flux_was_estimated";
	key_light_flux_of_star_in_pixel_values_per_milli_second =
	"light_flux_of_star_in_pixel_values_per_milli_second";
	
	key_absolute_light_flux_in_standard_bulbs = "absolute_light_flux";
}
//======================================================================
void set_sccan_point_name_prefix(std::string new_file_name_prefix){
	
	sccan_point_name_prefix = new_file_name_prefix;
	
	sccan_point_configuration_file_name = sccan_point_name_prefix;
	sccan_point_configuration_file_name += 
	file_name_extension_for_sccan_point_configuration_file;
}
//======================================================================
void set_run_number(uint new_run_number){
	run_number = new_run_number;
	
	create_sccan_point_name_prefix_using_current_timestamp();
	sccan_point_configuration_file_name = sccan_point_name_prefix;
	sccan_point_configuration_file_name += ".cfg";
}
//======================================================================
uint get_run_number(){
return run_number;
}
//======================================================================
void create_sccan_point_name_prefix_using_current_timestamp(){
	std::stringstream file_name_prefix_stream;
	file_name_prefix_stream<<global_time_stamp_manager_instance.
	get_startup_time_stamp();
	file_name_prefix_stream<<"_sccan_";
	file_name_prefix_stream<<run_number;
	sccan_point_name_prefix = file_name_prefix_stream.str();
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"create_sccan_point_name";
		std::cout<<"_prefix_using_current_timestamp() -> ";
		std::cout<<sccan_point_name_prefix<<std::endl;	
	}
}
//======================================================================
bool save_sccan_point_pair(){
	if(verbosity){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()"<<std::endl;
	}	
	
	if(reflector_image.export_image_to_disk()==false)
	{
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving reflector sccan_image!"<<std::endl;
		return false;
	}
		
	if(star_image.export_image_to_disk()==false)
	{
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving star sccan_image!"<<std::endl;
		return false;
	}
	
	if(save_sccan_point_configuration()==false){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving sccan config"<<std::endl;
		return false;
	}
	
	/*
	if(star_camera_intrinsic_parameter.
	save_intrinsic_parameter
	(sccan_point_name_prefix+star_image_name_postfix)==false){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving intrinsic parameter of star camera";
		std::cout<<std::endl;
		return false;
	}*/
	
	/*
	if(reflector_camera_intrinsic_parameter.
	save_intrinsic_parameter
	(sccan_point_name_prefix+reflector_image_name_postfix)
	==false){
		std::cout<<"sccan_point_pair->save_sccan_point_pair()->";
		std::cout<<"ERROR saving intrinsic parameter of reflector camera";
		std::cout<<std::endl;
		return false;
	}*/
	
	return true;
}
//======================================================================
bool load_sccan_point_pair(std::string sccan_point_name_prefix_to_load){
	if(verbosity){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()"<<std::endl;
	}
	
	if(reflector_image.load_image
	(sccan_point_name_prefix_to_load+reflector_image_name_postfix)
	==false)
	{
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading reflector sccan_image!"<<std::endl;
		return false;
	}	
	
	if(star_image.load_image
	(sccan_point_name_prefix_to_load+star_image_name_postfix)
	==false)
	{
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading star sccan_image!"<<std::endl;
		return false;
	}

	if(load_sccan_point_configuration
	(sccan_point_name_prefix_to_load)==false){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading sccan config"<<std::endl;
		return false;
	}	
	
	/*
	if(star_camera_intrinsic_parameter.
	load_intrinsic_camera_parameters
	(sccan_point_name_prefix_to_load+star_image_name_postfix)==false){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading intrinsic parameter of star camera";
		std::cout<<std::endl;
		return false;
	}
	*/
	/*
	if(reflector_camera_intrinsic_parameter.
	load_intrinsic_camera_parameters
	(sccan_point_name_prefix_to_load+reflector_image_name_postfix)
	==false){
		std::cout<<"sccan_point_pair->load_sccan_point_pair()->";
		std::cout<<"ERROR loading intrinsic parameter of reflector camera";
		std::cout<<std::endl;
		return false;
	}	
	*/
	return true;
}
//======================================================================
bool load_sccan_point_configuration
(std::string name_of_sccan_point_pair_to_load){
	Config sccan_point_configuration_handle;
	//==================================================================
	// Read the file. If there is an error, report it and exit.
	//==================================================================
	name_of_sccan_point_pair_to_load +=
	file_name_extension_for_sccan_point_configuration_file;
	std::cout<<"sccan_point_pair -> load_sccan_point_configuration() -> ";
	std::cout<<name_of_sccan_point_pair_to_load<<std::endl;
	
	try
	{
		sccan_point_configuration_handle.
		readFile(name_of_sccan_point_pair_to_load.c_str());
	}
	catch(const FileIOException &fioex)
	{
		std::cout << "I/O error while reading file." << std::endl;
		return(EXIT_FAILURE);
	}
	catch(const ParseException &pex)
	{
		std::cout << "Parse error at " << pex.getFile() << ":";
		std::cout << pex.getLine() << " - " << pex.getError() << std::endl;
		return(EXIT_FAILURE);
	}
	//==================================================================
	// get data
	//==================================================================
	const Setting& root = sccan_point_configuration_handle.getRoot();
	
	bool reading_is_valid = true;
	
	// Output a list of all books in the inventory.
	try
	{
		const Setting &Sccan = root[key_sccan_point];

		// delete all old sccan information
	//==================================================================
	// run number		
	//==================================================================
	int new_sccan_point_pair_run_number;
	if( Sccan.lookupValue
	(key_sccan_run_number, new_sccan_point_pair_run_number) ){
		initialize((uint)new_sccan_point_pair_run_number,verbosity);
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_sccan_run_number<<" is set to ";
			std::cout<<run_number<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> load_sccan_point_configuration() -> ";
		std::cout<<key_sccan_run_number<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	// sccan_point_name_prefix
	//==================================================================
	std::string new_sccan_point_name_prefix;
	if( Sccan.lookupValue
	(key_sccan_point_name_prefix, new_sccan_point_name_prefix) ){
		set_sccan_point_name_prefix(new_sccan_point_name_prefix);
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_sccan_point_name_prefix<<" is set to ";
			std::cout<<sccan_point_name_prefix<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> load_sccan_point_configuration() -> ";
		std::cout<<key_sccan_point_name_prefix<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// single star flag
	//==================================================================
	bool single_star_has_been_found_flag_of_file;
	if( Sccan.lookupValue
	(key_single_star_has_been_found, 
	single_star_has_been_found_flag_of_file) 
	){
		
		single_star_has_been_found = 
		single_star_has_been_found_flag_of_file;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_single_star_has_been_found<<" is set to ";
			if(single_star_has_been_found)
			{std::cout<<"true";}else{std::cout<<"false";}
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_sccan_point_name_prefix<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// single star pixel position X
	//==================================================================
	int new_single_star_pixel_position_x;
	if( 
	Sccan.lookupValue(
	key_star_position_in_pixels_x, new_single_star_pixel_position_x) 
	){
		sccan_star.position_in_pixels.x = 
		new_single_star_pixel_position_x;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_star_position_in_pixels_x<<" is set to ";
			std::cout<<sccan_star.position_in_pixels.x;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_star_position_in_pixels_x<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	// single star pixel position Y
	//==================================================================
	int new_single_star_pixel_position_y;
	if( 
	Sccan.lookupValue(
	key_star_position_in_pixels_y, new_single_star_pixel_position_y) 
	){
		sccan_star.position_in_pixels.y =
		new_single_star_pixel_position_y;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<key_star_position_in_pixels_y<<" is set to ";
			std::cout<<sccan_star.position_in_pixels.y;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_star_position_in_pixels_y<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	sccan_star.star_direction_relative_to_camera = 
	star_image.intrinsic_parameters.
	calculate_pointing_direction(sccan_star.position_in_pixels);
	
	//==================================================================
	// light flux in
	//==================================================================
	/*
	double new_light_flux_of_star_in_pixel_values_per_milli_second;
	if( 
	Sccan.lookupValue(
	key_light_flux_of_star_in_pixel_values_per_milli_second,
	new_light_flux_of_star_in_pixel_values_per_milli_second) 
	){
		light_flux_of_star_in_pixel_values_per_milli_second = 
		new_light_flux_of_star_in_pixel_values_per_milli_second;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<
			key_light_flux_of_star_in_pixel_values_per_milli_second;
			std::cout<<" is set to ";
			std::cout<<light_flux_of_star_in_pixel_values_per_milli_second;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_light_flux_of_star_in_pixel_values_per_milli_second;
		std::cout<<" fail."<<std::endl;
		reading_is_valid = false;
	}	*/
	//==================================================================
	// absolute light flux standard bulbs
	//==================================================================
	double new_absolute_light_flux_in_standard_bulbs;
	if( 
	Sccan.lookupValue(
	key_absolute_light_flux_in_standard_bulbs,
	new_absolute_light_flux_in_standard_bulbs) 
	){
		sccan_star.light_flux_of_star_in_bulbs = 
		new_absolute_light_flux_in_standard_bulbs;
		
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"load_sccan_point_configuration() -> ";
			std::cout<<
			key_absolute_light_flux_in_standard_bulbs;
			std::cout<<" is set to ";
			std::cout<<sccan_star.light_flux_of_star_in_bulbs;
			std::cout<<std::endl;
		}
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"load_sccan_point_configuration() -> ";
		std::cout<<key_absolute_light_flux_in_standard_bulbs;
		std::cout<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	
	//==================================================================		
	}
	catch(const SettingNotFoundException &nfex)
	{
		std::cout<<"reflector -> load_sccan_point_configuration() ->";
		std::cout<<"loading failed! Settings not found!"<<std::endl;
		reading_is_valid  = false;
	}
	return reading_is_valid;
}
//======================================================================
std::string get_sccan_point_pair_string(){
	std::stringstream out;
	out<<" _sccan_point_pair_No"<<run_number;
	out<<"_____________________"<<std::endl;
	out<<"| "<<std::endl;
	out<<"| sccan name prefix...: "<<sccan_point_name_prefix<<std::endl;
	out<<"| startup time........: "<<global_time_stamp_manager_instance.get_startup_time_detail();
	out<<"| run number..........: "<<run_number<<std::endl;
	out<<"| "<<std::endl;
	out<<"| star camera ID......: ";
	if(pointer_to_star_camera != NULL){
	out<<pointer_to_star_camera->get_camera_ID()<<std::endl;
	}else if(
	pointer_to_star_camera == NULL && 
	reflector_image.get_init_status() == true){
	out<<star_image.
	get_camera_ID_of_camera_taken_this_image()<<std::endl;
	}else{
	out<<"not set yet"<<std::endl;
	}
	out<<"| star file name......: "<<star_image.get_file_name()<<std::endl;
	out<<"| exposure time.......: ";
	out<<star_image.get_exposure_time_in_ms()<<" ms"<<std::endl;
	out<<"| "<<std::endl;
	out<<"| reflector camera ID.: ";
	if(pointer_to_reflector_camera != NULL){
	out<<pointer_to_reflector_camera->get_camera_ID()<<std::endl;
	}else if(
	pointer_to_reflector_camera == NULL && 
	reflector_image.get_init_status() == true){
	out<<reflector_image.
	get_camera_ID_of_camera_taken_this_image()<<std::endl;
	}else{
	out<<"not set yet"<<std::endl;
	}
	out<<"| reflector file name.: "<<reflector_image.get_file_name()<<std::endl;
	out<<"| exposure time.......: ";
	out<<reflector_image.get_exposure_time_in_ms()<<" ms"<<std::endl;
	out<<"| "<<std::endl;
	out<<"| single star was found......:";
	if(single_star_has_been_found){
	out<<"TRUE at ("<<sccan_star.position_in_pixels.x;
	out<<"|"<<sccan_star.position_in_pixels.y<")";
	}else{
	out<<"FALSE";
	}
	out<<std::endl;
	out<<"|__________________________________________"<<std::endl;
	
	if(verbosity){
		std::cout<<"sccan_point_pair->get_sccan_point_pair_string()"<<std::endl;
	}
	return out.str();
}
//======================================================================
void disp(){
	std::cout<<get_sccan_point_pair_string();
}
//======================================================================
void set_cameras(ueye_camera* star_camera,ueye_camera* reflector_camera){
pointer_to_star_camera = star_camera;
//star_camera_intrinsic_parameter = pointer_to_star_camera->intrinsic;

pointer_to_reflector_camera = reflector_camera;
//reflector_camera_intrinsic_parameter = reflector_camera->intrinsic;
}
//======================================================================
bool acquire_sccan_images(
double *pointer_to_star_exposure_time_in_ms, 
double *pointer_to_reflector_exposure_time_in_ms,
int		max_number_of_exposure_itterations,
bool	*max_number_of_exposure_itterations_has_been_exceeded
){
	bool star_image_ok;
	bool reflector_image_ok;
	
	star_image_ok =
	pointer_to_star_camera->
	acquire_image(pointer_to_star_exposure_time_in_ms,0.9);
	
	reflector_image_ok =
	pointer_to_reflector_camera->
	acquire_image(pointer_to_reflector_exposure_time_in_ms,0.9);	
	
	star_image = 
	pointer_to_star_camera->get_latest_image();
	star_image.set_name
	(sccan_point_name_prefix+star_image_name_postfix);
	star_image.toggle_verbosity(verbosity);
	
	reflector_image = 
	pointer_to_reflector_camera->get_latest_image();
	reflector_image.set_name
	(sccan_point_name_prefix+reflector_image_name_postfix);
	reflector_image.toggle_verbosity(verbosity);
	
	if(star_image_ok==true && reflector_image_ok==true){
		if(verbosity){
			std::cout<<"sccan_point_pair -> acquire_sccan_images() -> ";
			std::cout<<"images have been acquired"<<std::endl;
			*max_number_of_exposure_itterations_has_been_exceeded = true;
		}
	}else{
		std::cout<<"sccan_point_pair -> acquire_sccan_images() -> ";
		std::cout<<"aquisition failed. ";
		std::cout<<"Exposure might be out of range!"<<std::endl;
		*max_number_of_exposure_itterations_has_been_exceeded = false;
		return false;
	}
}
//======================================================================
bool save_sccan_point_configuration(){
	Config sccan_point_configuration_handle;
	Setting &root = sccan_point_configuration_handle.getRoot();

	// Add some settings to the configuration.
	Setting &sccan_point_settings = 
	root.add(key_sccan_point, Setting::TypeGroup);
	
	sccan_point_settings.
	add(key_sccan_point_name_prefix,Setting::TypeString) =
	sccan_point_name_prefix;
	
	sccan_point_settings.
	add(key_run_startup_time,Setting::TypeString) =
	global_time_stamp_manager_instance.get_startup_time_detail();
	
	sccan_point_settings.
	add(key_sccan_run_number, Setting::TypeInt) =
	(int)run_number;

	sccan_point_settings.
	add(key_star_image_filename,Setting::TypeString) = 
	star_image.get_file_name();

	sccan_point_settings.
	add(key_reflector_image_filename,	Setting::TypeString) = 
	reflector_image.get_file_name();

	sccan_point_settings.
	add(key_single_star_has_been_found,Setting::TypeBoolean) = 
	single_star_has_been_found;

	sccan_point_settings.
	add(key_star_position_in_pixels_x,Setting::TypeInt) = 
	(int)sccan_star.position_in_pixels.x;
	
	sccan_point_settings.
	add(key_star_position_in_pixels_y,Setting::TypeInt) = 
	(int)sccan_star.position_in_pixels.y;
	//==================================================================
	// light flux of star
	//==================================================================
	//relative 
	/*
	sccan_point_settings.
	add(key_light_flux_of_star_in_pixel_values_per_milli_second,
	Setting::TypeFloat) = 
	light_flux_of_star_in_pixel_values_per_milli_second;
	*/
	// absolute
	sccan_point_settings.
	add(key_absolute_light_flux_in_standard_bulbs,
	Setting::TypeFloat) = 
	sccan_star.light_flux_of_star_in_bulbs;
	
	// Write out the new configuration.
	try
	{
		sccan_point_configuration_handle.
		writeFile(sccan_point_configuration_file_name.c_str());
		
		if(verbosity){
			std::cout << "New configuration successfully written to: "<< 
			sccan_point_configuration_file_name<<std::endl;
		}
	}
	catch(const FileIOException &fioex)
	{
		std::cerr << "I/O error while writing file: "<<
		sccan_point_configuration_file_name<<std::endl;
		return(false);
	}

	return(true);	
}
//======================================================================
void find_bright_star_in_star_image
(double threshold_in_sigmas,
double radius_one_sigma_for_star_detection_in_degrees){
	
	std::vector<star_in_image> list_of_stars_in_star_image =
	star_image.find_stars(
	threshold_in_sigmas,
	radius_one_sigma_for_star_detection_in_degrees
	);

	if(list_of_stars_in_star_image.size() == 1){
		//one single star was found
		
		single_star_has_been_found = true;
		sccan_star = list_of_stars_in_star_image.at(0);
		
		if(verbosity){
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<" found one single star brighter than ";
			std::cout<<threshold_in_sigmas<<" sigma at ";
			std::cout<<"("<<sccan_star.position_in_pixels.x;
			std::cout<<"|"<<sccan_star.position_in_pixels.y;
			std::cout<<")px ";
			std::cout<<"flux ";
			std::cout<<sccan_star.light_flux_of_star_in_bulbs;
			std::cout<<"bulbs";
			std::cout<<std::endl;
		}
		
	}else if(list_of_stars_in_star_image.size() == 0){
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<" found no star at all brighter than ";
			std::cout<<threshold_in_sigmas<<" sigma."<<std::endl;
	}else{
			std::cout<<"find_bright_star_in_star_image() -> ";
			std::cout<<" found "<<list_of_stars_in_star_image.size();
			std::cout<<" stars brighter than ";
			std::cout<<threshold_in_sigmas<<" sigma."<<std::endl;
	}
}
//======================================================================
void display_calibration_star_in_star_image(int time_to_display_in_ms){
	if(single_star_has_been_found){
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"display_calibration_star_in_star_image() -> ";
			std::cout<<"the calibration star was found in star_image.";
			std::cout<<std::endl;
		}
		//visualize stars
		list_of_pixel_positions list_of_star_locations_in_pixels;
		
		cv::Point calibration_star=
		sccan_star.position_in_pixels;
		
		list_of_star_locations_in_pixels.push_back(calibration_star);
		
		simple_image star_image_with_overlay = star_image.
		get_overlay_with_stars(
		list_of_star_locations_in_pixels,
		cv::Scalar(128,128,255)
		);
		
		std::stringstream window_name; 
		window_name<<"star overlay, ";
		window_name<<list_of_star_locations_in_pixels.size()<<"stars, ";

		star_image_with_overlay.
		disp_simple_image(window_name.str(),time_to_display_in_ms);	
	}else{
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"display_calibration_star_in_star_image() -> ";
		std::cout<<"the calibration star was not found in star_image now!";
		std::cout<<std::endl;
	}
}
//======================================================================
bool a_single_star_has_been_found(){
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"a_single_star_has_been_found() -> ";
		std::cout<<"return ";
		if(single_star_has_been_found)
		{std::cout<<"true";}else{std::cout<<"false";}
		std::cout<<std::endl;
	}
	return(single_star_has_been_found);
}
//======================================================================
cv::Point get_point_of_star_in_star_image(){
	return sccan_star.position_in_pixels;
}
//======================================================================
simple_image get_resized_star_image_with_overlay
(int height_of_alignment_image){
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_star_image_with_overlay() -> ";
		std::cout<<"start."<<std::endl;
	}
	
	double scaling_factor_for_star_image;
	simple_image resized_star_image = star_image.get_resized_image
	(height_of_alignment_image,&scaling_factor_for_star_image);

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_star_image_with_overlay() -> ";
		std::cout<<"got resized star image."<<std::endl;
	}
	
	//==================================================================
	// add information
	//==================================================================
	int number_of_pixels_of_image = 
	resized_star_image.get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);
	

	double circle_radius_in_pixel_per_image_line = 25.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 1.0/1e3;
	
	// circle spcific information
	double circle_radius_in_pixel = 
	circle_radius_in_pixel_per_image_line*mean_number_of_image_lines;
	int circle_thickness_in_pixel = 
	(int)ceil(
	circle_thickness_in_pixel_per_image_line*mean_number_of_image_lines
	);
	int circle_lineType	=8;		
	
	// exposure time
	std::stringstream text_to_put_in_image;
	text_to_put_in_image<<star_image.
	get_exposure_time_in_ms();
	text_to_put_in_image<<"ms";
	
	resized_star_image.text_overlay(text_to_put_in_image.str(),
	0.1,0.1,1.0);
	
	// star position
	cv::Point star_position_in_original;
	cv::Point star_position_in_scaled_image;
	if(a_single_star_has_been_found()){
		
		star_position_in_original = 
		sccan_star.position_in_pixels;
		
		star_position_in_scaled_image.y = (int)
		((double)star_position_in_original.x*
		scaling_factor_for_star_image);
		
		star_position_in_scaled_image.x = (int)
		((double)star_position_in_original.y*
		scaling_factor_for_star_image);
		
		cv::Point circle_position_openCV_bug_x_y_switched;
		circle_position_openCV_bug_x_y_switched.x = 
		star_position_in_scaled_image.y;
		circle_position_openCV_bug_x_y_switched.y = 
		star_position_in_scaled_image.x;
		
		// star circle
		cv::circle( 
		resized_star_image.image_matrix,
		circle_position_openCV_bug_x_y_switched,
		circle_radius_in_pixel,
		cv::Scalar( 0, 0, 255),
		circle_thickness_in_pixel,
		circle_lineType );
				
		// light flux
		text_to_put_in_image.str("");
		text_to_put_in_image.precision(3);
		text_to_put_in_image<<
		sccan_star.light_flux_of_star_in_bulbs;
		text_to_put_in_image<<"bulbs";
		
		cv::Point light_flux_text_position = 
		circle_position_openCV_bug_x_y_switched + 
		cv::Point(2*circle_radius_in_pixel,0);
		
		
		resized_star_image.text_overlay(text_to_put_in_image.str(),
		light_flux_text_position,1.0);

	}else{
		resized_star_image.text_overlay(
		"No single star found!",0.5,0.5,2.0);
	}		
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_star_image_with_overlay() -> ";
		std::cout<<"end."<<std::endl;
	}
	
	return resized_star_image;
}
//======================================================================
simple_image get_resized_reflector_image
(int height_of_alignment_image){

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_reflector_image_with_overlay() -> ";
		std::cout<<"start."<<std::endl;
	}
	
	double scaling_factor_for_reflector_image;
	
	simple_image resized_reflector_image = 
	reflector_image.get_resized_image
	(height_of_alignment_image,&scaling_factor_for_reflector_image);

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_resized_reflector_image_with_overlay() -> ";
		std::cout<<"got resized reflector image."<<std::endl;
	}
		
	return resized_reflector_image;
}
//======================================================================
void get_reflector_image_exposure_time_stamp
(simple_image* pointer_to_reflector_image_for_overlay){

	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_reflector_image_exposure_time_stamp() -> ";
		std::cout<<"start."<<std::endl;
	}

	//==================================================================
	// add information
	//==================================================================
	std::stringstream exposure_time_prompt;
	exposure_time_prompt<<reflector_image.get_exposure_time_in_ms();
	exposure_time_prompt<<"ms";
	
	pointer_to_reflector_image_for_overlay->text_overlay(
	exposure_time_prompt.str(),0.1,0.1,1.0);
	
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"get_reflector_image_exposure_time_stamp() -> ";
		std::cout<<"end."<<std::endl;
	}
		
}
//======================================================================
// analysis
//======================================================================
sccan_analysis_point* calculate_sccan_analysis_point(
list_of_pixel_positions* pointer_to_list_of_pixel_positions_of_specific_mirror,
uint mirror_ID
){
	
	if(verbosity){
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"calculate_sccan_analysis_point() -> ";
		std::cout<<"start."<<std::endl;
	}
	
	if(single_star_has_been_found){
		//single_star_has_been_found
		
		sccan_analysis_point* pointer_to_a_new_sccan_analysis_point;

		pointer_to_a_new_sccan_analysis_point = 
		new sccan_analysis_point;
		
		pointer_to_a_new_sccan_analysis_point->
		toggle_verbosity(verbosity);
		
		//==============================================================
		// fill sccan analysis point
		//==============================================================
		pointer_to_a_new_sccan_analysis_point->
		set_mirror_ID(mirror_ID);
	
		pointer_to_a_new_sccan_analysis_point->
		set_star_position_relative_to_pointing_direction(
		sccan_star.star_direction_relative_to_camera
		);
	
		if(verbosity){
			std::cout<<"sccan_point_pair -> ";
			std::cout<<"calculate_sccan_analysis_point() -> ";
			std::cout<<"number of pixels in mask ";
			std::cout<<pointer_to_list_of_pixel_positions_of_specific_mirror->size();
			std::cout<<std::endl;
		}
		
		double mean_light_flux_of_specific_reflector_mirror_in_bulbs = 
		reflector_image.
		mean_light_flux_in_bulbs_of_complex_image_region(
		pointer_to_list_of_pixel_positions_of_specific_mirror);
		
		double normalized_light_flux = 
		mean_light_flux_of_specific_reflector_mirror_in_bulbs/
		sccan_star.light_flux_of_star_in_bulbs;
		
		pointer_to_a_new_sccan_analysis_point->
		set_normalized_light_flux(normalized_light_flux);
		
		return pointer_to_a_new_sccan_analysis_point;
	
	}else{
		//no single_star_has_been_found	
		std::cout<<"sccan_point_pair -> ";
		std::cout<<"calculate_sccan_analysis_point() -> ";
		std::cout<<"was called but there was no single ";
		std::cout<<"star found in this sccan point ";
		std::cout<<sccan_point_name_prefix;
		std::cout<<std::endl;

		return NULL;
	}
	
}
//======================================================================
};
//======================================================================
class mirror					:public programm_verbosity{
public:
	//bool 		verbosity_mirror;
	uint 		mirror_ID;
	
	// mirror mount orientation
	double tripod_orientation_z_in_rad;

	bool flag_manipulations_have_been_calculated;
	double manipulation_distance_of_first_tripod_leg_in_m;
	double manipulation_distance_of_second_tripod_leg_in_m;
	double manipulation_distance_of_third_tripod_leg_in_m;

	double manipulation_revolutions_of_first_tripod_leg_in_revs;
	double manipulation_revolutions_of_second_tripod_leg_in_revs;
	double manipulation_revolutions_of_third_tripod_leg_in_revs;

	// missalignment
	bool 	flag_misalignment_angles_have_been_calculated;
	double 	missalignment_angle_in_x_in_rad;
	double 	missalignment_angle_in_y_in_rad;
	
	// mask sccan_image
	bool 				flag_mask_has_been_initialized;
	std::vector<cv::Point> 	list_of_points_defining_mirror_polygon;
	simple_image		image_with_mirror_mask;
	list_of_pixel_positions 		list_of_Points_inside_mirror_polygon;
	
	// configuration file
	std::string 	key_ID;
	std::string 	key_tripod_orientation_z_in_rad;
	std::string	key_list_of_points_defining_mirror_polygon;
	std::string	key_polygon_point_pixel_coordinate_x;
	std::string	key_polygon_point_pixel_coordinate_y;	
//======================================================================
mirror(uint new_mirror_ID,bool new_verbosity){
	initialize(new_mirror_ID,new_verbosity);
}
//======================================================================
void set_mirror_ID(uint new_mirror_ID){
	mirror_ID = new_mirror_ID;
}
//======================================================================
uint get_mirror_ID(){
	return mirror_ID;
}
//======================================================================
void initialize(uint new_mirror_ID,bool new_verbosity){
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
std::string get_string(){
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
void disp(){std::cout<<get_string();}
//======================================================================
void export_configuration_setting(Setting *Mirror_file_handle){
	
	//mirror ID
	Mirror_file_handle->add(key_ID, Setting::TypeInt) = (int)mirror_ID;
	
	//tripod orientation
	Mirror_file_handle->
	add(key_tripod_orientation_z_in_rad, Setting::TypeFloat) = 
	tripod_orientation_z_in_rad;
	
	//list_of_pixes_defining_polygon
	if(
		! Mirror_file_handle->exists
		(key_list_of_points_defining_mirror_polygon)
	){
		Mirror_file_handle->add
		(key_list_of_points_defining_mirror_polygon, Setting::TypeList);	
	}
	
	Setting &polygon_file_handle = 
	(*Mirror_file_handle)[key_list_of_points_defining_mirror_polygon];
	
	for(
	int polygon_point_itterator = 0;
	polygon_point_itterator<
	list_of_points_defining_mirror_polygon.size();
	polygon_point_itterator++
	){
		Setting &polygon_point_handle = polygon_file_handle.add
		(Setting::TypeGroup);
		
		polygon_point_handle.add
		(key_polygon_point_pixel_coordinate_x,Setting::TypeInt) = 
		list_of_points_defining_mirror_polygon.at
		(polygon_point_itterator).x;
		
		polygon_point_handle.add
		(key_polygon_point_pixel_coordinate_y,Setting::TypeInt) = 
		list_of_points_defining_mirror_polygon.at
		(polygon_point_itterator).y;
		
	}
	
	if(verbosity){
		std::cout<<"mirror -> export_configuration_setting() ->";
		std::cout<<" mirror exorted. "<<std::endl;
	}
}
//======================================================================
bool import_configuration(Setting *Mirror_file_handle){
	
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
	const Setting &polygon_file_handle = 
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
		Setting &polygon_point_handle = 
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
void initialize_list_of_points_inside_mirror_polygon
(cv::Size image_size_of_reflector_camera){
	
	create_polygon_mask(image_size_of_reflector_camera);
	create_list_of_Points_inside_mirror_polygon();
	if(verbosity){
		std::cout<<"mirror -> ";
		std::cout<<"initialize_list_of_points_inside_mirror_polygon()"<<std::endl;
	}
}
//======================================================================
void create_polygon_mask(cv::Size image_size_of_reflector_camera){

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
void create_list_of_Points_inside_mirror_polygon(){
	list_of_Points_inside_mirror_polygon = 
	image_with_mirror_mask.create_list_of_Points_of_non_zero_pixels();
}
//======================================================================
list_of_pixel_positions* get_pointer_to_list_of_Points_inside_mirror_polygon(){
	return &list_of_Points_inside_mirror_polygon;
}
//======================================================================
simple_image highlight_mirror_in_reflector_image
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
};
//======================================================================
class reflector 				:public user_interaction{
private:
	std::string		reflector_name;
		
	// general reflector properties
	double tripod_radius_in_m;
	double pitch_of_bolt_in_m_per_revolution;
	
	// reflector calibration sccan_image
	std::string		name_of_calibration_image;
	sccan_image		reflector_calibration_image;
	bool 		flag_calibration_image_with_polygons_created;
	sccan_image		reflector_calibration_image_with_polygons;
	std::string		name_of_calibration_window_with_highlighted_poygons;
	
	// reflector configuration text file
	std::string		file_name_extension_for_configuration_file;
	std::string 		reflector_configuration_file_name;
	std::string		key_list_of_mirrors;
	std::string		key_reflector;
	std::string		key_reflector_name;
	std::string		key_reflector_calibration_image;
	std::string		key_tripod_radius_in_m;
	std::string		key_pitch_of_bolt_in_m_per_revolution;
//======================================================================
public:
//======================================================================
reflector(){
	verbosity = true;
	
	//mirror_list
	initialize_mirror_list();
	
	// reflector configuration text file
	file_name_extension_for_configuration_file = ".cfg";
	key_list_of_mirrors 					="mirrors";
	key_reflector 							="reflector";
	key_reflector_name			 			="reflector_name";
	key_reflector_calibration_image			="calibration_image";
	key_tripod_radius_in_m					="tripod_radius_in_m";
	key_pitch_of_bolt_in_m_per_revolution	=
	"pitch_of_bolt_in_m_per_revolution";
	
	initialize_default_reflector();
	menu_name = "reflector manipulation";
	
	reflector_calibration_image.
	toggle_verbosity(verbosity);
	reflector_calibration_image_with_polygons.
	toggle_verbosity(verbosity);
}
//======================================================================
void initialize_default_reflector(){
	pointer_to_current_mirror_to_work_with = NULL;

	list_of_pointers_to_mirrors.clear();
	
	reflector_name = "default_reflector";
	reflector_configuration_file_name =  reflector_name;
	reflector_configuration_file_name += ".cfg";
	
	name_of_calibration_image = "";
	
	name_of_calibration_window_with_highlighted_poygons = 
	"calibration sccan_image with polygons";
	
	tripod_radius_in_m = 0.295;
	pitch_of_bolt_in_m_per_revolution = 0.001;
	flag_calibration_image_with_polygons_created = false;
}
//======================================================================
void interaction(){	

	std::string key_manipulate_mirror = "m";
	add_control(key_manipulate_mirror,"manipulate current mirror ID");
	
	std::string key_choose_mirror_ID = "c";	
	add_control(key_choose_mirror_ID,"choose mirror ID to manipulate");
	
	std::string key_show_calibration_image = "s";	
	add_control(key_show_calibration_image,
	"show calibration sccan_image");
	
	std::string key_show_calibration_image_with_polygons = "sp";
	add_control(key_show_calibration_image_with_polygons,
	"show calibration image with mirror polygons");
	
	std::string key_calibration_image = "i";	
	add_control(key_calibration_image,"choose calibration sccan_image");
	
	std::string key_change_name = "name";	
	add_control(key_change_name,"change reflector name");
	
	std::string key_delete_current_mirror = "delete";	
	add_control(key_delete_current_mirror,"delete current mirror ID");	

	std::string key_save_reflector = "save";	
	add_control(key_save_reflector,"save reflector setup to file");
	
	std::string key_load_reflector = "load";	
	add_control(key_load_reflector,"load reflector setup from file");	
	
	std::string key_quit = "back";	
	add_control(key_quit,"back to main menu");
	
	std::stringstream out;
	bool user_wants_to_quit_mirror_manipulation = false;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_reflector_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==================================================================
	if(valid_user_input.compare(key_load_reflector)==0){	
		if(read_configuration(get_alphanumeric_string_by_user())){
			std::cout<<"reflector -> interaction() ->";
			std::cout<<" loading reflector successful."<<std::endl;
		}else{
			std::cout<<"reflector -> interaction() ->";
			std::cout<<" loading reflector failed."<<std::endl;		
		}
	}
	//==================================================================
	if(valid_user_input.compare(key_show_calibration_image)==0){	
		reflector_calibration_image.disp_image();
	}
	//==================================================================
	if(valid_user_input.compare(
	key_show_calibration_image_with_polygons)==0){
		update_calibration_image_with_polygons(false);
		reflector_calibration_image_with_polygons.disp_image();
	}
	//==================================================================
	if(valid_user_input.compare(key_calibration_image)==0){	
		set_reflector_calibration_image
		(get_alphanumeric_string_by_user());
	}
	//==================================================================
	if(valid_user_input.compare(key_change_name)==0){	
		set_reflector_name(get_alphanumeric_string_by_user());
	}
	//==================================================================
	if(valid_user_input.compare(key_save_reflector)==0){
		write_configuration_to_disk();
		save_calibration_image_with_mirror_polygons();
	}
	//==================================================================
	if(valid_user_input.compare(key_manipulate_mirror)==0){
		if(pointer_to_current_mirror_to_work_with==NULL){
			std::cout<<"reflector -> interaction() -> [";
			std::cout<<key_manipulate_mirror<<"] manipulate_mirror ";
			std::cout<<"but mirror ID is not defined yet!"<<std::endl;
		}else{
			if(reflector_calibration_image.get_init_status()){
				manipulate_mirror_polygon();
			}else{
				std::cout<<"reflector -> interaction() -> [";
				std::cout<<key_manipulate_mirror<<"] manipulate_mirror but ";
				std::cout<<"reflector calibration image is not defined yet!";
				std::cout<<std::endl;
			}
		}
	}
	//==================================================================
	if(valid_user_input.compare(key_delete_current_mirror)==0){
		// delete current mirror from list
		pop_mirror
		(pointer_to_current_mirror_to_work_with->get_mirror_ID());
	}
	//==================================================================
	if(valid_user_input.compare(key_choose_mirror_ID)==0){	
		uint user_chosen_mirror_ID;
		user_chosen_mirror_ID = 
		(uint)ask_user_for_integer("Enter mirror ID",0,1E6);
		
		if(choose_mirror_ID_to_work_with(user_chosen_mirror_ID)){
			// the mirror does exist already
		}else{
			// the mirror does not exist yet and is therefore created
			add_mirror(user_chosen_mirror_ID);
			choose_mirror_ID_to_work_with(user_chosen_mirror_ID);
		}
	}
	//==================================================================	
	if(valid_user_input.compare(key_quit)==0){	
		//==========================================================
		// chose to quit mirror manipulation
		//==========================================================
		user_wants_to_quit_mirror_manipulation = true;
	}
	//==================================================================
	update_status_prompt(get_reflector_prompt());
	}while(user_wants_to_quit_mirror_manipulation == false);	
}
//======================================================================
void set_reflector_name(std::string new_reflector_name){
	reflector_name = new_reflector_name;
	reflector_configuration_file_name =  new_reflector_name;
	reflector_configuration_file_name += ".cfg";
}	
//======================================================================
bool set_reflector_calibration_image(std::string image_file_name){
	if(reflector_calibration_image.load_image(image_file_name)){
		if(verbosity){
			std::cout<<"reflector -> ";
			std::cout<<"set_reflector_calibration_image() -> ";
			std::cout<<"successful: ";
			std::cout<<reflector_calibration_image.get_file_name()<<std::endl;
		}
		name_of_calibration_image = image_file_name;
		reflector_calibration_image_with_polygons = 
		reflector_calibration_image;
		return true;
	}else{
		name_of_calibration_image = "failed to load image!";
		
		reflector_calibration_image_with_polygons.image_initialized = 
		reflector_calibration_image.image_initialized;
		
		std::cout<<"reflector -> ";
		std::cout<<"set_reflector_calibration_image() -> ";
		std::cout<<"failed to load: ";
		std::cout<<image_file_name<<std::endl;
		return false;
	}
}
//======================================================================
void disp_status(){
	std::cout<<get_reflector_prompt();
}
//======================================================================
bool read_configuration(
std::string file_name_of_reflector_configuration_to_load){
	Config reflector_configuration_handle;
	//==================================================================
	// Read the file. If there is an error, report it and exit.
	//==================================================================
	file_name_of_reflector_configuration_to_load +=
	file_name_extension_for_configuration_file;
	std::cout<<"reflector -> read_configuration() -> ";
	std::cout<<file_name_of_reflector_configuration_to_load<<std::endl;
	
	try
	{
		reflector_configuration_handle.
		readFile(file_name_of_reflector_configuration_to_load.c_str());
	}
	catch(const FileIOException &fioex)
	{
		std::cout << "I/O error while reading file." << std::endl;
		return(EXIT_FAILURE);
	}
	catch(const ParseException &pex)
	{
		std::cout << "Parse error at " << pex.getFile() << ":";
		std::cout << pex.getLine() << " - " << pex.getError() << std::endl;
		return(EXIT_FAILURE);
	}
	//==================================================================
	// get data
	//==================================================================
	const Setting& root = reflector_configuration_handle.getRoot();
	
	bool reading_is_valid = true;
	
	// Output a list of all books in the inventory.
	try
	{
		const Setting &Reflector = root[key_reflector];
		const Setting &Mirrors = Reflector[key_list_of_mirrors];
		
		int number_of_mirrors_in_file = Mirrors.getLength();
		if(verbosity){
			std::cout<<"reflector -> read_configuration() -> There are ";
			std::cout<<number_of_mirrors_in_file;
			std::cout<<" mirror(s) to be loaded."<<std::endl;
		}
		
		
		// delete all old reflector information
		initialize_default_reflector();
	//read in new information
	
	//==================================================================
	std::string new_reflector_name;
	if( Reflector.lookupValue(key_reflector_name, new_reflector_name) ){
		set_reflector_name(new_reflector_name);
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_name<<" is set to "<<reflector_name<<std::endl;
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_name<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	if( Reflector.lookupValue
	(key_reflector_calibration_image, name_of_calibration_image) 	){
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_calibration_image<<" is set to ";
		std::cout<<name_of_calibration_image<<std::endl;
		
		if(set_reflector_calibration_image(name_of_calibration_image)){
			std::cout<<"reflector -> read_configuration() -> ";
			std::cout<<key_reflector_calibration_image;
			std::cout<<" loading complete."<<std::endl;
		}else{
			std::cout<<"reflector -> read_configuration() -> ";
			std::cout<<key_reflector_calibration_image;
			std::cout<<" failed to load sccan_image."<<std::endl;
			reading_is_valid = false;
		}
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_reflector_calibration_image<<" fail."<<std::endl;
		reading_is_valid = false;
	}
	//==================================================================
	double new_tripod_radius_in_m;
	if( Reflector.lookupValue
	(key_tripod_radius_in_m, new_tripod_radius_in_m) 	){
		tripod_radius_in_m = new_tripod_radius_in_m;
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_tripod_radius_in_m<<" set to ";
		std::cout<<tripod_radius_in_m<<std::endl;
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_tripod_radius_in_m<<" fail."<<std::endl;
		reading_is_valid = false;
	}	
	//==================================================================
	double new_pitch_of_bolt_in_m_per_revolution;	
	if( Reflector.lookupValue
	(key_pitch_of_bolt_in_m_per_revolution, 
	new_pitch_of_bolt_in_m_per_revolution) ){
		pitch_of_bolt_in_m_per_revolution = 
		new_pitch_of_bolt_in_m_per_revolution;
		
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_pitch_of_bolt_in_m_per_revolution<<" set to ";
		std::cout<<pitch_of_bolt_in_m_per_revolution<<std::endl;
	}else{
		std::cout<<"reflector -> read_configuration() -> ";
		std::cout<<key_pitch_of_bolt_in_m_per_revolution<<" fail."<<std::endl;
		reading_is_valid = false;
	}		
	//==================================================================
	for(
	int mirror_itterator = 0;
	mirror_itterator<number_of_mirrors_in_file;
	mirror_itterator++){
		Setting &Mirror_file_handle = Mirrors[mirror_itterator];
		
		mirror *pointer_to_a_new_mirror_read_form_file;
		pointer_to_a_new_mirror_read_form_file = 
		new mirror(0,verbosity);
		
		if(pointer_to_a_new_mirror_read_form_file->
		import_configuration(&Mirror_file_handle))
		{
			if(verbosity){
				std::cout<<"reflector -> read_configuration() -> ";
				std::cout<<"mirror "<<(mirror_itterator+1)<<" of ";
				std::cout<<number_of_mirrors_in_file;
				std::cout<<" has been loaded successful."<<std::endl;
			}	
	
			//==========================================================
			// update mirror masks
			//==========================================================
			pointer_to_a_new_mirror_read_form_file->
			initialize_list_of_points_inside_mirror_polygon(
			reflector_calibration_image_with_polygons.image_matrix.
			size());
			
			list_of_pointers_to_mirrors.
			push_back(pointer_to_a_new_mirror_read_form_file);
				
		}else{
			reading_is_valid  = false;
			std::cout<<"reflector -> read_configuration() -> ";
			std::cout<<"mirror "<<(mirror_itterator+1)<<" of ";
			std::cout<<number_of_mirrors_in_file;
			std::cout<<" failed to load from file!"<<std::endl;
		}
	}
	
	}
	catch(const SettingNotFoundException &nfex)
	{
		std::cout<<"reflector -> read_configuration() ->";
		std::cout<<"loading failed! Settings not found!"<<std::endl;
		reading_is_valid  = false;
	}
	

	
	return reading_is_valid;
}
//======================================================================
bool write_configuration_to_disk(){
	
	Config 	reflector_configuration_handle;
	Setting &root = reflector_configuration_handle.getRoot();

	// add reflector
	if(! root.exists(key_reflector))
    root.add(key_reflector, Setting::TypeGroup);

	Setting &Reflector = root[key_reflector];
	
	Reflector.add(key_reflector_name,Setting::TypeString)=
	reflector_name;
	
	Reflector.add
	(key_reflector_calibration_image, Setting::TypeString) = 
	name_of_calibration_image;
	
	Reflector.add(key_tripod_radius_in_m, Setting::TypeFloat) = 
	tripod_radius_in_m;

	Reflector.add
	(key_pitch_of_bolt_in_m_per_revolution, Setting::TypeFloat) = 
	pitch_of_bolt_in_m_per_revolution;
	
	// add mirrors

	if(! Reflector.exists(key_list_of_mirrors))
    Reflector.add(key_list_of_mirrors, Setting::TypeList);

	Setting &Mirrors = Reflector[key_list_of_mirrors];

	for(int mirror_itterator = 0;
	mirror_itterator<list_of_pointers_to_mirrors.size();
	mirror_itterator++)
	{
		// Create the new mirror entry.
		Setting &Mirror = Mirrors.add(Setting::TypeGroup);

		list_of_pointers_to_mirrors.
		at(mirror_itterator)->export_configuration_setting(&Mirror);
	}
	// Write out the new configuration.
	try
	{
		reflector_configuration_handle.
		writeFile(reflector_configuration_file_name.c_str());
		std::cout << "reflector -> ";
		std::cout << "write_configuration_to_disk() -> ";
		std::cout << "successfull: ";
		std::cout << reflector_configuration_file_name << std::endl;
	}
	catch(const FileIOException &fioex)
	{
		std::cout << "reflector -> ";
		std::cout << "write_configuration_to_disk() -> ";
		std::cerr << "I/O error while writing file: " ;
		std::cerr << reflector_configuration_file_name << std::endl;
		return(false);
	}
	return(true);	
}
//======================================================================
bool save_calibration_image_with_mirror_polygons(){
	
	std::string calibration_image_with_polygons_filename = 
	reflector_name+"_mirror_polygons.png";	
	
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"save_calibration_image_with_mirror_polygons() -> ";
		std::cout<<calibration_image_with_polygons_filename;
	}

	update_calibration_image_with_polygons(false);
	reflector_calibration_image_with_polygons.save_simple_image(
	calibration_image_with_polygons_filename);
	
	if(verbosity){
		std::cout<<" has been saved"<<std::endl;
	}
	
}
//======================================================================
std::string get_reflector_prompt(){
	std::stringstream out, info;
	out<<"|"<<std::endl;
	
	// reflector name
	info.str("");
	info<<reflector_name;
	out<<make_nice_line_with_dots("| reflector name",info.str());
	
	// number of mirrors
	info.str("");
	info<<list_of_pointers_to_mirrors.size();
	out<<make_nice_line_with_dots("| number of mirrors",info.str());
	
	// list of mirrors
	info.str("");
	if(list_of_pointers_to_mirrors.size()>0){
		for(
		int mirror_itterator=0;
		mirror_itterator<list_of_pointers_to_mirrors.size();
		mirror_itterator++)
		{
			info<<list_of_pointers_to_mirrors.at(mirror_itterator)->
			get_mirror_ID()<<", ";
		}
	}else{
		info<<"no mirrors on reflector";
	}
	out<<make_nice_line_with_dots("| mirror IDs:",info.str());
	
	// current mirror ID to work with
	info.str("");
	if(pointer_to_current_mirror_to_work_with==NULL){
		info<<"has not been set yet";	
	}else{
		info<<pointer_to_current_mirror_to_work_with->
		get_mirror_ID();
	}
	out<<make_nice_line_with_dots
	("| current mirror ID to work with",info.str());
	
	out<<"|"<<std::endl;
	
	// reflector calibration sccan_image
	info.str("");
	if(reflector_calibration_image.get_init_status()){
		info<<name_of_calibration_image;
	}else{
		info<<"has not been set yet";
	}
	out<<make_nice_line_with_dots
	("| reflector calibration sccan_image",info.str());
	
	out<<"|"<<std::endl;
	
	// mirror tripod radius
	info.str("");
	info<<tripod_radius_in_m<<"m";
	out<<make_nice_line_with_dots("| mirror tripod radius",info.str());
		
	// pitch of mirror bolts
	info.str("");
	info<<pitch_of_bolt_in_m_per_revolution<<"m/revolution";
	out<<make_nice_line_with_dots("| pitch of mirror bolts",info.str());
	
	out<<"|";
	
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
std::string get_reflector_name(){
	return reflector_name;
}
//======================================================================
// mirror polygon manipulation 
//======================================================================
void update_calibration_image_with_polygons(bool highlight){
	
	reflector_calibration_image_with_polygons=
	reflector_calibration_image;	
	
	uint mirror_ID_of_mirror_to_highlight;
	if(pointer_to_current_mirror_to_work_with == 0){
		mirror_ID_of_mirror_to_highlight = 0;
	}else{
		mirror_ID_of_mirror_to_highlight = 
		pointer_to_current_mirror_to_work_with->get_mirror_ID();
	}
	
	if(!highlight)
	mirror_ID_of_mirror_to_highlight = 0;
	
	// to normalize zhe lines and text fonts to be drawed the image size
	// is taken into account
	int number_of_pixels_of_image = 
	reflector_calibration_image_with_polygons.get_number_of_pixels();
	
	double mean_number_of_image_lines = 
	sqrt((double)number_of_pixels_of_image);
	
	double line_thickness_per_image_line = 2.0/1e3;
	double circle_radius_in_pixel_per_image_line = 5.0/1e3;
	double circle_thickness_in_pixel_per_image_line = 2.0/1e3;
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
	//==================================================================
	// for each mirror polygon group
	//==================================================================
	for(	int mirror_itterator = 0;
			mirror_itterator<list_of_pointers_to_mirrors.size();
			mirror_itterator++)
	{
		int number_of_points = 
		list_of_pointers_to_mirrors.at(mirror_itterator)->
		list_of_points_defining_mirror_polygon.size();
		
		if(number_of_points==0){
			if(verbosity){
				std::cout<<"reflector -> ";
				std::cout<<"update_calibration_image_with_polygons -> ";
				std::cout<<"No polygon of mirror ID:";
				std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()<<std::endl;
			}
		}else{
			if(verbosity){
				std::cout<<"reflector -> ";
				std::cout<<"update_calibration_image_with_polygons -> ";
				std::cout<<"Draw polygon of mirror ID:";
				std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()<<std::endl;
			}
			
			// reset mirror ID location
			location_of_mirror_ID.x=0;
			location_of_mirror_ID.y=0;
			//==========================================================
			// for each line to draw
			//==========================================================
			for(int point_itterator=0; 
				point_itterator<number_of_points;
				point_itterator++)
			{
					// init start point of line
					cv::Point start_point = 
					list_of_pointers_to_mirrors.at(mirror_itterator)->
					list_of_points_defining_mirror_polygon.
					at(point_itterator);
					
					// init end point of line
					cv::Point end_point;
					if((point_itterator+1) == 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.size())
					{
						end_point = 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.at(0);
					}else{
						end_point = 
						list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						list_of_points_defining_mirror_polygon.
						at(point_itterator+1);
					}
					// draw line
					if(verbosity){
						std::cout<<"reflector -> ";
						std::cout<<"update_calibration_image_with_polygons";
						std::cout<<" -> ";
						std::cout<<"draw line "<<point_itterator+1;
						std::cout<<": ("<<start_point.x<<"|"<<start_point.y;
						std::cout<<")->("<<end_point.x;
						std::cout<<"|"<<end_point.y<<")"<<std::endl;
					}
					if(	list_of_pointers_to_mirrors.
						at(mirror_itterator)->
						get_mirror_ID()
						==
						mirror_ID_of_mirror_to_highlight
					)
					{
						//colour highlighted
						//std::cout<<"highlighted !!!"<<std::endl;
						cv::line(
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						end_point,
						cv::Scalar( 0, 255, 0),//Blue,Green,Red
						line_thickness,
						line_Type 
						);
					}else{
						//colour default
						cv::line(
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						end_point,
						cv::Scalar( 0, 0, 255),//Blue,Green,Red
						line_thickness,
						line_Type 
						);
					}
					
					// calculate mirror ID location
					location_of_mirror_ID = location_of_mirror_ID +
					start_point;
					
					// draw a filled circle
					if(	list_of_pointers_to_mirrors.
						at(mirror_itterator)->get_mirror_ID()
						==
						mirror_ID_of_mirror_to_highlight
					)
					{
						//colour highlighted
						cv::circle( 
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						circle_radius_in_pixel,
						cv::Scalar( 128, 255, 0 ),
						circle_thickness_in_pixel,
						circle_lineType );
					}else{
						//colour default
						cv::circle( 
						reflector_calibration_image_with_polygons.
						image_matrix,
						start_point,
						circle_radius_in_pixel,
						cv::Scalar( 128, 0, 255 ),
						circle_thickness_in_pixel,
						circle_lineType );
					}
				}
		
			// draw mirror ID
			// calculate mirror ID location
			location_of_mirror_ID.x = 
			(location_of_mirror_ID.x)/number_of_points;
			
			location_of_mirror_ID.y = 
			(location_of_mirror_ID.y)/number_of_points;
			
			//create text ID to display
			std::stringstream text_to_put_in_image;
			text_to_put_in_image<<"ID";
			text_to_put_in_image<<list_of_pointers_to_mirrors.
			at(mirror_itterator)->get_mirror_ID();
			
			// put text to sccan_image
			if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
				get_mirror_ID()
				==
				mirror_ID_of_mirror_to_highlight)
			{
				//colour highlighted
				cv::putText(
				reflector_calibration_image_with_polygons.
				image_matrix,
				text_to_put_in_image.str(),
				location_of_mirror_ID,
				text_fontFace,// fontFace
				text_fontScale,// fontScale
				cv::Scalar( 64, 255, 64 ),//Blue,Green,Red
				text_thickness,
				8
				);
			}else{
				//colour default
				//colour highlighted
				cv::putText(
				reflector_calibration_image_with_polygons.
				image_matrix,
				text_to_put_in_image.str(),
				location_of_mirror_ID,
				text_fontFace,// fontFace
				text_fontScale,// fontScale
				cv::Scalar( 64, 64, 255 ),//Blue,Green,Red
				text_thickness,
				8
				);
			}
		}
	}
	flag_calibration_image_with_polygons_created = true;
	
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"update_calibration_image_with_polygons -> ";
		std::cout<<"end"<<std::endl;	
	}
}
//======================================================================
void manipulate_mirror_polygon(){
	
	// draw all existing polygons
	update_calibration_image_with_polygons(true);
	
	// display usage
	disp_status();
	std::cout<<"Press [ESC] to end Mirror manipulation"<<std::endl;
	std::cout<<"Press [ d ] to erase the last point"<<std::endl;
	std::cout<<"Press [ left mouse button ] to create new point"<<std::endl;
	
	if(	flag_calibration_image_with_polygons_created ){
		//create window for mirror polygon manipulation
		cv::namedWindow(
			name_of_calibration_window_with_highlighted_poygons.c_str(),
			CV_WINDOW_NORMAL 
		);
		
		// show sccan_image for mirror polygon manipulation
		cv::imshow( 
			name_of_calibration_window_with_highlighted_poygons.c_str(),
			reflector_calibration_image_with_polygons.image_matrix
		);  
		
		// mouse callback
		// on left mose click a new polygon point is added to the 
		// list
		cv::setMouseCallback(
			name_of_calibration_window_with_highlighted_poygons.c_str(),
			onMouse, 0);
		
		//wait for command lin input
		int user_input_key;
		do{
			user_input_key = cv::waitKey(0); 
			
			switch( (char)user_input_key ){
				case 'd':
					pop_point_of_polygon_point_list_of_mirror();
					//std::cout<<"destroy last point"<<std::endl;
					// destroy the last polygon element
				break;
	//~ default:
	//~ std::cout<<"Press [ESC] to end Mirror manipulation"<<std::endl;
	//~ std::cout<<"Press [ d ] to erase the last point"<<std::endl;
	//~ std::cout<<"Press [ left mouse button ] to create new point"<<std::endl;
			}
		}while( (user_input_key & 255) != 27 ); // ESC key to exit
		
		//destroy window for sccan_image manipulation
		cv::destroyWindow(
			name_of_calibration_window_with_highlighted_poygons.c_str()
		);  
	}else{
		std::stringstream out;
		out<<"reflector -> manipulate_list_points_of_mirror() ->";
		out<<" the calibration sccan_image with polygons was not created!"<<std::endl;
		std::cout<<out.str();
	}
	//==================================================================
	// update mirror mask
	//==================================================================
	pointer_to_current_mirror_to_work_with->
	initialize_list_of_points_inside_mirror_polygon(
	reflector_calibration_image_with_polygons.image_matrix.size()
	);
}	
//======================================================================
static void onMouse(int event, int x, int y, int flags, void *param){
		if( event != cv::EVENT_LBUTTONDOWN )
		return;
		// MUST USE GLOBAL VARIABLE config
		onMouse2(event,x,y,flags,param);
		
}
//======================================================================
void add_point_to_polygon_point_list_of_mirror(int pixel_x, int pixel_y){

	cv::Point point_to_add;
	point_to_add.x = pixel_x;
	point_to_add.y = pixel_y;
	
	pointer_to_current_mirror_to_work_with->
	list_of_points_defining_mirror_polygon.push_back(point_to_add);
	
	// update sccan_image
	update_calibration_image_with_polygons(true);
	
	// update display
	cv::imshow( 
		name_of_calibration_window_with_highlighted_poygons,
		reflector_calibration_image_with_polygons.image_matrix
	);
	if(verbosity){
		std::cout<<"reflector -> ";
		std::cout<<"add_point_to_polygon_point_list_of_mirror -> ";
		std::cout<<"add point ("<<pixel_x<<"|"<<pixel_y<<") ";
		std::cout<<"to mirror polygon"<<std::endl;
	}
}
//======================================================================
void pop_point_of_polygon_point_list_of_mirror(){
		
	if(	pointer_to_current_mirror_to_work_with->
		list_of_points_defining_mirror_polygon.size() > 0)
	{
		// delete last point
		pointer_to_current_mirror_to_work_with->
		list_of_points_defining_mirror_polygon.pop_back();
		
		// update sccan_image
		update_calibration_image_with_polygons(true);
		
		// update display
		cv::imshow(
			name_of_calibration_window_with_highlighted_poygons,
			reflector_calibration_image_with_polygons.image_matrix
		);
			
		if(verbosity){
			std::cout<<"reflector -> ";
			std::cout<<"pop_point_of_polygon_point_list_of_mirror -> ";
			std::cout<<"pop last point"<<std::endl;
		}
	}else{
		if(verbosity)
		{
		std::cout<<"reflector -> ";
		std::cout<<"pop_point_of_polygon_point_list_of_mirror -> ";
		std::cout<<"no point to pop"<<std::endl;
		}
	} 
}
//======================================================================
void toggle_verbosity(){
	verbosity = !verbosity;
	
	reflector_calibration_image.
	toggle_verbosity(verbosity);
	
	reflector_calibration_image_with_polygons.
	toggle_verbosity(verbosity);
	
}
//======================================================================
// list of mirrors
//======================================================================
std::vector<mirror*> list_of_pointers_to_mirrors;
mirror 			*pointer_to_current_mirror_to_work_with;
//======================================================================
void initialize_mirror_list(){
	pointer_to_current_mirror_to_work_with = NULL;
	list_of_pointers_to_mirrors.clear();
}
//======================================================================
bool choose_mirror_ID_to_work_with(uint mirror_ID_to_work_with){
	bool mirror_found = false;
	
	for(int mirror_itterator=0;
	mirror_itterator<list_of_pointers_to_mirrors.size();
	mirror_itterator++)
	{
		if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
		get_mirror_ID() == mirror_ID_to_work_with)
		{
			mirror_found = true;
			pointer_to_current_mirror_to_work_with = 
			list_of_pointers_to_mirrors.at(mirror_itterator);
		}
	}
	if(verbosity){
		std::cout<<"mirror_list -> choose_mirror_ID_to_work_with() -> ";
		std::cout<<"ID "<<mirror_ID_to_work_with<<" ";
		if(mirror_found){std::cout<<"was set to be manipulated.";}else
		{std::cout<<"was not found in mirror list!";}
		std::cout<<std::endl;
	}
	return mirror_found;	
}
//======================================================================
bool add_mirror(uint new_mirror_ID){
	
	mirror *pointer_to_new_mirror;
	pointer_to_new_mirror = new mirror(0,verbosity);
	pointer_to_new_mirror->set_mirror_ID(new_mirror_ID);
	
	if(choose_mirror_ID_to_work_with
	(pointer_to_new_mirror->get_mirror_ID())){
		// mirror already in list
		std::cout<<"mirror_list -> add_mirror() -> mirror ID ";
		std::cout<<pointer_to_new_mirror->get_mirror_ID();
		std::cout<<" already in list!"<<std::endl;
		return false;
	}else{
		// a new mirror
		list_of_pointers_to_mirrors.push_back(pointer_to_new_mirror);
		if(list_of_pointers_to_mirrors.size()==1)
		{
			pointer_to_current_mirror_to_work_with = 
			list_of_pointers_to_mirrors.at(0);
		}
		
		if(verbosity){
			std::cout<<"mirror_list -> add_mirror() -> mirror ID ";
			std::cout<<new_mirror_ID<<" was add to mirror list."<<std::endl;
		}
		return true;
	}
//======================================================================
}
//======================================================================
bool pop_mirror(uint old_mirror_ID){
	bool mirror_popped = false;
	// loop and destroy...
	for(int mirror_itterator=0;
	mirror_itterator<list_of_pointers_to_mirrors.size();
	mirror_itterator++)
	{
		if(	list_of_pointers_to_mirrors.at(mirror_itterator)->
		get_mirror_ID() == old_mirror_ID)
		{
			list_of_pointers_to_mirrors.
			erase(list_of_pointers_to_mirrors.begin()+mirror_itterator);
			mirror_popped = true;
		}
	}
	if(verbosity){
		std::cout<<"mirror_list -> pop_mirror() -> ";
		std::cout<<"ID "<<old_mirror_ID<<" ";
		if(mirror_popped){std::cout<<"was popped.";}else
		{std::cout<<"was not found in mirror list!";}
		std::cout<<std::endl;
	}
	return mirror_popped;		
}
//======================================================================
bool is_there_a_mirror_to_work_with(){
	if(pointer_to_current_mirror_to_work_with==NULL){
		if(verbosity){
			std::cout<<"mirror_list -> ";
			std::cout<<"is_there_a_mirror_to_work_with() -> ";
			std::cout<<"No its not."<<std::endl;
		}
		return false;
	}else{
		if(verbosity){
			std::cout<<"mirror_list -> ";
			std::cout<<"is_there_a_mirror_to_work_with() -> ";
			std::cout<<"Yes it is."<<std::endl;
		}
		return true;
	}
}
//======================================================================
int get_number_of_mirrors_on_reflector(){
	return list_of_pointers_to_mirrors.size();
}
//======================================================================
list_of_pixel_positions* get_pointer_to_list_of_Points_inside_mirror_polygon
(int mirror_itterator){
	
	if(mirror_itterator <list_of_pointers_to_mirrors.size()){
		
		if(verbosity){
			std::cout<<"mirror_list -> ";
			std::cout<<"get_pointer_to_list_of_Points_inside_mirror_polygon";
			std::cout<<" -> ";
			std::cout<<"return list of pixels of mirror ID";
			std::cout<<list_of_pointers_to_mirrors.at(mirror_itterator)->
			get_mirror_ID();
			std::cout<<std::endl;
		}
		
		return list_of_pointers_to_mirrors.at(mirror_itterator)->
		get_pointer_to_list_of_Points_inside_mirror_polygon();
	}else{
		std::cout<<"mirror_list -> ";
		std::cout<<"get_pointer_to_list_of_Points_inside_mirror_polygon";
		std::cout<<" -> ";
		std::cout<<"there is no mirror "<<mirror_itterator<<".";
		std::cout<<" There are/is only "<<list_of_pointers_to_mirrors.size();
		std::cout<<" mirror(s)!";
		std::cout<<std::endl;
		return NULL;
	}
}
//======================================================================
};
//======================================================================
reflector global_reflector_instance;
//======================================================================
static void onMouse2(int event, int x, int y, int flags, void *param){
	    if( event != cv::EVENT_LBUTTONDOWN )
	    return;
		// MUST USE GLOBAL VARIABLE 
		global_reflector_instance.
		add_point_to_polygon_point_list_of_mirror(x,y);
		
}
//======================================================================
class snapshot 					:public user_interaction{
public:
	bool 		snapshot_verbosity;
	uint 		snapshot_counter;
	bool 		flag_long_time_exposure;
	double 		snapshot_exposure_time_in_ms;
	std::string		snapshot_file_name;
	ueye_camera *pointer_to_snapshot_camera;
	std::vector<ueye_camera*> list_of_pointers_to_cameras;
//======================================================================
snapshot(){
	initialize();
	menu_name = "snapshot";
	//add_camera(NULL);
}
//======================================================================
snapshot(ueye_camera* ptr_to_snapshot_cam){
	add_camera(ptr_to_snapshot_cam);
	initialize();
}
//======================================================================
void initialize(){
	snapshot_verbosity = true;

	snapshot_counter = 1;
	snapshot_exposure_time_in_ms = 10;
	create_snapshot_file_name();
	flag_long_time_exposure = false;
	if(list_of_pointers_to_cameras.size()>0){
		pointer_to_snapshot_camera = 
		list_of_pointers_to_cameras.at(0);
	}
	
	if(snapshot_verbosity){
		std::cout<<"snapshot ->";
		std::cout<<"snapshot() ->";
		std::cout<<"initialize exp. time ";
		std::cout<<snapshot_exposure_time_in_ms<<"ms, ";
		std::cout<<"number of cameras ";
		std::cout<<list_of_pointers_to_cameras.size()<<std::endl;
	}
}
//======================================================================
void add_camera(ueye_camera* ptr_to_snapshot_cam){
	if(choose_camera(ptr_to_snapshot_cam->get_camera_ID())){
		//camera already in list
		std::cout<<"snapshot -> add_camera() -> camera ID ";
		std::cout<<ptr_to_snapshot_cam->get_camera_ID();
		std::cout<<" already in list!"<<std::endl;
	}else{
		// a new camera
		list_of_pointers_to_cameras.push_back(ptr_to_snapshot_cam);
		if(list_of_pointers_to_cameras.size()==1)
		{
			pointer_to_snapshot_camera = 
			list_of_pointers_to_cameras.at(0);
		}
	}
}
//======================================================================
void interaction(){

	std::string key_user_wants_to_acquire_image ="y";
	add_control(key_user_wants_to_acquire_image,"take a snapshot");
	
	std::string key_modify_exposure_time ="e";	
	add_control(key_modify_exposure_time,"manipulate exposure time");
	
	std::string key_modify_long_time_exposure ="l";	
	add_control(key_modify_long_time_exposure,
	"toggle long time exposure");
	
	std::string key_modify_camera_ID ="c";	
	add_control(key_modify_camera_ID,"choose camera ID");
	
	std::string key_take_exposure_time_series ="eS";
	add_control(key_take_exposure_time_series,
	"take exposure time series");
	
	std::string key_spectate_stars = "s";
	add_control(key_spectate_stars,"spectate stars");
	
	std::string key_user_wants_to_end_snapshot  ="back";
	add_control(key_user_wants_to_end_snapshot ,"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_interaction = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_snapshot_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();	
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_end_snapshot)==0){
		flag_user_wants_to_interaction = false;
	}
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_acquire_image)==0){	
		//==========================================================
		// chose to take another interaction_with_ueye_camera
		//==========================================================
		// increase snapshot counter for snapshot number in filename
		
		if(pointer_to_snapshot_camera->
		acquire_image(&snapshot_exposure_time_in_ms)){
			// sccan_image acquisition was ok
			pointer_to_snapshot_camera->
			export_latest_image(snapshot_file_name);
			
			pointer_to_snapshot_camera->disp_latest_image();
		}else{
			// sccan_image acquisition failed
		}
		snapshot_counter++;
		create_snapshot_file_name();
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_exposure_time)==0){
		//==========================================================
		// user chose to adjust exposure time
		//==========================================================
		snapshot_exposure_time_in_ms = ask_user_for_non_zero_float
		("Enter new exposure time [ms]",0.001,60000);
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_long_time_exposure)==0){
		if(flag_long_time_exposure==true){
			flag_long_time_exposure = false;
		}else{
			flag_long_time_exposure = true;
		}	
		pointer_to_snapshot_camera->
		long_time_exposure(flag_long_time_exposure);
	}	
	//==================================================================
	if(valid_user_input.compare(key_take_exposure_time_series)==0){

		int user_input_of_number_of_exposure_time_itterations =
		ask_user_for_integer(
		"Enter number of exposure time itterations",
		0,100);

		take_exposure_time_series(
		user_input_of_number_of_exposure_time_itterations);
	
	}
	//==================================================================	
	if(valid_user_input.compare(key_spectate_stars)==0){

		double star_detection_threshold_in_sigmas = 
		ask_user_for_non_zero_float(
		"Enter star detection threshold in sigmas",
		0.1,
		1000.0
		);
		
		double star_detection_radius_in_degrees = 
		ask_user_for_non_zero_float(
		"Enter star detection radius in degrees",
		0.001,
		180.0
		);
		
		spectate_stars(
		star_detection_threshold_in_sigmas,
		star_detection_radius_in_degrees
		);
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_camera_ID)==0){
		
		std::vector<int> list_of_valid_camera_IDs;
		for(int cam_itt=0;
		cam_itt<list_of_pointers_to_cameras.size();
		cam_itt++)
		{
			list_of_valid_camera_IDs.push_back(
			list_of_pointers_to_cameras.at(cam_itt)->get_camera_ID()
			);
		}
		uint user_chosen_camera_ID =ask_user_for_integer
		("Enter camera ID",&list_of_valid_camera_IDs);
		choose_camera(user_chosen_camera_ID);
		/*
		bool flag_user_input_camera_ID_is_valid = false;
		uint user_chosen_camera_ID;
		do{
			user_chosen_camera_ID = 
			(uint)ask_user_for_integer("Enter camera ID",0,256);
			
			if(choose_camera(user_chosen_camera_ID))
			{
				//camera was found
				flag_user_input_camera_ID_is_valid = true;
			}else{
				//camera is not in list
				out.str("");
				out<<"->"<<user_chosen_camera_ID<<"<- ";
				out<<"is not in camera ID list!"<<std::endl;
				std::cout<<out.str();	
			}
		}while( !flag_user_input_camera_ID_is_valid  );
		*/
	}
	//==================================================================

}while(flag_user_wants_to_interaction);
}
//======================================================================
void create_snapshot_file_name(){
	std::stringstream snapshot_file_name_stream;
	snapshot_file_name_stream<<global_time_stamp_manager_instance.
	get_current_time_stamp();
	snapshot_file_name_stream<<"_snapshot";
	snapshot_file_name_stream<<"_"<<snapshot_counter;
	snapshot_file_name = snapshot_file_name_stream.str();
}
//======================================================================
std::string get_snapshot_prompt(){
	int sys_return =  system("clear");
	std::stringstream out, info;
	
	out<<"| cameras (IDs): ";
	for(int camera_itterator=0;
	camera_itterator<list_of_pointers_to_cameras.size();
	camera_itterator++){
	out<<
	list_of_pointers_to_cameras.at(camera_itterator)->
	get_camera_ID()<<", ";
	}
	out<<std::endl;
	
	// current camera ID
	info.str("");
	info<<pointer_to_snapshot_camera->get_camera_ID();
	out<<make_nice_line_with_dots("| current camera ID",info.str());
	
	out<<"|"<<std::endl;
	
	// snapshot counter
	info.str("");
	info<<snapshot_counter;
	out<<make_nice_line_with_dots("| snapshot counter",info.str());

	// next snapshot
	info.str("");
	info<<snapshot_file_name;
	out<<make_nice_line_with_dots("| next snapshot",info.str());

	// next desired exposure time
	info.str("");
	info<<snapshot_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| next desired exposure time",info.str());

	// long time exposure feature
	info.str("");
	if(flag_long_time_exposure){info<<"active";}else{info<<"inactive";}
	out<<make_nice_line_with_dots
	("| long time exposure feature",info.str());
	
	// horizontal line 
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str(); 	
}
//======================================================================
bool choose_camera(uint camera_ID_to_use){
	bool camera_found = false;
	
	for(int camera_itterator=0;
	camera_itterator<list_of_pointers_to_cameras.size();
	camera_itterator++)
	{
		if(	list_of_pointers_to_cameras.at(camera_itterator)->
		get_camera_ID() == camera_ID_to_use)
		{
			camera_found = true;
			pointer_to_snapshot_camera = 
			list_of_pointers_to_cameras.at(camera_itterator);
		}
	}
	return camera_found;	
}
//======================================================================
void toggle_verbosity(){
	snapshot_verbosity = !snapshot_verbosity;
}
//======================================================================
bool verbosity_status(){
	return snapshot_verbosity;
}
//======================================================================
void take_exposure_time_series(int number_of_exposure_itterations){
	
	if(verbosity){
		std::cout<<"snapshot -> ";
		std::cout<<"take_exposure_time_series() -> taking ";
		std::cout<<number_of_exposure_itterations<<" images.";
		std::cout<<std::endl;	
	}
	
	if(number_of_exposure_itterations<=0)
	return;
	
	std::stringstream exposure_series_protocoll;
	
	exposure_series_protocoll<<" ____exposure_time_series_______"<<std::endl;
	
	for(
	int snapshot_itterator = 0;
	snapshot_itterator<number_of_exposure_itterations;
	snapshot_itterator++){
	
		double exposure_time_in_ms_for_specific_snapshot = 
		snapshot_exposure_time_in_ms*
		pow(2.0,(double)snapshot_itterator);
		
		if(verbosity){
			std::cout<<"snapshot -> ";
			std::cout<<"take_exposure_time_series() -> ";
			std::cout<<"exposure time ";
			std::cout<<exposure_time_in_ms_for_specific_snapshot<<"ms";
			std::cout<<std::endl;	
		}	
		
	
		pointer_to_snapshot_camera->
		acquire_image(&exposure_time_in_ms_for_specific_snapshot);
		
		simple_image snapshot_image_for_specific_exposure_time =
		pointer_to_snapshot_camera->get_latest_image();
		
		std::stringstream window_name_for_specific_snapshot;
		
		if(verbosity){
			window_name_for_specific_snapshot<<
			"snapshot, exposure time ";
			window_name_for_specific_snapshot<<
			exposure_time_in_ms_for_specific_snapshot;
			window_name_for_specific_snapshot<<"ms";
			/*
			snapshot_image_for_specific_exposure_time.
			disp_simple_image(
			window_name_for_specific_snapshot.str());
			*/
		}
		
		cv::Scalar mean_pixel_value;
		
		mean_pixel_value = cv::mean(
		snapshot_image_for_specific_exposure_time.image_matrix
		);
		
	exposure_series_protocoll<<"| "<<snapshot_itterator<<")\t";
	
	bool expose_longer;
	if(snapshot_image_for_specific_exposure_time.
	check_exposure_time(0.0,255,&expose_longer)){
		exposure_series_protocoll.precision(3);
		exposure_series_protocoll<<
		exposure_time_in_ms_for_specific_snapshot<<"ms\t";
		
		//exposure_series_protocoll<<"R "<<mean_pixel_value[2]<<"\t";
		//exposure_series_protocoll<<"G "<<mean_pixel_value[1]<<"\t";
		//exposure_series_protocoll<<"B "<<mean_pixel_value[0]<<"\t";
		exposure_series_protocoll<<"Mean "<<
		(double)
		(mean_pixel_value[2]+mean_pixel_value[1]+mean_pixel_value[0])/
		3.0;
		
	}else{
		exposure_series_protocoll<<"overflow";
	}
	
		if(verbosity){
			std::cout<<"snapshot -> ";
			std::cout<<"take_exposure_time_series() -> ";
			std::cout<<"Mean \tR "<<mean_pixel_value[2]<<" ";
			std::cout<<"\tG "<<mean_pixel_value[1]<<" ";
			std::cout<<"\tB "<<mean_pixel_value[0]<<" ";
			std::cout<<std::endl;
		}
		exposure_series_protocoll<<std::endl;
	}
	
	exposure_series_protocoll<<"|_______________________________"<<std::endl;
	
	std::cout<<exposure_series_protocoll.str();
}
//======================================================================
void spectate_stars(double threshold_in_sigmas_for_star_detection,
double radius_in_deg_of_one_sigma_region_for_star_detection){
	
	pointer_to_snapshot_camera->
	acquire_image(&snapshot_exposure_time_in_ms);
	
	sccan_image snapshot_image_to_spectate_stars =
	pointer_to_snapshot_camera->get_latest_image();
	
	std::vector<star_in_image> list_of_all_stars_found_in_snapshot = 
	snapshot_image_to_spectate_stars.find_stars(
	threshold_in_sigmas_for_star_detection,
	radius_in_deg_of_one_sigma_region_for_star_detection
	);
	
	if(verbosity){
		std::cout<<"snapshot -> ";
		std::cout<<"spectate_stars() -> ";
		std::cout<<list_of_all_stars_found_in_snapshot.size()<<" stars";
		std::cout<<"have been found.";
		std::cout<<std::endl;
	}
	
	simple_image stars_with_overlay = snapshot_image_to_spectate_stars.
	get_overlay_with_stars(
	list_of_all_stars_found_in_snapshot,
	cv::Scalar(255,128,128)
	);
	
	stars_with_overlay.disp_simple_image("star snapshot");
}
//======================================================================
};
//======================================================================
class sccan_point_pair_handler 	:public user_interaction{
	public:
	//std::vector<sccan_point_pair> list_of_sccan_point_pairs;
	
	double star_camera_last_good_exposure_time_in_ms;
	double reflector_camera_last_good_exposure_time_in_ms;
	
	ueye_camera 	*pointer_to_star_camera;
	ueye_camera 	*pointer_to_reflector_camera;
	bool			exposure_range_is_good;
	int 			sccan_run_number;
	//bool			verbosity_sccan_handler;

	double 	threshold_in_sigmas_for_star_detection;
	double 	star_recognition_one_sigma_radius_in_degrees;
	int		max_number_of_exposure_manipulations;
	
	// status
	simple_image 	star_position_indicator_image;
	simple_image	status_display;

	list_of_pixel_positions 	list_of_star_positions_of_all_points_in_directory;
	
	std::vector<std::string>	list_of_sccan_points_in_directory;
//======================================================================
sccan_point_pair_handler(){
	verbosity = false;
	
	list_of_star_positions_of_all_points_in_directory.
	toggle_verbosity(verbosity);
	
	star_position_indicator_image.
	toggle_verbosity(verbosity);
	
	status_display.toggle_verbosity(verbosity);

	if(verbosity){
		std::cout<<"sccan_point_pair_handler()"<<std::endl;
	}
	star_camera_last_good_exposure_time_in_ms = 10.0;
	reflector_camera_last_good_exposure_time_in_ms = 10.0;
	//list_of_sccan_point_pairs.clear();
	
	pointer_to_star_camera = NULL;
	pointer_to_reflector_camera = NULL;

	menu_name = "sccan point handler";
	update_list_of_runs_in_current_directory();
	
	sccan_run_number = list_of_sccan_points_in_directory.size()+1;
	
	threshold_in_sigmas_for_star_detection = 8.0;
	star_recognition_one_sigma_radius_in_degrees = 0.05;
	max_number_of_exposure_manipulations = 25;
	
	exposure_range_is_good = true;
	
	update_list_of_star_positions();
}
//======================================================================
void set_cameras(ueye_camera* star_camera,ueye_camera* reflector_camera){
	pointer_to_star_camera = star_camera;
	pointer_to_reflector_camera = reflector_camera;

	// only neccessary to figure out star camera resolution 
	double exposure = 10;
	sccan_image image_to_figure_out_image_size_of_star_camera;
	
	pointer_to_star_camera->acquire_image(&exposure);

	image_to_figure_out_image_size_of_star_camera = 
	pointer_to_star_camera->get_latest_image();
	
	// now overwrite image taken with star camera to zeros
	star_position_indicator_image.image_matrix = cv::Mat::zeros(
	image_to_figure_out_image_size_of_star_camera.image_matrix.rows,
	image_to_figure_out_image_size_of_star_camera.image_matrix.cols,
	CV_8UC3);
}
//======================================================================
void acquire_sccan_points(int number_of_sccan_points_to_acquire){
	//==================================================================
	// check number of sccan points
	//==================================================================
	if(number_of_sccan_points_to_acquire<1){
		std::cout<<"sccan_point_pair_handler -> acquire_sccan_points() -> ";
		std::cout<<"number of sccan images to qcquire must not be below 1!";
		std::cout<<std::endl;
		return;
	}

	//==================================================================
	// check cameras
	//==================================================================
	if(pointer_to_star_camera==NULL||pointer_to_reflector_camera==NULL){
		std::cout<<"sccan_point_pair_handler -> acquire_sccan_points() -> ";
		std::cout<<"either one or both cameras have not been set yet!";
		std::cout<<std::endl;
		return;		
	}
	
	exposure_range_is_good = true;
	int sccan_point_itterator = 0;
	
	//==================================================================
	// create star_position display
	//==================================================================
	update_star_position_indicator_image();
	std::string name_of_star_position_display = "star position display";
	std::string name_of_status_display = "status display";
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"acquire_sccan_points -> open status display ";
		std::cout<<name_of_star_position_display<<std::endl;
	}
	// Create a window for star position display.
	cv::namedWindow(name_of_star_position_display.c_str(),
	CV_WINDOW_NORMAL);
	//CV_WINDOW_AUTOSIZE
	
	cv::namedWindow(name_of_status_display.c_str(),
	CV_WINDOW_AUTOSIZE);
	//CV_WINDOW_NORMAL
	
	cv::imshow(
	name_of_star_position_display.c_str(),
	star_position_indicator_image.image_matrix
	);
	//cv::waitKey(1);
	
	do{
		//==============================================================
		//SCCAN run number
		//==============================================================
		sccan_point_itterator++;
		
		if(verbosity){
		std::cout<<"sccan_point_pair_handler -> acquire_sccan_points() -> ";
		std::cout<<"take sccan point number "<<sccan_run_number;
		std::cout<<std::endl;
		}

		sccan_point_pair current_sccan_point = 
		acquire_single_sccan_point(&exposure_range_is_good);
		
		update_status_display(&current_sccan_point);
		
		cv::imshow(
		name_of_status_display.c_str(),
		status_display.image_matrix
		);
		
		cv::imshow(
		name_of_star_position_display.c_str(),
		star_position_indicator_image.image_matrix
		);
		
		cv::waitKey(1);		
		
		if(exposure_range_is_good){
			// save sccan_point
			current_sccan_point.save_sccan_point_pair();
			
			// if a star was found, add it to list of stars
			if(current_sccan_point.a_single_star_has_been_found()){
				list_of_star_positions_of_all_points_in_directory.
				push_back
				(current_sccan_point.get_point_of_star_in_star_image());
				update_star_position_indicator_image();
				
				if(verbosity){
					std::cout<<"sccan_point_pair_handler -> ";
					std::cout<<"acquire_sccan_points -> ";
					std::cout<<"update status dispaly ";
					std::cout<<name_of_star_position_display<<std::endl;
				}
			}
			
			if(verbosity){		
				std::cout<<"sccan_point_pair_handler -> ";
				std::cout<<"acquire_sccan_points() -> ";
				std::cout<<"sccan_point number "<<sccan_run_number;
				std::cout<<" has been acquired.";
				std::cout<<std::endl;
				std::cout<<"sccan_point_pair_handler -> exposure times: ";
				std::cout<<"star ";
				std::cout<<star_camera_last_good_exposure_time_in_ms;
				std::cout<<"ms ";
				std::cout<<"reflector ";
				std::cout<<reflector_camera_last_good_exposure_time_in_ms;
				std::cout<<"ms";
				std::cout<<std::endl;
			}
		}else{
			std::cout<<"sccan_point_pair_handler -> ";
			std::cout<<"acquire_sccan_points() -> ";
			std::cout<<"exposure range is bad!";
			std::cout<<std::endl;			
		}
		
	}while( 
	(sccan_point_itterator<number_of_sccan_points_to_acquire) &&
	exposure_range_is_good);
	//==================================================================
	// close star_position display
	//==================================================================	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"acquire_sccan_points -> close status display ";
		std::cout<<name_of_star_position_display<<std::endl;
	}
	// close window
	cv::destroyWindow(name_of_star_position_display.c_str());  
	cv::destroyWindow(name_of_status_display.c_str());  
}
//======================================================================
sccan_point_pair acquire_single_sccan_point(
bool *pointer_to_exposure_range_is_good){
		
		sccan_point_pair current_sccan_point
		(sccan_run_number,verbosity);
		
		current_sccan_point.
		set_cameras(pointer_to_star_camera,pointer_to_reflector_camera);
		
		
		bool max_number_of_exposure_manipulations_has_been_exceeded 
		= false;
		
		// acquire sccan point
		current_sccan_point.acquire_sccan_images(
		&star_camera_last_good_exposure_time_in_ms,
		&reflector_camera_last_good_exposure_time_in_ms,
		max_number_of_exposure_manipulations,
		&max_number_of_exposure_manipulations_has_been_exceeded
		);
		
		if(max_number_of_exposure_manipulations_has_been_exceeded){
			std::cout<<"sccan_point_pair_handler -> ";
			std::cout<<"acquire_sccan_points() -> ";
			std::cout<<"desired exposure time can not be reached!"<<std::endl;
			*pointer_to_exposure_range_is_good = false;
		}else{
			sccan_run_number++;
		}
		
		// find bright star
		current_sccan_point.find_bright_star_in_star_image(
		threshold_in_sigmas_for_star_detection,
		star_recognition_one_sigma_radius_in_degrees
		);
		
	return current_sccan_point;
}
//======================================================================
std::string get_prompt(){
	std::stringstream out, info;
	// sccan run number
	info.str("");
	info<<sccan_run_number;
	out<<make_nice_line_with_dots
	("| sccan run number",info.str());
	
	// star camera exposure time
	info.str("");
	info<<star_camera_last_good_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| star camera exposure time",info.str());
	
	// reflector camera exposure time
	info.str("");
	info<<reflector_camera_last_good_exposure_time_in_ms<<"ms";
	out<<make_nice_line_with_dots
	("| reflector camera exposure time",info.str());

	// exposure range is good
	info.str("");
	if(exposure_range_is_good){info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| exposure range is good",info.str());
	
	out<<"|"<<std::endl;
	
	// number of sccan_points in current directory
	info.str("");
	info<<list_of_sccan_points_in_directory.size();
	out<<make_nice_line_with_dots
	("| number of sccan_points in current directory",info.str());
	
	out<<"|"<<std::endl;
	
	// threshold in sigmas for star detection
	info.str("");
	info<<threshold_in_sigmas_for_star_detection;
	out<<make_nice_line_with_dots
	("| threshold in sigmas for star detection",info.str());
	
	// star recognition radius in pixels
	info.str("");
	info<<star_recognition_one_sigma_radius_in_degrees<<"deg";
	out<<make_nice_line_with_dots
	("| star recognition radius one sigma",info.str());
	
	// max number of exposure itterations
	info.str("");
	info<<max_number_of_exposure_manipulations;
	out<<make_nice_line_with_dots
	("| max number of exposure itterations",info.str());
	
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
void interaction(){
	
	std::string key_user_wants_to_take_more_sccan_points  ="m";
	std::string key_show_list_of_sccan_points_in_current_directory ="s";
	std::string key_reset_exposure_times ="r";
	std::string key_show_star_position_indicator_image ="i";
	std::string key_modify_sigma_threshold ="sigma";
	std::string key_modify_recognition_radius ="radius";
	std::string key_user_wants_to_end_taking_sccan_points  ="back";
	
	add_control(key_user_wants_to_take_more_sccan_points,
	"aquire >N< additional sccan points");
	
	add_control
	(key_show_list_of_sccan_points_in_current_directory,
	"show list of sccan_points in current directory");

	add_control
	(key_show_star_position_indicator_image,
	"show star position indicator image");

	add_control
	(key_reset_exposure_times,
	"reset exposure times of cameras");
	
	add_control
	(key_modify_sigma_threshold,
	"modify sigma threshold");
	
	add_control
	(key_modify_recognition_radius,
	"modify recognition radius");	
	
	add_control
	(key_user_wants_to_end_taking_sccan_points,
	"back to main menu");
	
	std::string user_input;
	
	std::stringstream out;
	
	bool flag_user_wants_to_acquire_sccan_points = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_list_of_runs_in_current_directory();
	update_status_prompt(get_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare
	(key_user_wants_to_end_taking_sccan_points)==0){
		flag_user_wants_to_acquire_sccan_points = false;
	}
	//==================================================================
	if(valid_user_input.compare
	(key_user_wants_to_take_more_sccan_points)==0){
		int number_of_sccan_points_to_be_acquired = ask_user_for_integer
		("Enter number of sccan points to be acquired",0,1e4);
		acquire_sccan_points(number_of_sccan_points_to_be_acquired);
	}
	//==================================================================
	if(valid_user_input.compare(
	key_show_list_of_sccan_points_in_current_directory)==0){
		disp_list_of_sccan_points_in_current_directory();
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_sigma_threshold)==0){
		threshold_in_sigmas_for_star_detection =
		ask_user_for_non_zero_float(
		"Enter threshold for star detection in sigmas of image noise",
		0.0,100.0);
	}
	//==================================================================
	if(valid_user_input.compare(key_modify_recognition_radius)==0){
		star_recognition_one_sigma_radius_in_degrees =
		ask_user_for_non_zero_float
		("Enter new star recognition radius",0.0,180.0);
	}
	//==================================================================
	if(
	valid_user_input.compare(key_show_star_position_indicator_image)
	==0)
	{
		display_star_position_indicator_image();
	}
	//==================================================================
	if(valid_user_input.compare(key_reset_exposure_times)==0){
		reset_exposure_times();
	}	
	//==================================================================
}while(flag_user_wants_to_acquire_sccan_points);	
}
//======================================================================
void update_list_of_runs_in_current_directory(){
	
	list_of_sccan_points_in_directory.clear();
	// create list of all possible files in directory
	std::vector<std::string> list_of_all_files_in_directory = 
	get_list_of_files_in_directory("");
	
	for(int file_itterator=0;
	file_itterator<list_of_all_files_in_directory.size();
	file_itterator++){
	
		if(
		is_string_a_sccan_run_config(
		list_of_all_files_in_directory.at(file_itterator)
		)
		){
			// erase the file ending
			std::string filename_to_add =
			list_of_all_files_in_directory.at(file_itterator).
			substr(
			0,
			list_of_all_files_in_directory.at(file_itterator).
			find(".",0)
			);
			
			if(verbosity){
				std::cout<<"sccan_point_pair_handler -> ";
				std::cout<<"update_list_of_runs_in_current_directory() -> ";
				std::cout<<"sccan point: "<<filename_to_add<<std::endl;
			}
			list_of_sccan_points_in_directory.push_back
			(filename_to_add);
		}
	}
}
//======================================================================
void disp_list_of_sccan_points_in_current_directory(){
	std::stringstream out;
	out<<"list of "<<list_of_sccan_points_in_directory.size();
	out<<" sccan_points in current directory:"<<std::endl;

	for(int sccan_point_itterator=0;
	sccan_point_itterator<list_of_sccan_points_in_directory.size();
	sccan_point_itterator++){
	out<<list_of_sccan_points_in_directory.at(sccan_point_itterator);
	out<<std::endl;
	}
	std::cout<<out.str();
}
//======================================================================
bool is_string_a_sccan_run_config(std::string line){
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"is_string_a_sccan_run_config() -> line: ";
		std::cout<<line<<std::endl;
	}
	
	// use DEAs for the std::string control
	// find 

	enum TokenT{ 
		digit,
		point,
		underscore,
		char_c,
		char_f,
		char_g,
		char_s,
		char_a,
		char_n,
		ERR
	};
		   
	int GetState[][23] ={
	//dig point under	c	f	g	s	a	n
	{  1,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//0		d
	{  2,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//1		d
	{  3,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//2		d
	{  4,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//3		d
	{  5,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//4		d
	{  6,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//5		d
	{  7,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//6		d
	{  8,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//7		d
	{  -1,	-1,  9,		-1,	-1,	-1, -1,	-1,	-1 },	//8		_
	{  10,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//9		d
	{  11,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//10	d
	{  12,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//11	d
	{  13,	-1,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//12	d
	{  -1,	-1,	14,		-1,	-1,	-1, -1,	-1,	-1 },	//13	_	
	{  -1,	-1,	-1,		-1,	-1,	-1, 15,	-1,	-1 },	//14	s
	{  -1,	-1,	-1,		16,	-1,	-1, -1,	-1,	-1 },	//15	c
	{  -1,	-1,	-1,		17,	-1,	-1, -1,	-1,	-1 },	//16	c
	{  -1,	-1,	-1,		-1,	-1,	-1, -1,	18,	-1 },	//17	a
	{  -1,	-1,	-1,		-1,	-1,	-1, -1,	-1,	19 },	//18	n
	{  -1,	-1,	20,		-1,	-1,	-1, -1,	-1,	-1 },	//19	_	
	{  20,	21,	-1,		-1,	-1,	-1, -1,	-1,	-1 },	//20	d	
	{  -1,	-1,	-1,		22,	-1,	-1, -1,	-1,	-1 },	//21	.
	{  -1,	-1,	-1,		-1,	23,	-1, -1,	-1,	-1 },	//22	c
	{  -1,	-1,	-1,		-1,	-1,	24, -1,	-1,	-1 },	//23	f
													//24	g
	};
	//for( int i=0; i<11;i++){std::cout<<"("<<i<<")"<<GetState[26][i]<<", ";}
	//std::cout<<std::endl;
	std::string ID;
	std::string tilt_x;
	std::string tilt_y;
	std::string tilt_z;
	std::string str_vec2D_x;
	std::string str_vec2D_y;
	int int_vec2D_count = 0;
	
	
	int state = 0;
	int char_number = 0;
	//std::cout<<"str laenge: "<<line.size()<<std::endl;
	while(state != -1 && char_number<line.size()){
		TokenT token = ERR;
		char s = line.at( char_number);
		if(is_c(s)) token = char_c;
		if(is_f(s)) token = char_f;
		if(is_g(s)) token = char_g;
		if(is_s(s)) token = char_s;
		if(is_a(s)) token = char_a;
		if(is_n(s)) token = char_n;
		if(isdigit(s)) token = digit; 
		//std::cout<<char(s)<<"==digit"<<std::endl;}
		if(is_point(s)) token = point;
		if(is_underscore(s)) token = underscore;
		//std::cout<<"check"<<char_number<<": ";
		//std::cout<<char(s)<<" state: "<<state<<" token: "<<token<<std::endl;
		state = (token == ERR) ? :GetState[state][token];
		char_number ++; 	
	
	}
	if(state==24){return true;}else{return false;};
}
//======================================================================
void update_star_position_indicator_image(){
	//overwrite image to zeros
	star_position_indicator_image.image_matrix = cv::Mat::zeros(
	star_position_indicator_image.image_matrix.rows,
	star_position_indicator_image.image_matrix.cols,
	CV_8UC3);
	
	star_position_indicator_image = 
	star_position_indicator_image.get_overlay_with_stars(
	list_of_star_positions_of_all_points_in_directory,
	cv::Scalar(255,128,128)); //BGR
}
//======================================================================
void display_star_position_indicator_image(){
	update_star_position_indicator_image();
	star_position_indicator_image.disp_simple_image
	("star position indicator image");
}
//======================================================================
void update_list_of_star_positions(){
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_list_of_star_positions() -> ";
		std::cout<<"start";
		std::cout<<std::endl;
	}
	
	update_list_of_runs_in_current_directory();
	
	list_of_star_positions_of_all_points_in_directory.clear();
	
	for(int sccan_point_itterator = 0;
	sccan_point_itterator<list_of_sccan_points_in_directory.size();
	sccan_point_itterator++){
		
		sccan_point_pair sccan_point_to_check(0,verbosity);
		sccan_point_to_check.load_sccan_point_configuration(
		list_of_sccan_points_in_directory.at(sccan_point_itterator)
		);
		
		if(sccan_point_to_check.a_single_star_has_been_found()){
			list_of_star_positions_of_all_points_in_directory.
			push_back(
			sccan_point_to_check.get_point_of_star_in_star_image()
			);
		}
	}
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_list_of_star_positions() -> ";
		std::cout<<list_of_star_positions_of_all_points_in_directory.size();
		std::cout<<" star positions found in ";
		std::cout<<list_of_sccan_points_in_directory.size();
		std::cout<<" sccan_points in current directory.";
		std::cout<<std::endl;
	}
	
}
//======================================================================
void reset_exposure_times(){
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"reset_exposure_times() -> ";
		std::cout<<"set both exposure times to 10.0ms";
		std::cout<<std::endl;
	}
	star_camera_last_good_exposure_time_in_ms = 100.0;
	reflector_camera_last_good_exposure_time_in_ms = 100.0;
}
//======================================================================
void update_status_display
(sccan_point_pair *pointer_to_current_sccan_point){
	
	int number_of_lines_of_status_display_for_sccan_point_taking = 480;
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"start.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get star camera image
	//==================================================================
	
	simple_image resized_star_image_with_overlay = 
	pointer_to_current_sccan_point->get_resized_star_image_with_overlay
	(number_of_lines_of_status_display_for_sccan_point_taking);
	
	if(pointer_to_current_sccan_point->a_single_star_has_been_found()){
		// overlay with star direction
		std::stringstream star_direction_prompt;
		
		star_direction_prompt<<"x ";
		star_direction_prompt<<pointer_to_current_sccan_point->
		sccan_star.star_direction_relative_to_camera.
		get_x_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.1,0.9,0.25);
		
		star_direction_prompt.str("");
		star_direction_prompt<<"y ";
		star_direction_prompt<<pointer_to_current_sccan_point->
		sccan_star.star_direction_relative_to_camera.
		get_y_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.5,0.9,0.25);
	}

	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"got resized star image with overlay.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get reflector camera image
	//==================================================================

	simple_image resized_reflector_image_with_overlay =
	pointer_to_current_sccan_point->
	get_resized_reflector_image(
	number_of_lines_of_status_display_for_sccan_point_taking);
	
	pointer_to_current_sccan_point->
	get_reflector_image_exposure_time_stamp(
	&resized_reflector_image_with_overlay);
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"got resized reflector image with overlay.";
		std::cout<<std::endl;
	}	
	
	//==================================================================
	//concatenate matrices
	//==================================================================	
	
	status_display.horizontal_concatenate_with_delimiter_line(
	&resized_star_image_with_overlay,
	&resized_reflector_image_with_overlay);
	
	if(verbosity){
		std::cout<<"sccan_point_pair_handler -> ";
		std::cout<<"update_status_display() -> ";
		std::cout<<"end.";
		std::cout<<std::endl;
	}
}
//======================================================================
int get_number_of_sccan_points_in_current_directory(){
	return list_of_sccan_points_in_directory.size();
}
//======================================================================
std::string get_sccan_point_pair_name_at(
int position_of_sccan_point_pair_name){
	if(position_of_sccan_point_pair_name>
	list_of_sccan_points_in_directory.size()){
		return "";
	}else{
		return list_of_sccan_points_in_directory.
		at(position_of_sccan_point_pair_name);
	}
}
//======================================================================
void toggle_verbosity(){
	verbosity = !verbosity;
	
	list_of_star_positions_of_all_points_in_directory.
	toggle_verbosity(verbosity);
	
	star_position_indicator_image.
	toggle_verbosity(verbosity);
	
	status_display.
	toggle_verbosity(verbosity);
	
}
//======================================================================
private:
bool is_c(char c){return c=='c';}
bool is_f(char c){return c=='f';}
bool is_g(char c){return c=='g';}
bool is_s(char c){return c=='s';}
bool is_a(char c){return c=='a';}
bool is_n(char c){return c=='n';}
bool is_point(char c){return c=='.';}
bool is_underscore(char c){return c=='_';}
bool is_end_of_line(char c){return c=='\0';}
};
//======================================================================
class quick_align 				:public user_interaction{
public:
reflector* pointer_to_reflector;
sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;

std::string window_name_of_alignment_image;
simple_image alignment_image;

int height_of_alignment_image;
//======================================================================
quick_align(
reflector* new_pointer_to_reflector,
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler){
	//quick_align_verbosity = true;
	if(verbosity){
		std::cout<<"quick_align -> quick_align()"<<std::endl;
	}
	
	
	menu_name = "quick single mirror alignment";
	
	pointer_to_reflector = 
	new_pointer_to_reflector;
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	
	window_name_of_alignment_image = "quick alignment display";
	height_of_alignment_image = 480;
	alignment_image.toggle_verbosity(verbosity);
}
//======================================================================
void start_alignment(){
	if(verbosity){
		std::cout<<"quick_align -> start_alignment() -> start"<<std::endl;
	}	
	
	// check whether there is a mirror to work with or not
	if(pointer_to_reflector->is_there_a_mirror_to_work_with()){
		if(verbosity){
			std::cout<<"quick_align -> start_alignment() -> ";
			std::cout<<"working on mirror ID ";
			std::cout<<pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->get_mirror_ID();
			std::cout<<std::endl;
		}
		// run alignment
		run_alignment();
	}else{
		if(verbosity){
			std::cout<<"quick_align -> start_alignment() -> ";
			std::cout<<"there is no mirror set to work with!"<<std::endl;
		}		
	}
}
//======================================================================
void run_alignment(){
	display_help();
	
	int time_until_images_are_updated_in_ms = 1;
	int key_pressed_by_user=0;
	
	
	cv::namedWindow(
	window_name_of_alignment_image.c_str(),
	CV_WINDOW_AUTOSIZE 
	);
	
	while(key_pressed_by_user!=27) //27 is ESC key
	{
		update_alignment_window();
		
		key_pressed_by_user =
		cvWaitKey(time_until_images_are_updated_in_ms);

		switch(key_pressed_by_user){
			//~ case 'w': move_forward();
			//~ break;
			//~ case 's': move_backward();
			//~ break;
			//~ case 'a': move_left();
			//~ break;
			//~ case 'd': move_right();
			//~ break;
			//~ case '+': increase_FoV();
			//~ break;
			//~ case '-': decrease_FoV();
			//~ break;
			//~ case 'i' : look_up();
			//~ break;
			//~ case 'k' : look_down();
			//~ break;
			//~ case 'j' : look_left();
			//~ break;
			//~ case 'l' : look_right();
			//~ break;
			case 'h' : {
				display_help();
			};
			break;
			default: int i=1;
		}
	}
	cv::destroyWindow(window_name_of_alignment_image.c_str());
}
//======================================================================
void display_help(){
	std::cout<<"quick_align -> start_alignment() -> ";
	std::cout<<"help"<<std::endl;
}
//======================================================================
void update_alignment_window(){
	
	acquire_sccan_point_for_quick_alignment();
	
	cv::imshow(window_name_of_alignment_image.c_str(),
	alignment_image.image_matrix); 
}
//======================================================================
void acquire_sccan_point_for_quick_alignment(){

	bool exposure_range_is_good = true;
	sccan_point_pair current_sccan_point = 
	pointer_to_sccan_point_pair_handler->
	acquire_single_sccan_point(&exposure_range_is_good);
	
	update_alignment_window(current_sccan_point);
	
	if(exposure_range_is_good){
		if(verbosity){
			std::cout<<"quick_align -> ";
			std::cout<<"acquire_sccan_point_for_quick_alignment() -> ";
			std::cout<<"sccan point acquiered, exposure range is good.";
			std::cout<<std::endl;
		}	
	}else{
		std::cout<<"quick_align -> ";
		std::cout<<"acquire_sccan_point_for_quick_alignment() -> ";
		std::cout<<"sccan point acquiered, exposure range is bad!";
		std::cout<<std::endl;
	}
}
//======================================================================
void update_alignment_window(sccan_point_pair current_sccan_point){

	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"start.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get star camera image
	//==================================================================
	
	simple_image resized_star_image_with_overlay = 
	current_sccan_point.get_resized_star_image_with_overlay
	(height_of_alignment_image);
	resized_star_image_with_overlay.toggle_verbosity(verbosity);

	if(current_sccan_point.a_single_star_has_been_found()){
		// overlay with star direction
		std::stringstream star_direction_prompt;
		
		star_direction_prompt<<"x ";
		star_direction_prompt<<current_sccan_point.sccan_star.
		star_direction_relative_to_camera.
		get_x_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.1,0.9,0.25);
		
		star_direction_prompt.str("");
		star_direction_prompt<<"y ";
		star_direction_prompt<<current_sccan_point.sccan_star.
		star_direction_relative_to_camera.
		get_y_tilt_prompt_in_deg_min_sec();
				
		resized_star_image_with_overlay.text_overlay(
		star_direction_prompt.str(),0.5,0.9,0.25);
	}
	
	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"got resized star image with overlay.";
		std::cout<<std::endl;
	}
	//==================================================================	
	// get reflector camera image
	//==================================================================
	
	simple_image resized_reflector_image_with_overlay;
	resized_reflector_image_with_overlay.toggle_verbosity(verbosity);
	
	// calculate region of interest for specific mirror ID
	if(pointer_to_reflector->pointer_to_current_mirror_to_work_with 
	!= NULL){
		if(verbosity){
			std::cout<<"quick_align -> ";
			std::cout<<"update_alignment_window() -> ";
			std::cout<<"region of interest for ";	
			std::cout<<"mirror ID ";
			std::cout<<pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->get_mirror_ID();
			std::cout<<std::endl;	
		}	
			
			simple_image roi = 
			pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->
			highlight_mirror_in_reflector_image
			(&current_sccan_point.reflector_image);
			roi.toggle_verbosity(verbosity);
			
			double scaling = 1.0;
			
			resized_reflector_image_with_overlay=
			roi.get_resized_image(height_of_alignment_image,&scaling);
			
			// overlay with specific mirror ID
			std::stringstream mirror_ID_prompt_in_image;
			mirror_ID_prompt_in_image<<"mirror ID ";
			mirror_ID_prompt_in_image<<pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->get_mirror_ID();
			
			resized_reflector_image_with_overlay.text_overlay(
			mirror_ID_prompt_in_image.str(),0.1,0.15,1.0);
			
			// overlay with mean light flux
			double mean_light_flux_in_bulbs_of_mirror = 				
			current_sccan_point.reflector_image.
			mean_light_flux_in_bulbs_of_complex_image_region(
			pointer_to_reflector->
			pointer_to_current_mirror_to_work_with->
			get_pointer_to_list_of_Points_inside_mirror_polygon());
			
			std::stringstream light_flux_mirror_promot;
			light_flux_mirror_promot.precision(3);
			light_flux_mirror_promot<<"light flux mirror: ";
			light_flux_mirror_promot<<
			mean_light_flux_in_bulbs_of_mirror<<" bulbs";
			
			resized_reflector_image_with_overlay.text_overlay(
			light_flux_mirror_promot.str(),0.1,0.2,1.0);	
			
			// overlay normalized light flux
			if(current_sccan_point.a_single_star_has_been_found()){
				
				std::stringstream normalized_light_flux_mirror_promot;
				normalized_light_flux_mirror_promot.precision(3);
				normalized_light_flux_mirror_promot<<
				"normalized light flux: "<<
				mean_light_flux_in_bulbs_of_mirror/
				current_sccan_point.sccan_star.
				light_flux_of_star_in_bulbs<<" [1]";

				resized_reflector_image_with_overlay.text_overlay(
				normalized_light_flux_mirror_promot.str(),0.1,0.25,1.0);	
			}	
			
	}else{
		if(verbosity){
			std::cout<<"quick_align -> ";
			std::cout<<"update_alignment_window() -> ";	
			std::cout<<"mirror ID was not set yet!";
			std::cout<<std::endl;	
		}
		resized_reflector_image_with_overlay=
		current_sccan_point.get_resized_reflector_image(
		height_of_alignment_image);
		
		resized_reflector_image_with_overlay.text_overlay(
		"mirror ID has not been set!",0.1,0.15,1.0);
	}
	
	current_sccan_point.get_reflector_image_exposure_time_stamp(
	&resized_reflector_image_with_overlay);
	
	
	
	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"got resized reflector image with overlay.";
		std::cout<<std::endl;
	}	
	
	//==================================================================
	//concatenate matrices
	//==================================================================	
	
	alignment_image.horizontal_concatenate_with_delimiter_line(
	&resized_star_image_with_overlay,
	&resized_reflector_image_with_overlay);
	
	if(verbosity){
		std::cout<<"quick_align -> ";
		std::cout<<"update_alignment_window() -> ";
		std::cout<<"end.";
		std::cout<<std::endl;
	}
}
//======================================================================
std::string get_quick_alignment_prompt(){
	std::stringstream out, info;
	out<<pointer_to_sccan_point_pair_handler->get_prompt();
	
	out<<"|"<<std::endl;
	
	// Mirror ID to align
	info.str("");
	if(pointer_to_reflector->
	pointer_to_current_mirror_to_work_with==NULL){
		info<<"has not beem set yet";
	}else{
		info<<pointer_to_reflector->
		pointer_to_current_mirror_to_work_with->get_mirror_ID();
	}
	out<<make_nice_line_with_dots("| Mirror ID to align",info.str());
	
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str();
}
//======================================================================
void interaction(){
	
	std::string key_user_wants_to_satrt_quick_mirror_alignment  ="q";
	add_control(key_user_wants_to_satrt_quick_mirror_alignment,
	"start quick mirror alignment");	
	
	std::string key_user_wants_to_end_quick_mirror_alignment  ="back";
	add_control(key_user_wants_to_end_quick_mirror_alignment,
	"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_align_mirrors = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_quick_alignment_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	if(valid_user_input.
	compare(key_user_wants_to_end_quick_mirror_alignment)==0){
		flag_user_wants_to_align_mirrors = false;
	}
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_satrt_quick_mirror_alignment)==0){
		run_alignment();
	}	
	//==================================================================
	}while(flag_user_wants_to_align_mirrors);
}
//======================================================================
void toggle_verbosity(){
	verbosity = !verbosity;
	alignment_image.toggle_verbosity(verbosity);
}
//======================================================================
};
//======================================================================
class sccan_point_analysis 		:public user_interaction{
	public:
	bool 	sccan_point_analysis_verbosity;
	
	std::vector< std::vector<sccan_analysis_point*> > sccan_matrix;
	// dimension 1 sccan analysis points
	// dimension 2 mirrors
	//input
	sccan_point_pair_handler* pointer_to_sccan_point_pair_handler;
	reflector				* pointer_to_reflector;
//======================================================================
sccan_point_analysis(
sccan_point_pair_handler* new_pointer_to_sccan_point_pair_handler,
reflector* new_pointer_to_reflector){
	
	sccan_point_analysis_verbosity = true;
	
	if(sccan_point_analysis_verbosity){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"sccan_point_analysis()";
		std::cout<<std::endl;
	}
	
	menu_name = "sccan point analysis";
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	pointer_to_reflector = new_pointer_to_reflector;		
}	
//======================================================================
void fill_sccan_matrix(){
	
	sccan_matrix.clear();
	
	// update sccan point list of current directory
	pointer_to_sccan_point_pair_handler->
	update_list_of_runs_in_current_directory();

	//for all sccan points
	for(int sccan_point_itterator = 0;
	sccan_point_itterator<
	pointer_to_sccan_point_pair_handler->
	get_number_of_sccan_points_in_current_directory();
	sccan_point_itterator++)
	{
		sccan_point_pair current_sccan_point
		(0,sccan_point_analysis_verbosity);
		
		if(current_sccan_point.load_sccan_point_pair(
		pointer_to_sccan_point_pair_handler->
		get_sccan_point_pair_name_at(sccan_point_itterator)
		)){
			if(sccan_point_analysis_verbosity){
				std::cout<<"sccan_point_analysis -> ";
				std::cout<<"fill_sccan_matrix() -> ";
				std::cout<<"sccan point "<<(sccan_point_itterator+1);
				std::cout<<"/"<<	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				std::cout<<" loading successful.";
				std::cout<<std::endl;
			}			
		}else{
				std::cout<<"sccan_point_analysis -> ";
				std::cout<<"fill_sccan_matrix() -> ";
				std::cout<<"sccan point "<<(sccan_point_itterator+1);
				std::cout<<"/"<<	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				std::cout<<" loading failed.";
				std::cout<<std::endl;			
		}
		//==============================================================
		// check whether a single star was found or not 
		//==============================================================
		if(current_sccan_point.a_single_star_has_been_found()){ 
			// a single star in sccan point
			
			std::vector<sccan_analysis_point*> 
			list_of_sccan_analysis_points_for_specific_sccan_point;
			//==========================================================
			//for all mirrors on reflector
			//==========================================================
			for(int mirror_itterator = 0;
			mirror_itterator<
			pointer_to_reflector->get_number_of_mirrors_on_reflector();
			mirror_itterator++)
			{
				if(sccan_point_analysis_verbosity){
					std::cout<<"_____________________________________"<<std::endl;
					std::cout<<"sccan_point_analysis -> ";
					std::cout<<"fill_sccan_matrix() -> ";
					std::cout<<"sccan point "<<(sccan_point_itterator+1);
					std::cout<<"/"<<	
					pointer_to_sccan_point_pair_handler->
					get_number_of_sccan_points_in_current_directory();
					std::cout<<" mirror "<<(mirror_itterator+1);
					std::cout<<"/"<<	
					pointer_to_reflector->
					get_number_of_mirrors_on_reflector();
					std::cout<<std::endl;
				}
				
				sccan_analysis_point* 
				pointer_to_specific_sccan_analysis_point = 
				current_sccan_point.calculate_sccan_analysis_point(
				pointer_to_reflector->
				get_pointer_to_list_of_Points_inside_mirror_polygon
				(mirror_itterator),
				pointer_to_reflector->list_of_pointers_to_mirrors.
				at(mirror_itterator)->get_mirror_ID()
				);
				
				pointer_to_specific_sccan_analysis_point->
				toggle_verbosity(sccan_point_analysis_verbosity);
				
				list_of_sccan_analysis_points_for_specific_sccan_point.
				push_back(pointer_to_specific_sccan_analysis_point);
			}
			
			sccan_matrix.push_back
			(list_of_sccan_analysis_points_for_specific_sccan_point);
			
		}else{
			// no single star in sccan point
			if(sccan_point_analysis_verbosity){
				std::cout<<"sccan_point_analysis -> ";
				std::cout<<"fill_sccan_matrix() -> ";
				std::cout<<"sccan point "<<(sccan_point_itterator+1);
				std::cout<<"/"<<	
				pointer_to_sccan_point_pair_handler->
				get_number_of_sccan_points_in_current_directory();
				std::cout<<" no single star found!"<<std::endl;
				std::cout<<std::endl;
			}			
		}
	}
}
//======================================================================
std::string get_sccan_matrix_prompt(){
	std::stringstream out;
	
	int number_of_sccan_analysis_points =  sccan_matrix.size();
	if(number_of_sccan_analysis_points<1){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"get_sccan_matrix_prompt() -> ";
		std::cout<<"sccan matrix is empty."<<std::endl;
	}else{
		int number_of_mirrors =  sccan_matrix.at(0).size();
		
		if(number_of_mirrors<1){
			std::cout<<"sccan_point_analysis -> ";
			std::cout<<"get_sccan_matrix_prompt() -> ";
			std::cout<<"sccan matrix has no mirrors."<<std::endl;
		}
		//==============================================================
		//for all mirrors on reflector
		//==============================================================
		for(
		int mirror_itterator = 0;
		mirror_itterator<number_of_mirrors;
		mirror_itterator++)
		{	
			//==========================================================
			//for all sccan points
			//==========================================================
			for(
			int sccan_point_itterator = 0;
			sccan_point_itterator<number_of_sccan_analysis_points;
			sccan_point_itterator++)
			{
				out<<sccan_matrix.
				at(sccan_point_itterator).
				at(mirror_itterator)->get_info_prompt();
				out<<std::endl;
			}
		}
	}

	return out.str();
}
//======================================================================
void display_sccan_matrix_prompt(){
	std::cout<<std::endl;
	std::cout<<get_sccan_matrix_prompt();
}
//======================================================================
std::string get_analysis_prompt(){
	std::stringstream out;
	std::stringstream info;
	
	out<<"| analysis"<<std::endl;
	
	// sccan matrix
	info.str("");
	if(sccan_matrix.size()>0){
		info<<sccan_matrix.size()<<" sccan points times";
		info<<" "<<sccan_matrix.at(0).size()<<" mirrors";
	}else{
		info<<" empty";
	}
	out<<make_nice_line_with_dots
	("| sccan matrix: ",info.str());	
	
	// reflector name
	out<<make_nice_line_with_dots
	("| reflector name",pointer_to_reflector->get_reflector_name());
	
	// number of mirrors
	info.str("");
	info<<pointer_to_reflector->get_number_of_mirrors_on_reflector();
	out<<make_nice_line_with_dots
	("| number of mirrors",info.str());
	
	// horizontal line 
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str(); 
}
//======================================================================
void interaction(){
	
	std::string key_fill_sccan_point_matrix  ="f";
	add_control(key_fill_sccan_point_matrix,
	"fill sccan point matrix");

	std::string key_show_sccan_point_matrix  ="s";
	add_control(key_show_sccan_point_matrix,
	"show sccan point matrix");	
	
	std::string key_draw_all_mirror_response_maps  ="draw";
	add_control(key_draw_all_mirror_response_maps,
	"draw all mirror response maps");
	
	std::string key_user_wants_to_end_analysis  ="back";
	add_control(key_user_wants_to_end_analysis,
	"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_analyse = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_analysis_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_end_analysis)
	==0){
		flag_user_wants_to_analyse = false;
	}
	//==================================================================
	if(valid_user_input.compare(key_fill_sccan_point_matrix)==0){
		fill_sccan_matrix();
	}
	//==================================================================
	if(valid_user_input.compare(key_show_sccan_point_matrix)==0){
		display_sccan_matrix_prompt();
		draw_telescope_pointing_directions_of_sccan_runs();
	}
	//==================================================================
	if(valid_user_input.compare(key_draw_all_mirror_response_maps)==0){
		draw_all_mirror_light_flux_response_maps();
	}
	//==================================================================
}while(flag_user_wants_to_analyse);
}
//======================================================================
void toggle_verbosity(){
	sccan_point_analysis_verbosity = !sccan_point_analysis_verbosity;
}
//======================================================================
bool verbosity_status(){
	return sccan_point_analysis_verbosity;
}
//======================================================================
void draw_telescope_pointing_directions_of_sccan_runs(){
   
	std::stringstream python_execution_string;

	python_execution_string<<"import pylab"<<std::endl;
	python_execution_string<<"pylab.plot(";
	python_execution_string<<get_tilt_position_matrix_for_python();
	python_execution_string<<",'x')"<<std::endl;
	python_execution_string<<"pylab.xlabel('x-tilt [DEG]')"<<std::endl;
	python_execution_string<<"pylab.ylabel('y-tilt [DEG]')"<<std::endl;
	python_execution_string<<"pylab.title(";
	python_execution_string<<"'pointing direction of sccan runs'";
	python_execution_string<<")"<<std::endl;
	python_execution_string<<"pylab.show()"<<std::endl;

	if(verbosity){	
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"draw_telescope_pointing_directions_of_sccan_runs() -> ";
		std::cout<<"plotting using python:"<<std::endl;
		std::cout<<"_________________________________________________"<<std::endl;
		std::cout<<python_execution_string.str();
		std::cout<<"|________________________________________________"<<std::endl;
	}
	
	PyRun_SimpleString(python_execution_string.str().c_str());
}
//======================================================================
void draw_all_mirror_light_flux_response_maps(){
	
	if(sccan_matrix.size()>0){
	
		if(sccan_matrix.at(0).size()==0){
			std::cout<<"sccan_point_analysis -> ";
			std::cout<<"draw_all_mirror_light_flux_response_maps() -> ";
			std::cout<<"there are no mirrors in sccan_matrix!"<<std::endl;			
		}else{
		
			for(uint mirror_itterator = 0;
			mirror_itterator<sccan_matrix.at(0).size(); 
			mirror_itterator++){
				
				if(verbosity){
					std::cout<<"sccan_point_analysis -> ";
					std::cout<<"draw_all_mirror_light_flux_response_maps()";
					std::cout<<" -> ";
					std::cout<<"draw response map "<< mirror_itterator+1;
					std::cout<<"of"<<sccan_matrix.at(0).size();
					std::cout<<" for mirror ID ";
					std::cout<<sccan_matrix.at(0).at(mirror_itterator)->
					get_mirror_ID();
					std::cout<<std::endl;
				}
				draw_normalized_light_flux_response_map
				(mirror_itterator);
			}
		}
	}else{
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"draw_all_mirror_light_flux_response_maps() -> ";
		std::cout<<"there are no sccan runs in sccan_matrix!"<<std::endl;
	}
}
//======================================================================
void draw_normalized_light_flux_response_map(uint mirror_itterator){
	std::stringstream python_execution_string;

	python_execution_string<<"from mpl_toolkits.mplot3d import axes3d"<<std::endl;
	python_execution_string<<"import matplotlib.pyplot as plt"<<std::endl;
	python_execution_string<<"from matplotlib import cm"<<std::endl;
	python_execution_string<<"import numpy as np"<<std::endl;
	
	python_execution_string<<"fig = plt.figure()"<<std::endl;
	python_execution_string<<"ax = fig.add_subplot(111, projection='3d')"<<std::endl;
	python_execution_string<<"ax.plot_surface("<<get_tilt_position_matrix_for_python()<<","<<get_light_flux_matrix_for_python(mirror_itterator)<<", rstride=1, cstride=1, cmap=cm.coolwarm, linewidth=0, antialiased=False)"<<std::endl;
	python_execution_string<<"plt.show()"<<std::endl;
	
	if(verbosity){	
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"draw_telescope_pointing_directions_of_sccan_runs() -> ";
		std::cout<<"plotting using python:"<<std::endl;
		std::cout<<"_________________________________________________"<<std::endl;
		std::cout<<python_execution_string.str();
		std::cout<<"|________________________________________________"<<std::endl;
	}
	
	PyRun_SimpleString(python_execution_string.str().c_str());
}
//======================================================================
private:
std::string get_tilt_position_matrix_for_python(){
	
	std::stringstream list_of_x_tilts_in_deg;
	std::stringstream list_of_y_tilts_in_deg;
	
	for(int pointing_position_itterator = 0;
	pointing_position_itterator<sccan_matrix.size();
	pointing_position_itterator++)
	{
		if(sccan_matrix.at(pointing_position_itterator).size()>0)
		{
			list_of_x_tilts_in_deg<<
			sccan_matrix.
			at(pointing_position_itterator).at(0)->
			get_star_position_relative_to_pointing_direction().
			get_x_tilt_prompt_in_deg();
			
			list_of_y_tilts_in_deg<<
			sccan_matrix.
			at(pointing_position_itterator).at(0)->
			get_star_position_relative_to_pointing_direction().
			get_y_tilt_prompt_in_deg();
				
			// insert delimiters	
			if(pointing_position_itterator<(sccan_matrix.size()-1))
			{
				list_of_x_tilts_in_deg<<",";
				list_of_y_tilts_in_deg<<",";
			}
		}
	}
	std::stringstream tilt_position_matrix_for_python;
	tilt_position_matrix_for_python<<"[";
	tilt_position_matrix_for_python<<list_of_x_tilts_in_deg.str();
	tilt_position_matrix_for_python<<"],[";
	tilt_position_matrix_for_python<<list_of_y_tilts_in_deg.str();
	tilt_position_matrix_for_python<<"]";
	return tilt_position_matrix_for_python.str();
}
//======================================================================
std::string get_light_flux_matrix_for_python(uint mirror_itterator){
	

	if(verbosity){
		std::cout<<"sccan_point_analysis -> ";
		std::cout<<"get_normalized_light_flux_of";
		std::cout<<"_specific_mirror_for_python -> ";
		std::cout<<"mirror ID "<<sccan_matrix.at(0).at(mirror_itterator)->
		get_mirror_ID();
		std::cout<<" at position ";
		std::cout<<mirror_itterator<<std::endl;
	}
	
	std::stringstream list_of_normalized_light_flux;
	
	for(int pointing_position_itterator = 0;
	pointing_position_itterator<sccan_matrix.size();
	pointing_position_itterator++)
	{
		list_of_normalized_light_flux<< 
		sccan_matrix.
		at(pointing_position_itterator).
		at(mirror_itterator)->
		get_normalized_light_flux();
		
		// insert delimiters	
		if(pointing_position_itterator<(sccan_matrix.size()-1))
		{
			list_of_normalized_light_flux<<",";
		}
	}
	
	std::stringstream list_of_normalized_light_flux_python_array_string;
	list_of_normalized_light_flux_python_array_string<<"[";
	list_of_normalized_light_flux_python_array_string<<
	list_of_normalized_light_flux.str();
	list_of_normalized_light_flux_python_array_string<<"]";
	
	return list_of_normalized_light_flux_python_array_string.str();
		
}
//======================================================================
};
//======================================================================
class verbosity_handler 		:public user_interaction{
	//this class is for toggling programm verbosity during runtime
	time_stamp_manager 	*pointer_to_time_stamp_manager;
	ueye_camera		*pointer_to_star_camera;
	ueye_camera		*pointer_to_reflector_camera;
	reflector		*pointer_to_reflector;
	snapshot		*pointer_to_snapshot;
	sccan_point_pair_handler	*pointer_to_sccan_point_pair_handler;
	quick_align		*pointer_to_quick_align;
	sccan_point_analysis	*pointer_to_sccan_point_analysis;
public:
//======================================================================
verbosity_handler(
	time_stamp_manager 	*new_pointer_to_time_stamp_manager,
	ueye_camera		*new_pointer_to_star_camera,
	ueye_camera		*new_pointer_to_reflector_camera,
	reflector		*new_pointer_to_reflector,
	snapshot		*new_pointer_to_snapshot,
	sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
	quick_align		*new_pointer_to_quick_align,
	sccan_point_analysis	*new_pointer_to_sccan_point_analysis
){
	pointer_to_time_stamp_manager = new_pointer_to_time_stamp_manager;
	pointer_to_star_camera 	= new_pointer_to_star_camera;
	pointer_to_reflector_camera = new_pointer_to_reflector_camera;
	pointer_to_reflector 	= new_pointer_to_reflector;
	pointer_to_snapshot 	= new_pointer_to_snapshot;
	pointer_to_sccan_point_pair_handler =
	new_pointer_to_sccan_point_pair_handler;
	pointer_to_quick_align = new_pointer_to_quick_align;
	pointer_to_sccan_point_analysis = 
	new_pointer_to_sccan_point_analysis;
	
	menu_name = "verbosity menu";
	toggle_all_verbosity(false);
}
//======================================================================
std::string get_verbosity_menu_prompt(){
	std::stringstream out, info;
	
	out<<"| verbosity status"<<std::endl;
	
	// time manager
	info.str("");
	if(pointer_to_time_stamp_manager->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| time manager",info.str());

	// star_camera
	info.str("");
	if(pointer_to_star_camera->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| star camera",info.str());

	// reflector_camera
	info.str("");
	if(pointer_to_reflector_camera->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| reflector camera",info.str());

	// reflector
	info.str("");
	if(pointer_to_reflector->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| reflector manager",info.str());

	//snapshot
	info.str("");
	if(pointer_to_snapshot->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots("| snapshot",info.str());
		
	//sccan_point_pair_handler
	info.str("");
	if(pointer_to_sccan_point_pair_handler->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| sccan point pair acquisition",info.str());
		
	//quick_align
	info.str("");
	if(pointer_to_quick_align->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| quick single mirror alignment",info.str());	
	
	//sccan_point_analysis
	info.str("");
	if(pointer_to_sccan_point_analysis->verbosity_status())
	{info<<"TRUE";}else{info<<"FALSE";}
	out<<make_nice_line_with_dots
	("| sccan point pair analysis",info.str());	
		
	//horizontal line
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	
	return out.str(); 
}
//======================================================================
void interaction(){
	std::string key_toggle_time_stamp_manager  ="t";
	add_control(key_toggle_time_stamp_manager,"time manager");	

	std::string key_toggle_star_camera = "sc";
	add_control(key_toggle_star_camera,"star camera");
	
	std::string key_toggle_reflector_camera = "rc";
	add_control(key_toggle_reflector_camera,"reflector camera");
	
	std::string key_toggle_reflector  ="r";
	add_control(key_toggle_reflector,"reflector handler");

	std::string key_toggle_sccan_point_handler  ="sh";
	add_control(key_toggle_sccan_point_handler,"sccan handler");

	std::string key_toggle_snapshot  ="sn";
	add_control(key_toggle_snapshot,"snapshot");

	std::string key_toggle_quick_align  ="q";
	add_control(key_toggle_quick_align,"quick align");

	std::string key_sccan_point_analysis  ="sa";
	add_control(key_sccan_point_analysis,"sccan point pair analysis");
	
	std::string key_all_verbosity_true  ="TRUE";
	add_control(key_all_verbosity_true,"activate all");	

	std::string key_all_verbosity_false  ="FALSE";
	add_control(key_all_verbosity_false,"inactivate all");	
	
	std::string key_back_to_main_menu  ="back";
	add_control(key_back_to_main_menu,"back to main menu");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_toggle_verbosity = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_verbosity_menu_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_back_to_main_menu)==0){
		flag_user_wants_to_toggle_verbosity = false;
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_time_stamp_manager)==0){
		pointer_to_time_stamp_manager->toggle_verbosity();
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_reflector_camera)==0){
		pointer_to_reflector_camera->toggle_verbosity();
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_star_camera)==0){
		pointer_to_star_camera->toggle_verbosity();
	}
	//==============================================================
	if(valid_user_input.compare(key_toggle_reflector)==0){
		pointer_to_reflector->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_sccan_point_handler)==0){
		pointer_to_sccan_point_pair_handler->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_quick_align)==0){
		pointer_to_quick_align->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_sccan_point_analysis)==0){
		pointer_to_sccan_point_analysis->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_all_verbosity_true)==0){
		toggle_all_verbosity(true);
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_snapshot )==0){
		pointer_to_snapshot->toggle_verbosity();
	}
	//==================================================================
	if(valid_user_input.compare(key_all_verbosity_false)==0){
		toggle_all_verbosity(false);
	}
	//==================================================================
}while(flag_user_wants_to_toggle_verbosity);
}
//======================================================================
void toggle_all_verbosity(bool new_verbosity){
	
	if(pointer_to_time_stamp_manager->verbosity_status() != new_verbosity)
	pointer_to_time_stamp_manager->toggle_verbosity();
			
	if(pointer_to_star_camera->verbosity_status() != new_verbosity)
		pointer_to_star_camera->toggle_verbosity();

	if(pointer_to_reflector_camera->verbosity_status() != new_verbosity)
		pointer_to_reflector_camera->toggle_verbosity();
		
	if(pointer_to_reflector->verbosity_status() != new_verbosity)
		pointer_to_reflector->toggle_verbosity();

	if(pointer_to_snapshot->verbosity_status() != new_verbosity)
		pointer_to_snapshot->toggle_verbosity();

	if(pointer_to_sccan_point_pair_handler->verbosity_status() 
	!= new_verbosity)
		pointer_to_sccan_point_pair_handler->toggle_verbosity();
	
	if(pointer_to_quick_align->verbosity_status() 
	!= new_verbosity)
		pointer_to_quick_align->toggle_verbosity();
		
	if(pointer_to_sccan_point_analysis->verbosity_status() 
	!= new_verbosity)
		pointer_to_sccan_point_analysis->toggle_verbosity();
}
//======================================================================
};
//======================================================================
class main_menu 				:public user_interaction{
	public:
	snapshot *pointer_to_snapshot;
	reflector *pointer_to_reflector;
	sccan_point_pair_handler *pointer_to_sccan_point_pair_handler;
	quick_align* pointer_to_quick_align;
	sccan_point_analysis *pointer_to_sccan_point_analysis;
	verbosity_handler *pointer_to_verbosity_handler;
	ueye_camera *pointer_to_star_camera;
	ueye_camera *pointer_to_reflector_camera;
//======================================================================
main_menu
(
snapshot *new_pointer_to_snapshot,
reflector *new_pointer_to_reflector,
sccan_point_pair_handler *new_pointer_to_sccan_point_pair_handler,
quick_align *new_pointer_to_quick_align,
sccan_point_analysis *new_pointer_to_sccan_point_analysis,
verbosity_handler *new_pointer_to_verbosity_handler,
ueye_camera *new_pointer_to_star_camera,
ueye_camera *new_pointer_to_reflector_camera){
	
	pointer_to_snapshot = 
	new_pointer_to_snapshot;
	
	pointer_to_reflector = 
	new_pointer_to_reflector;
	
	pointer_to_sccan_point_pair_handler = 
	new_pointer_to_sccan_point_pair_handler;
	
	pointer_to_quick_align = 
	new_pointer_to_quick_align;
	
	pointer_to_sccan_point_analysis = 
	new_pointer_to_sccan_point_analysis;
	
	pointer_to_verbosity_handler=
	new_pointer_to_verbosity_handler;
	
	pointer_to_star_camera = new_pointer_to_star_camera;
	pointer_to_reflector_camera = new_pointer_to_reflector_camera;
	
	menu_name = "Mirror alignment main menu";
}
//======================================================================
std::string get_main_menu_status_prompt(){
	std::stringstream out, info;
	
	// status star camera
	info.str("");
	if(pointer_to_star_camera->camera_status())
	{info<<"[online]";}else{info<<"[offline]";}
	out<<make_nice_line_with_dots
	("| star camera",info.str());

	// status reflector camera
	info.str("");
	if(pointer_to_reflector_camera->camera_status())
	{info<<"[online]";}else{info<<"[offline]";}
	out<<make_nice_line_with_dots
	("| reflector camera",info.str());
	
	// horizontal line 
	out<<"|";
	out<<get_line_of_specific_character("_",command_line_columns-1);
	out<<std::endl;
	return out.str(); 
}
//======================================================================
void interaction(){
	
	std::string key_user_wants_to_interaction  ="s";
	add_control(key_user_wants_to_interaction,
	"take snapshots");
	
	std::string key_user_wants_to_modify_reflector="r";
	add_control(key_user_wants_to_modify_reflector,
	"modify mirrors of reflector");
	
	std::string key_user_wants_to_acquire_sccan_points="a";
	add_control(key_user_wants_to_acquire_sccan_points,
	"acquire sccan points");
	
	std::string key_user_wants_to_quick_align_a_mirror ="q";
	add_control(key_user_wants_to_quick_align_a_mirror,
	"quick single mirror alignment");
	
	std::string key_analysis ="A";
	add_control(key_analysis,
	"analyse sccan points");
	
	std::string key_toggle_verbosity ="v";
	add_control(key_toggle_verbosity,
	"toggle programm verbosity");
	
	std::string key_user_wants_to_end_mirror_alignment  ="quit";
	add_control(key_user_wants_to_end_mirror_alignment,
	"quit mirror alignment");
	
	std::string user_input;
	std::stringstream out;
	
	bool flag_user_wants_to_align_mirrors = true;
	
	do{
	//==============================================================
	// prompt snapshot options to command line
	//==============================================================
	update_status_prompt(get_main_menu_status_prompt());
	//==============================================================
	// get input of user via command line
	//==============================================================
	get_user_input();
	//==============================================================
	// interpret answer of user
	//==============================================================
	//std::cout<<"valid_user_input: "<<valid_user_input<<std::endl;	
	//==============================================================
	if(valid_user_input.compare(key_user_wants_to_end_mirror_alignment)
	==0){
		flag_user_wants_to_align_mirrors = false;
	}
	//==================================================================
	if(valid_user_input.compare(key_analysis)==0){
		pointer_to_sccan_point_analysis->interaction();
	}
	//==================================================================
	if(valid_user_input.compare(key_user_wants_to_interaction)==0){
		pointer_to_snapshot->interaction();
	}
	//==================================================================
	if(valid_user_input.compare(key_user_wants_to_modify_reflector)==0){
		pointer_to_reflector->interaction();
	}
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_acquire_sccan_points)==0){
		pointer_to_sccan_point_pair_handler->interaction();
	} 
	//==================================================================
	if(valid_user_input.
	compare(key_user_wants_to_quick_align_a_mirror)==0){
		pointer_to_quick_align->interaction();
	}
	//==================================================================
	if(valid_user_input.compare(key_toggle_verbosity)==0){
		pointer_to_verbosity_handler->interaction();
	}
	//==================================================================
}while(flag_user_wants_to_align_mirrors);
}
//======================================================================
};                                                                      
//======================================================================
int main(){
	int system_call_return_value;
	system_call_return_value = system("clear"); 
	system_call_return_value = system("clear"); 

	// python for plotting
	Py_Initialize();
	
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

	quick_align quick(&global_reflector_instance,&sccan_handle);

	sccan_point_analysis analysis(
	&sccan_handle,&global_reflector_instance);
	
	verbosity_handler verbosity_interaction(
	&global_time_stamp_manager_instance,
	&star_camera,
	&reflector_camera,
	&global_reflector_instance,
	&snap,
	&sccan_handle,
	&quick,
	&analysis
	);
	
	main_menu menu(
	&snap,
	&global_reflector_instance,
	&sccan_handle,
	&quick,
	&analysis,
	&verbosity_interaction,
	&star_camera,
	&reflector_camera);
	
	menu.interaction();	
	
	return 0;
	}
	
