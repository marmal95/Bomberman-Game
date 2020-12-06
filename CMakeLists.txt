cmake_minimum_required(VERSION 3.19)

PROJECT(Bomberman)
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-Wall -Werror -Wpedantic")

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)


FIND_PACKAGE(SFML 2 REQUIRED audio graphics network system window)
INCLUDE_DIRECTORIES(${SFML_INCLUDE_DIR})

FIND_PACKAGE(EntityX REQUIRED)
INCLUDE_DIRECTORIES(${ENTITYX_INCLUDE_DIR})


MACRO(HEADER_DIRECTORIES return_list)
    FILE(GLOB_RECURSE new_list *.h *.hpp)
    SET(dir_list "")
    FOREACH(file_path ${new_list})
        GET_FILENAME_COMPONENT(dir_path ${file_path} PATH)
        SET(dir_list ${dir_list} ${dir_path})
    ENDFOREACH()
    LIST(REMOVE_DUPLICATES dir_list)
    SET(${return_list} ${dir_list})
ENDMACRO()

HEADER_DIRECTORIES(INCLUDES)
include_directories(${INCLUDES})


FILE(GLOB_RECURSE HEADERS "Bomberman/include/*.hpp")
FILE(GLOB_RECURSE SOURCES "Bomberman/source/*.cpp")

SOURCE_GROUP("include" FILES ${HEADERS})
SOURCE_GROUP("source" FILES ${SOURCES})


ADD_EXECUTABLE(Bomberman ${HEADERS} ${SOURCES})
TARGET_LINK_LIBRARIES(Bomberman sfml-audio sfml-graphics sfml-network sfml-system sfml-window ${ENTITYX_LIBRARY})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Bomberman/resources ${EXECUTABLE_OUTPUT_PATH}/resources)
