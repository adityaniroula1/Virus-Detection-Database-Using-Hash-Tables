cmake_minimum_required(VERSION 3.23)
project(proj4)

set(CMAKE_CXX_STANDARD 14)

add_executable(proj4 main.cpp vdetect.cpp vdetect.h)
