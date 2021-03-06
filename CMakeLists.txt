cmake_policy(SET CMP0011 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0079 NEW)

#-------------------------------------------------------------------------------
# The subdirlist macro.
#-------------------------------------------------------------------------------

macro(subdirList result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
      list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

#-------------------------------------------------------------------------------
# Framework finder function.
#-------------------------------------------------------------------------------

if(APPLE)
  function(find_framework nameof)
    message("Finding framework ${nameof}")
    find_library(FRAMEWORK_LIB ${nameof})
    mark_as_advanced(FRAMEWORK_LIB)
    get_filename_component(FRAMEWORK_NAME ${nameof} NAME_WE)
    get_filename_component(FRAMEWORK_PATH ${FRAMEWORK_LIB} PATH)
    set(FRAMEWORK_PATHS "${FRAMEWORK_PATHS} -F ${FRAMEWORK_PATH}" PARENT_SCOPE)
    set(FRAMEWORK_NAMES "${FRAMEWORK_NAMES} -framework ${FRAMEWORK_NAME}" PARENT_SCOPE)
  endfunction()
endif()

#-------------------------------------------------------------------------------
# Include cotire and speed up development!
#-------------------------------------------------------------------------------

if(WIN32 OR APPLE OR UNIX)
  set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
  include(cotire)
endif()

#-------------------------------------------------------------------------------
# Setup some basics for macOS.
#-------------------------------------------------------------------------------

if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")
  set(CMAKE_OSX_SYSROOT macosx10.15)
endif()

# ------------------------------------------------------------------------------
# Setup some platform dependent options.
# ------------------------------------------------------------------------------

# Establish which platform directory to look in (rel).
if(WIN32)
  set(OS "win64")
endif()

# Setup necessary #defines for using the engine. Establishes debug/release.
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -DRELEASE")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   -D_DEBUG -DDEBUG")

#-------------------------------------------------------------------------------
# Setup the core EON project.
#-------------------------------------------------------------------------------

# Setup the minimum required version.
cmake_minimum_required(VERSION 3.16.2)

# Setup the project and versioning.
project(cog VERSION 0.3.2)

# Set the C++ standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED)

# Configure the version header.
configure_file(config.h.in config.h)

#-------------------------------------------------------------------------------
# Add libraries and defines.
#-------------------------------------------------------------------------------

add_subdirectory(src)

# ------------------------------------------------------------------------------
# Setup debugging directory in Visual Studio.
# ------------------------------------------------------------------------------

if(MSVC)
  set_property(
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT cog_unity)
endif()
