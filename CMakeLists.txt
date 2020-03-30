cmake_minimum_required(VERSION 2.8.9)
project(sars_cov2_slovakia)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")

include_directories(include)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cxx")
 
add_executable(main ${SOURCES})