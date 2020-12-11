# @author LucianoFC & TomasMP

cmake_minimum_required(VERSION 3.16)
project( STOCKY )

include(third_party/opencv.cmake)

option(BUILD_PLAYGROUND "Build playground module" OFF)

add_subdirectory( app )

if(BUILD_PLAYGROUND)
    add_subdirectory( playground/tomas )
    add_subdirectory( playground/luciano )
endif()