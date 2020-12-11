project("playground-tomas")

add_executable(HISTO_ANALYSIS histogram_analysis.cpp)
target_link_libraries(HISTO_ANALYSIS PRIVATE 
    HISTUTILS
    ${OPENCV})