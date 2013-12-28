#---------------------------------------------
# Description: PTLib cmake-able build system
#---------------------------------------------
cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME "opal_ptlib")
project(${PROJECT_NAME} C CXX)

set(PTLIB_SRC_ROOT ${PROJECT_SOURCE_DIR} CACHE PATH "PTLib source root")
set(CMAKE_MODULE_PATH ${PTLIB_SRC_ROOT}/cmake ${CMAKE_MODULE_PATH})
set(PTLIB_INCLUDE_PATH ${PTLIB_SRC_ROOT}/include)

#---------------------
# Options block
#---------------------
option(BUILD_SHARED_LIBS "Build ptlib as shared library" OFF)
option(WITH_SAMPLES "Build ptlib's samples" ON)

#---------------------
# Configuration block
#---------------------
set(ptlib_postfix "")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(STATUS "Compiling ptlib for 64-bit...")
    set(ptlib_postfix "${ptlib_postfix}64")
else()
    message(STATUS "Compiling ptlib for 32-bit...")
endif()
if(NOT ${BUILD_SHARED_LIBS})
    set(ptlib_postfix "${ptlib_postfix}s")
endif()

set(ptlib "ptlib${ptlib_postfix}")

include(PTLibUtils)

ptlib_get_version(PTLIB_MAJOR PTLIB_MINOR PTLIB_BUILD)
message(" >>> Building PTLib version: ${PTLIB_MAJOR}.${PTLIB_MINOR}.${PTLIB_BUILD}")
ptlib_write_revision_h()
get_WIN32_WINNT(WINVER)

include(PTLibConfig)
if(EXPAT_FOUND)
    set(P_EXPAT 1)
endif()
configure_file(${CMAKE_MODULE_PATH}/ptbuildopts.h.cmake
    ${PTLIB_INCLUDE_PATH}/ptbuildopts.h)
configure_file(${CMAKE_MODULE_PATH}/ptbuildopts.h.cmake
    ${PTLIB_INCLUDE_PATH}/ptlib_config.h)

#---------------------
# Adding subprojects
#---------------------
add_subdirectory(src)
if (WITH_SAMPLES)
    add_subdirectory(samples)
endif()
