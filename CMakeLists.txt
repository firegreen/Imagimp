project(miniGimp-IMAC-JS)
cmake_minimum_required(VERSION 2.8)
# -DCMAKE_CC_COMPILER=gcc

FILE(GLOB_RECURSE SRC_LIST "src/*.c")

INCLUDE_DIRECTORIES(include)

FILE(GLOB_RECURSE HEADERS "include/*.h")
#add_custom_target(headers SOURCES ${HEADERS})
#
set(LIBRARIES "-lm -lopengl32 -lglu32 -lfreeglut")

SET (CMAKE_C_COMPILER "gcc")
SET (CMAKE_C_FLAGS	               "-Wall -std=c11 ${LIBRARIES}")
SET (CMAKE_C_FLAGS_DEBUG	       "-g")
SET (CMAKE_C_FLAGS_MINSIZEREL	       "-O1 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELEASE	       "-O2 -DNDEBUG")
SET (CMAKE_C_FLAGS_RELWITHDEBINFO	"-O1 -g")

set(PWD ${CMAKE_SOURCE_DIR})

if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${SRC_LIST})
else()
    add_executable(${PROJECT_NAME} ${SRC_LIST})
endif()
if(WIN32)
	set(outputdir "${PWD}/Win32")
else()
	set(outputdir "${PWD}/Unix")
endif()

file(MAKE_DIRECTORY ${outputdir})
set(EXECUTABLE_OUTPUT_PATH ${outputdir})
SET(CMAKE_EXE_LINKER_FLAGS  "" )
