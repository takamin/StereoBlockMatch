cmake_minimum_required(VERSION 2.8)
project( StereoBlockMatch )

add_subdirectory( cvImagePipeline )
add_subdirectory( win-c )

find_package( OpenCV REQUIRED )

include_directories(
    cvImagePipeline/include
    win-c/include )

add_executable(
    StereoBlockMatch
    StereoBlockMatch.cpp
    cal.cpp
    util.cpp)

target_link_libraries(
    StereoBlockMatch
    cvImagePipeline
    libwinc
    ${OpenCV_LIBS} )

add_executable(
    stereoShot
    stereoShot.cpp
    util.cpp )

target_link_libraries(
    stereoShot
    cvImagePipeline
    libwinc
    ${OpenCV_LIBS} )

install(
    TARGETS
        StereoBlockMatch
        stereoShot
    DESTINATION .)
