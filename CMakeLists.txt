#Specify minimum cmake version requirement
CMAKE_MINIMUM_REQUIRED(VERSION 3.1 FATAL_ERROR)

#Set compiler flags variable to allow for longer loops
set(EXTRA_COMPILE_FLAGS "-frounding-math")

#Specify project name and languages
project(arithmetic LANGUAGES CXX)

#Option for code sanitization
#include(../Sanitizers.cmake)

#Globally include 'include' directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Create variable for interval arithmetic headers
set(interval_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/ra/interval.hpp)

#Create vrariable for cexpr_mandelbrot header
#set(mandelbrot_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/ra/mandelbrot.hpp)

#Add executable targets
add_executable(test_interval ${CMAKE_CURRENT_SOURCE_DIR}/app/test_interval.cpp ${interval_headers})
#add_executable( test_mandelbrot ${CMAKE_CURRENT_SOURCE_DIR}/app/test_mandelbrot.cpp ${mandelbrot_headers})

#Make sure interval arithmetic gets compiler flags for floating-point environmental changes
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/app/test_interval.cpp PROPERTIES COMPILE_FLAGS ${EXTRA_COMPILE_FLAGS})

#Find Catch2 package
find_package(Catch2 REQUIRED)

#Include Catch2 header
include(Catch)
