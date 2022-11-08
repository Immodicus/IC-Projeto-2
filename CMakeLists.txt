cmake_minimum_required(VERSION 3.2)
project( IC-Project-2 )

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( img_cpy img_cpy.cpp )
target_link_libraries( img_cpy ${OpenCV_LIBS} )

add_executable( img_effects img_effects.cpp )
target_link_libraries( img_effects ${OpenCV_LIBS} )

add_executable( golomb_test golomb_test.cpp )