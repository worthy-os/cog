message("Generating project for cog")

################################################################################

#-------------------------------------------------------------------------------
# Adding sources and headers.
#-------------------------------------------------------------------------------

file(GLOB_RECURSE cog_HEADERS "include/*.h")
file(GLOB_RECURSE cog_SOURCES "src/*.cpp")

#-------------------------------------------------------------------------------
# Create library for libcog.a
#-------------------------------------------------------------------------------

add_executable(cog
  "${cog_HEADERS}"
  "${cog_SOURCES}")

#-------------------------------------------------------------------------------
# Figure out what platform we're on.
#-------------------------------------------------------------------------------

if(WIN32)
  set(OS "win64")
elseif(APPLE)
  set(OS "macOS")
elseif(UNIX)
  set(OS "unix")
endif()

################################################################################

#-------------------------------------------------------------------------------
# Setup all the include directories per platform.
#-------------------------------------------------------------------------------

if(WIN32)
  target_include_directories(cog PRIVATE
    "${CMAKE_SOURCE_DIR}/usr/share/boost/1.71.0"
    "${CMAKE_SOURCE_DIR}/usr/share"
    "${CMAKE_SOURCE_DIR}/src/core/include"
    "${CMAKE_SOURCE_DIR}/src/lua/5.3.5"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${PROJECT_BINARY_DIR}")
elseif(APPLE)
  target_include_directories(cog PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/src/core/include"
    "${CMAKE_SOURCE_DIR}"
    "/usr/local/include")
elseif(UNIX)
  target_include_directories(cog PRIVATE
    "${CMAKE_SOURCE_DIR}/usr/share/boost/1.71.0"
    "${CMAKE_SOURCE_DIR}/usr/share"
    "${CMAKE_SOURCE_DIR}/src/core/include"
    "${CMAKE_SOURCE_DIR}/src/lua/5.3.5"
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "/usr/local/include")
endif()

#-------------------------------------------------------------------------------
# Tell LUA to use 32-bit floats.
#-------------------------------------------------------------------------------

add_compile_definitions(cog LUA_FLOAT_TYPE=1)

#-------------------------------------------------------------------------------
# Force include eon.h into every unity cxx.
#-------------------------------------------------------------------------------

if(WIN32)
  add_definitions(/FI"eon/eon.h")
endif()

#-------------------------------------------------------------------------------
# Setup maximum warnings on my stuff.
#-------------------------------------------------------------------------------

if(WIN32)
  target_compile_options(cog PRIVATE -W3)
endif()

################################################################################

#-------------------------------------------------------------------------------
# Cotire stuff: https://github.com/sakra/cotire/blob/master/MANUAL.md
#-------------------------------------------------------------------------------

if(WIN32 OR UNIX) # includes APPLE macOS.
  # Exclude templatized files from unity.
  set_property(SOURCE src/view_schematic_block_input.cpp
    PROPERTY COTIRE_EXCLUDED TRUE)
  # Set the linked libraries.
  set_target_properties(cog PROPERTIES
    COTIRE_UNITY_LINK_LIBRARIES_INIT "NONE")
  # Set the multi-threading option...
  set_target_properties(cog PROPERTIES
    COTIRE_UNITY_SOURCE_MAXIMUM_NUMBER_OF_INCLUDES
    "-j")
  # Setup properties on cotire.
  set_target_properties(cog PROPERTIES
    COTIRE_PREFIX_HEADER_IGNORE_PATH  "${CMAKE_CURRENT_SOURCE_DIR}"
    COTIRE_PREFIX_HEADER_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")
  # Run that bad boy.
  cotire(cog)
endif()

################################################################################

#-------------------------------------------------------------------------------
# Link in the shared code and other EON libraries.
#-------------------------------------------------------------------------------

if(WIN32)

  #-----------------------------------------------------------------------------
  # Setup link directories for Windows that are outside the normal paths.
  #-----------------------------------------------------------------------------

  target_link_directories(cog PUBLIC
    "${CMAKE_SOURCE_DIR}/lib/win64/boost/1.71.0")

  #-----------------------------------------------------------------------------
  # Setup libraries to link with excluding boost; that comes in through pragma.
  #-----------------------------------------------------------------------------

  target_link_libraries(cog PUBLIC
    libboost_filesystem-vc142-mt-x64-1_71.lib
    eonStartup
    eonPAL
    eon
    lz4
    lua)

elseif(UNIX AND NOT APPLE)

  #-----------------------------------------------------------------------------
  # Setup link directories for Linux that are outside the normal paths.
  #-----------------------------------------------------------------------------

  target_link_directories(cog PUBLIC 
    "/usr/lib/x86_64-linux-gnu")
  find_package(Threads REQUIRED)

  #-----------------------------------------------------------------------------
  # Setup libraries to link with including boost.
  #
  # https://discourse.cmake.org/t/find-package-does-not-find-boost/509
  #-----------------------------------------------------------------------------

  target_link_libraries(cog PUBLIC
    Threads::Threads
    eonStartup
    eonPAL
    eon
    lz4
    lua)

elseif(APPLE)

  #-----------------------------------------------------------------------------
  # Setup frameworks to link against.
  #-----------------------------------------------------------------------------

  find_framework(CoreFoundation)
  find_framework(Foundation)

  set_target_properties(cog PROPERTIES
    COMPILE_FLAGS "${FRAMEWORK_PATHS}"
    LINK_FLAGS "${FRAMEWORK_PATHS} ${FRAMEWORK_NAMES}")

  #-----------------------------------------------------------------------------
  # Setup boost location.
  #-----------------------------------------------------------------------------

  find_library(boost REQUIRED PATHS
    "/usr/local/lib")

  #-----------------------------------------------------------------------------
  # Setup libraries to link with including boost.
  #-----------------------------------------------------------------------------

  target_link_libraries(cog PUBLIC
    /usr/local/lib/libboost_filesystem-mt.a
    /usr/local/lib/libboost_system-mt.a
    ${lz4_LIBRARIES}
    eonStartup
    eonPAL
    eon
    lz4
    lua)

endif()
