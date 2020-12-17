# @author LucianoFC & TomasMP

project("app")

add_library(SHELFUTILS src/shelfUtils/shelfUtils.cpp)
add_library(HISTUTILS src/histUtils/histUtils.cpp)
add_library(PERSUTILS src/perspectiveUtils/perspectiveUtils.cpp)

target_include_directories(SHELFUTILS PUBLIC include)
target_link_libraries(SHELFUTILS PRIVATE ${OPENCV})

target_include_directories(HISTUTILS PUBLIC include)
target_link_libraries(HISTUTILS PRIVATE ${OPENCV})

target_include_directories(PERSUTILS PUBLIC include)
target_link_libraries(PERSUTILS PRIVATE ${OPENCV})

add_executable( STOCKY main.cpp )

target_include_directories( STOCKY PRIVATE include)
target_link_libraries( STOCKY PRIVATE SHELFUTILS PERSUTILS ${OPENCV} )