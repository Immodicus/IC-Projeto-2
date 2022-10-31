cmake_minimum_required(VERSION 2.8)
project( IC-Project-2 )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( img_cpy img_cpy.cpp )
target_link_libraries( img_cpy ${OpenCV_LIBS} )

add_executable( img_effects img_effects.cpp )
target_link_libraries( img_effects ${OpenCV_LIBS} )