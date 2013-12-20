#---------------------------------------------
# Description: PTLib cmake-able build system
#---------------------------------------------
cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME "opalvoip_ptlib")
project(${PROJECT_NAME} C CXX)

set(PTLIB_SRC_ROOT ${PROJECT_SOURCE_DIR} CACHE PATH "PTLib source root")
set(CMAKE_MODULE_PATH ${PTLIB_SRC_ROOT}/cmake ${CMAKE_MODULE_PATH})
set(PTLIB_INCLUDE_PATH ${PTLIB_SRC_ROOT}/include)

#---------------------
# Options block
#---------------------

#---------------------
# Configuration block
#---------------------
include(PTLibUtils)

ptlib_get_version(PTLIB_MAJOR PTLIB_MINOR PTLIB_BUILD)
message(" >>> Building PTLib version: ${PTLIB_MAJOR}.${PTLIB_MINOR}.${PTLIB_BUILD}")
ptlib_write_revision_h()
get_WIN32_WINNT(WINVER)

include(PTLibConfig)

configure_file(${CMAKE_MODULE_PATH}/ptbuildopts.h.cmake
    ${PTLIB_INCLUDE_PATH}/ptbuildopts.h)
configure_file(${CMAKE_MODULE_PATH}/ptbuildopts.h.cmake
    ${PTLIB_INCLUDE_PATH}/ptlib_config.h)

#---------------------
# Adding subprojects
#---------------------
add_subdirectory(src)
add_subdirectory(samples)
#add_subdirectory(plugins)
#add_subdirectory(samples)
#add_subdirectory(tools)
