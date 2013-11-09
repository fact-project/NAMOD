cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_FLAGS "-std=gnu++11 -O3") #-Wall
project( dual_camera )
include_directories(/usr/include)
add_executable( dual_camera 
dual_camera.cpp
programm_verbosity.cpp
user_interaction.cpp
time_stamp_manager.cpp
list_of_pixel_positions.cpp
pointing_direction.cpp
star_in_image.cpp
intrinsic_camera_parameter.cpp
simple_image.cpp
sccan_image.cpp
global_variables.cpp
ueye_camera.cpp
sccan_analysis_point.cpp
sccan_point_pair.cpp
mirror.cpp
reflector.cpp
)
#=======================================================================
# UEYE IDS imaging
target_link_libraries(dual_camera libueye_api.so )
#=======================================================================
# configiration file library: libconfig
target_link_libraries(dual_camera config++)
#=======================================================================
#open CV
find_package( OpenCV REQUIRED )
include_directories(/usr/include)
target_link_libraries(dual_camera ${OpenCV_LIBS} )
#=======================================================================
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})
#link_directories(${PythonLibs_LIBRARY_DIRS})
#add_definitions(${PythonLibs_DEFINITIONS})
target_link_libraries(dual_camera ${PYTHON_LIBRARIES} )
#=======================================================================
#target_link_libraries(dual_camera log4cpp)
#=======================================================================
