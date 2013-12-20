#---------------------------------------------
# Description: PTLib cmake-able build system
# 
#---------------------------------------------
macro(ptlib_write_revision_h)
    if(NOT EXISTS ${PROJECT_SOURCE_DIR}/revision.h.in)
        message(FATAL_ERROR "PTLib revision.h.in can't be found in ${PROJECT_SOURCE_DIR}")
    endif()

    file(READ ${PROJECT_SOURCE_DIR}/revision.h.in _VIN_FILE)

    # default out-of-subversion revision
    set(ptlib_DEFAULT_WC_REVISION "666")

    if(EXISTS ${PROJECT_SOURCE_DIR}/.svn)
        # doing build from subversion
        find_package(Subversion)
        if(Subversion_FOUND)
            Subversion_WC_INFO(${PROJECT_SOURCE_DIR} ptlib)
            message("Doing build for Rev: ${ptlib_WC_REVISION}")
            set(ptlib_DEFAULT_WC_REVISION ${ptlib_WC_REVISION})
        endif()
    endif()

    if(NOT Subversion_FOUND)
        # cover case when we build from SF package
        string(REGEX MATCH "Revision:[ ]+[0-9]+" _pkg_rev ${_VIN_FILE})
        if(_pkg_rev AND NOT _pkg_rev STREQUAL "")
            string(REGEX MATCH "[0-9]+" ptlib_DEFAULT_WC_REVISION ${_pkg_rev})
        endif()
    endif()

    string(REPLACE "$WCREV$" "${ptlib_DEFAULT_WC_REVISION}" _out ${_VIN_FILE})

    file(WRITE ${PROJECT_SOURCE_DIR}/revision.h ${_out})
endmacro()

macro(ptlib_get_version vmajor vminor vpatch)
    file(READ "${PROJECT_SOURCE_DIR}/version.h" _ptlib_ver_file)
    string(REGEX MATCH "#define[ ]+MAJOR_VERSION[ ]+[0-9]+" _maj_str ${_ptlib_ver_file})
    string(REGEX MATCH "#define[ ]+MINOR_VERSION[ ]+[0-9]+" _min_str ${_ptlib_ver_file})
    string(REGEX MATCH "#define[ ]+BUILD_NUMBER[ ]+[0-9]+" _patch_str ${_ptlib_ver_file})
    string(REGEX MATCH "[0-9]+" ${vmajor} ${_maj_str})
    string(REGEX MATCH "[0-9]+" ${vminor} ${_min_str})
    string(REGEX MATCH "[0-9]+" ${vpatch} ${_patch_str})
endmacro()

macro(get_WIN32_WINNT version)
    if (WIN32 AND CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REPLACE "." "" ver ${ver})
        string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
    endif()
endmacro()
