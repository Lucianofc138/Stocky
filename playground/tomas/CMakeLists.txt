project("playground-tomas")

add_executable(HISTO_ANALYSIS histogram_analysis.cpp)
target_link_libraries(HISTO_ANALYSIS PRIVATE 
    HISTUTILS
    ${OPENCV})

add_executable(PRODUCT product.cpp)

target_include_directories(PRODUCT PRIVATE include)

target_link_libraries(PRODUCT PRIVATE 
    HISTUTILS
    ${OPENCV})