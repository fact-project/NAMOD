#include "ueye_camera.h"
//======================================================================
ueye_camera::ueye_camera(int camera_ID_to_initialize,
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
void ueye_camera::set_camera_ID(int camera_ID_to_initialize){
	ueye_camera_handle = (HIDS)camera_ID_to_initialize;
	ueye_camera_id = camera_ID_to_initialize;	
}
//======================================================================
uint ueye_camera::get_camera_ID(){
	return ueye_camera_id;
}
//======================================================================
ueye_camera::~ueye_camera(){
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
bool ueye_camera::initialize(){
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
bool ueye_camera::acquire_image(
double *pointer_to_desired_exposure_time_in_ms){

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
bool ueye_camera::acquire_image(
double *pointer_to_desired_exposure_time_in_ms,
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
		
		std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
		std::cout<<"acquire_image() -> ";
		std::cout<<"exp. time iteration No.: ";
		std::cout<<exposure_itterations;
		std::cout<<" -> exp. time = ";
		std::cout<<*pointer_to_desired_exposure_time_in_ms;
		std::cout<<"ms, ";
		std::cout<<"max rel. response = ";
		std::cout<<max_relative_camera_response<<"[1]"<<std::endl;
		
		if(verbosity){
			std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
			std::cout<<"acquire_image() -> ";
			std::cout<<"max_relative_camera_response ";
			std::cout<<max_relative_camera_response<<std::endl;
		}		
		
		double relative_camera_response_difference = 
		desired_relative_maximal_camera_response -
		max_relative_camera_response;
		
		if(verbosity){
			std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
			std::cout<<"acquire_image() -> ";
			std::cout<<"relative_camera_response_difference ";
			std::cout<<relative_camera_response_difference;
			std::cout<<std::endl;
		}		
		
		if(
		fabs(relative_camera_response_difference) <= 0.05 &&
		max_relative_camera_response != 1.0
		){
			exposure_time_is_bad = false;
			if(verbosity){
				std::cout<<"ueye camera ID: "<<ueye_camera_id<<" -> ";
				std::cout<<"acquire_image() -> ";
				std::cout<<"abs(relative_camera_response_difference) ";
				std::cout<<fabs(relative_camera_response_difference);
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
bool ueye_camera::long_time_exposure(bool long_time_exposure){
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
void ueye_camera::display_camera_information(){
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
sccan_image ueye_camera::get_latest_image(){
	if(verbosity){
		std::cout<<"ueye_camera->get_latest_image()"<<std::endl;
	}
	return latest_image;
}
//======================================================================
void ueye_camera::export_latest_image(std::string filename_prefix){
	latest_image.set_name(filename_prefix);
	latest_image.export_image_to_disk();
}
//======================================================================
double ueye_camera::get_current_exposure_time_in_ms(){
	return ueye_exposure_time_in_ms;
}
//======================================================================
void ueye_camera::disp_latest_image(){
	if(verbosity){
		std::cout<<"ueye_camera -> disp_latest_image()"<<std::endl;
	}
	latest_image.disp_image();
}
//======================================================================
bool ueye_camera::camera_status(){
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
void ueye_camera::toggle_verbosity(){
	verbosity = !verbosity;
	intrinsic.toggle_verbosity(verbosity);
	latest_image.toggle_verbosity(verbosity);
}
//======================================================================
cv::Size ueye_camera::
get_sensor_size()const{
	cv::Size sensor_size;
	sensor_size.height = ueye_camera_sensor_number_of_pixels_in_hight;
	sensor_size.width  = ueye_camera_sensor_number_of_pixels_in_width;
	return sensor_size;
}
//======================================================================
