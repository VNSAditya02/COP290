cmake_minimum_required(VERSION 3.15)
project(COP290)

set(CMAKE_CXX_STANDARD 14)

add_executable(COP290 src/angle_correction.cpp src/traffic_density.cpp src/Partc_Method_1.cpp src/Partc_Method_2.cpp src/Partc_Method_3.cpp src/Partc_Method_4.cpp)
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )

FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES})