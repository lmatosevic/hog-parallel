cmake_minimum_required(VERSION 3.3)
project(diplomski_hog)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp include/HistogramOfOrientedGradients.h src/HistogramOfOrientedGradients.cpp include/PPMImage.h src/PPMImage.cpp include/Operations.h src/Operations.cpp include/QPCounter.h src/QPCounter.cpp include/Common.h)
add_executable(diplomski_hog ${SOURCE_FILES})