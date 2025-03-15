###################################
# Setup CMake                     #
###################################
cmake_minimum_required(VERSION 3.27)

###################################
# Setup project                   #
###################################
project(bomberman-dev VERSION 1.0 DESCRIPTION "TODO" LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
add_subdirectory(src)

###################################
# Setup tools                     #
###################################
include(format)
