#=======================================================================
#
# multi mirror alignment software by Sebastian Mueller E5b Dortmund
# Chair for astro particle physics
#
#=======================================================================
cmake_minimum_required(VERSION 2.8)
#=======================================================================
# compiler flags
set (CMAKE_CXX_FLAGS "-std=gnu++11  -O3") #-Wall
#=======================================================================
project( dual_camera )
include_directories(/usr/include)
#=======================================================================
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
snapshot.cpp
sccan_point_pair_handler.cpp
quick_align.cpp
sccan_point_analysis.cpp
verbosity_handler.cpp
main_menu.cpp
Vector3D.cpp
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
#find_package(PythonLibs REQUIRED)
#include_directories(${PYTHON_INCLUDE_DIRS})
#link_directories(${PythonLibs_LIBRARY_DIRS})
#add_definitions(${PythonLibs_DEFINITIONS})
#target_link_libraries(dual_camera ${PYTHON_LIBRARIES} )
#=======================================================================
# plotting mathGL
if(MGL_HAVE_GSL)
	include_directories(${GSL_INCLUDE_DIR})
endif(MGL_HAVE_GSL)

#target_link_libraries(mgl_example mgl)
target_link_libraries(dual_camera mgl)

if(MGL_HAVE_FLTK)
	include_directories(${FLTK_INCLUDE_DIR})
	#add_executable(mgl_fltk_example wnd_samples.cpp fltk_example.cpp)
	target_link_libraries(dual_camera mgl-fltk)
endif(MGL_HAVE_FLTK)

if(MGL_HAVE_GLUT)
	#add_executable(mgl_glut_example wnd_samples.cpp glut_example.cpp)
	target_link_libraries(dual_camera mgl-glut)
endif(MGL_HAVE_GLUT)

if(MGL_HAVE_WX)
	include(${wxWidgets_USE_FILE})
	#add_executable(mgl_wx_example wnd_samples.cpp wx_example.cpp)
	target_link_libraries(dual_camera mgl-wx)
endif(MGL_HAVE_WX)

if(MGL_HAVE_QT)
	include(${QT_USE_FILE})
#	qt_wrap_cpp(mgl_qt_example MGL_MOC_EX_FILES ../include/mgl2/qt.h)
	#add_executable(mgl_qt_example wnd_samples.cpp qt_example.cpp ${MGL_MOC_EX_FILES})
	target_link_libraries(dual_camera mgl-qt)
endif(MGL_HAVE_QT)
#=======================================================================
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
