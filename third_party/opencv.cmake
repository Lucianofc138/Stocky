find_package( OpenCV REQUIRED )

if(OpenCV_FOUND)
    set(OPENCV ${OpenCV_LIBS})
else()
    message(FATAL_ERROR "Unable to find openCV, make sure is installed.")
endif()