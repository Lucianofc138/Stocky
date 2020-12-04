cmake_minimum_required(VERSION 2.8)
project( MAIN )
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
find_package( OpenCV REQUIRED )

add_executable( MAIN main.cpp
                     src/floorFunctions.cpp)
target_include_directories( MAIN PRIVATE include)
target_link_libraries( MAIN ${OpenCV_LIBS} )