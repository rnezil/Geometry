#Specify minimum cmake version requirement
CMAKE_MINIMUM_REQUIRED(VERSION 3.1 FATAL_ERROR)

#Specify project name and languages
project(arithmetic LANGUAGES CXX)

#Set compiler flags variable to allow for longer loops
set(EXTRA_COMPILE_FLAGS "-frounding-math")

#Create variable for interval arithmetic headers
set(interval_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/ra/interval.hpp)

#Create variable for kernel headers
set(kernel_headers ${CMAKE_CURRENT_SOURCE_DIR}/include/ra/kernel.hpp)

#Force CGAL to not warn about CMake build type
set(CGAL_DO_NOT_WARN_ABOUT_CMAKE_BUILD_TYPE TRUE)

#Add compiler options
add_compile_options(${EXTRA_COMPILE_FLAGS})

#Find packages
find_package(Catch2 REQUIRED)
find_package(CGAL REQUIRED)

#Include Catch2 header
include(Catch)

#Globally include 'include' directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

#Add executable targets
add_executable(test_interval ${CMAKE_CURRENT_SOURCE_DIR}/app/test_interval.cpp ${interval_headers})
add_executable(test_kernel ${CMAKE_CURRENT_SOURCE_DIR}/app/test_kernel.cpp ${kernel_headers})
add_executable(tri_demo ${CMAKE_CURRENT_SOURCE_DIR}/app/triangulation_2_demo.cpp ${CMAKE_CURRENT_SOURCE_DIR}/app/triangulation_2.hpp)
add_executable(delaunay_triangulation ${CMAKE_CURRENT_SOURCE_DIR}/app/delaunay_triangulation.cpp ${kernel_headers} ${CMAKE_CURRENT_SOURCE_DIR}/app/triangulation_2.hpp)

#Link libraries and include target-specific directories
target_include_directories(test_kernel PUBLIC ${CGAL_INCLUDE_DIRS})
target_link_libraries(test_kernel ${CGAL_LIBRARY})
target_include_directories(tri_demo PUBLIC ${CGAL_INCLUDE_DIRS})
target_include_directories(tri_demo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(tri_demo ${CGAL_LIBRARY})
target_include_directories(delaunay_triangulation PUBLIC ${CGAL_INCLUDE_DIRS})
target_include_directories(delaunay_triangulation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(delaunay_triangulation ${CGAL_LIBRARY})
