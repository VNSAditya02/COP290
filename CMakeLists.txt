cmake_minimum_required(VERSION 3.15)
project(COP290)

set(CMAKE_CXX_STANDARD 14)

add_executable(COP290 src/angle_correction.cpp)
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

find_package( Boost REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )
target_link_libraries( ${PROJECT_NAME} ${Boost_LIBS} )