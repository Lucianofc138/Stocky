# @author LucianoFC & TomasMP

project("app")

find_package( OpenCV REQUIRED )

add_library(SHELFUTILS src/shelfUtils/shelfUtils.cpp)

target_include_directories(SHELFUTILS PUBLIC include)
target_link_libraries(SHELFUTILS PRIVATE ${OpenCV_LIBS})

add_executable( STOCKY main.cpp )

target_include_directories( STOCKY PRIVATE include)
target_link_libraries( STOCKY PRIVATE SHELFUTILS ${OpenCV_LIBS} )