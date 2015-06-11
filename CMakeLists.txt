cmake_minimum_required(VERSION 2.8)
project( StereoBlockMatch )
add_subdirectory( cvImagePipeline )
find_package( OpenCV REQUIRED )
include_directories( cvImagePipeline/include )
add_executable( StereoBlockMatch StereoBlockMatch.cpp )
target_link_libraries( StereoBlockMatch
		cvImagePipeline
		${OpenCV_LIBS} )
install(TARGETS StereoBlockMatch DESTINATION .)
