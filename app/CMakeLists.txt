# @author LucianoFC & TomasMP

project("app")

add_library(SHELFUTILS  src/shelfUtils/shelfUtils.cpp)
add_library(HISTUTILS   src/histUtils/histUtils.cpp)
add_library(PERSUTILS   src/perspectiveUtils/perspectiveUtils.cpp)
add_library(PROCMISC    src/processingMisc/scanFloorLib.cpp
                        src/processingMisc/drawResources.cpp
                        src/processingMisc/movementMask.cpp)

target_include_directories(SHELFUTILS PUBLIC include)
target_link_libraries(SHELFUTILS PRIVATE ${OPENCV})

target_include_directories(HISTUTILS PUBLIC include)
target_link_libraries(HISTUTILS PRIVATE ${OPENCV})

target_include_directories(PERSUTILS PUBLIC include)
target_link_libraries(PERSUTILS PRIVATE ${OPENCV})

target_include_directories(PROCMISC PUBLIC include)
target_link_libraries(PROCMISC PRIVATE ${OPENCV})

add_executable( STOCKY main.cpp )

target_include_directories( STOCKY PRIVATE include)
target_link_libraries( STOCKY PRIVATE SHELFUTILS HISTUTILS PERSUTILS PROCMISC ${OPENCV} )