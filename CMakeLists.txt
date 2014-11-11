cmake_minimum_required(VERSION 2.8)
project( StereoBlockMatch )
find_package( OpenCV REQUIRED )
set( cvIP_DIR C:/cvImagePipeline)
include_directories( ${cvIP_DIR}/include )
add_executable( StereoBlockMatch StereoBlockMatch.cpp )
target_link_libraries( StereoBlockMatch
		${cvIP_DIR}/cvImagePipeline.lib
		${OpenCV_LIBS} )

