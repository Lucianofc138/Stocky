# @author LucianoFC & TomasMP

project("app")

add_library(SHELFUTILS src/shelfUtils/shelfUtils.cpp)
add_library(HISTUTILS src/histUtils/histUtils.cpp)

target_include_directories(SHELFUTILS PUBLIC include)
target_link_libraries(SHELFUTILS PRIVATE ${OPENCV})

target_include_directories(HISTUTILS PUBLIC include)
target_link_libraries(HISTUTILS PRIVATE ${OPENCV})

add_executable( STOCKY main.cpp )

target_include_directories( STOCKY PRIVATE include)
target_link_libraries( STOCKY PRIVATE SHELFUTILS ${OPENCV} )