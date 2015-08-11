# - Find the Poco includes and libraries.
# The following variables are set if Poco is found.  If Poco is not
# found, Poco_FOUND is set to false.
#  Poco_FOUND        - True when the Poco include directory is found.
#  Poco_INCLUDE_DIRS - the path to where the poco include files are.
#  Poco_LIBRARY_DIR - The path to where the poco library files are.
#  Poco_BINARY_DIRS - The path to where the poco dlls are.
#  Poco_LIBRARIES - list of all libs from requested components.

# ----------------------------------------------------------------------------
# If you have installed Poco in a non-standard location.
# Then you have three options.
# In the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of Poco. e.g
# If you have put poco in C:\development\Poco then <Your Path> is
# "C:/development/Poco" and in this directory there will be two
# directories called "include" and "lib".
# 1) After CMake runs, set Poco_INCLUDE_DIR to <Your Path>/poco<-version>
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/poco<-version>. This will allow find_path()
#    to locate Poco_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g.
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${POCO_ROOT} that points to the root of where you have
#    installed Poco, e.g. <Your Path>. It is assumed that there is at least a subdirectory called
#    Foundation/include/Poco in this path.
#
# Note:
#  1) If you are just using the poco headers, then you do not need to use
#     Poco_LIBRARY_DIR in your CMakeLists.txt file.
#  2) If Poco has not been installed, then when setting Poco_LIBRARY_DIR
#     the script will look for /lib first and, if this fails, then for /stage/lib.
#
# Usage:
# In your CMakeLists.txt file do something like this:
# ...
# # Poco
# find_package(Poco REQUIRED COMPONENTS XML Net Data...)
# ...
# include_directories(${Poco_INCLUDE_DIRS})
# link_directories(${Poco_LIBRARY_DIR})
#
# In Windows, we make the assumption that, if the Poco files are installed, the default directory
# will be C:\poco or C:\Program Files\Poco or C:\Programme\Poco.

message(STATUS "Searching for Poco library...")

set(POCO_INCLUDE_PATH_DESCRIPTION
  "top-level directory containing the poco include directories. E.g /usr/local/include/ or c:\\poco\\include\\poco-1.3.2")
set(POCO_INCLUDE_DIR_MESSAGE
  "Set the Poco_INCLUDE_DIR cmake cache entry to the ${POCO_INCLUDE_PATH_DESCRIPTION}")
set(POCO_LIBRARY_PATH_DESCRIPTION "top-level directory containing the poco libraries.")
set(POCO_LIBRARY_DIR_MESSAGE
  "Set the Poco_LIBRARY_DIR cmake cache entry to the ${POCO_LIBRARY_PATH_DESCRIPTION}")

set(POCO_DIR_SEARCH $ENV{POCO_ROOT})
if(POCO_DIR_SEARCH)
  file(TO_CMAKE_PATH ${POCO_DIR_SEARCH} POCO_DIR_SEARCH)
endif()

if(WIN32)
  set(POCO_DIR_SEARCH
    ${POCO_DIR_SEARCH}
    "C:/poco"
    "D:/poco"
    "C:/Program Files/poco"
    "D:/Program Files/poco"
  )
endif()

# Add in some path suffixes. These will have to be updated whenever a new Poco version comes out.
set(SUFFIX_FOR_INCLUDE_PATH
  poco-1.3.2
  poco-1.3.3
  poco-1.3.4
  poco-1.3.5
  poco-1.3.6
)

set(SUFFIX_FOR_LIBRARY_PATH
  poco-1.3.2/lib
  poco-1.3.2/lib/Linux/i686
  poco-1.3.2/lib/Linux/x86_64
  poco-1.3.3/lib
  poco-1.3.3/lib/Linux/i686
  poco-1.3.3/lib/Linux/x86_64
  poco-1.3.4/lib
  poco-1.3.4/lib/Linux/i686
  poco-1.3.4/lib/Linux/x86_64
  poco-1.3.5/lib
  poco-1.3.5/lib/Linux/i686
  poco-1.3.5/lib/Linux/x86_64
  poco-1.3.6/lib
  poco-1.3.6/lib/Linux/i686
  poco-1.3.6/lib/Linux/x86_64
  lib
  lib/Linux/i686
  lib/Linux/x86_64
)

#
# Look for an installation.
#
find_path(Poco_INCLUDE_DIR
  NAMES Foundation/include/Poco/SharedLibrary.h
  PATH_SUFFIXES ${SUFFIX_FOR_INCLUDE_PATH} PATHS
  # Look in other places.
  ${POCO_DIR_SEARCH}
  # Help the user find it if we cannot.
  DOC "The ${POCO_INCLUDE_PATH_DESCRIPTION}"
)

if(NOT Poco_INCLUDE_DIR)

# Look for standard unix include paths
  find_path(Poco_INCLUDE_DIR Poco/Poco.h DOC "The ${POCO_INCLUDE_PATH_DESCRIPTION}")

endif()

# Assume we didn't find it.
set(Poco_FOUND 0)

# Now try to get the include and library path.
if(Poco_INCLUDE_DIR)
  if(EXISTS "${Poco_INCLUDE_DIR}/Foundation/include/Poco/SharedLibrary.h")
    set(Poco_INCLUDE_DIRS
      ${Poco_INCLUDE_DIR}/CppUnit/include
      ${Poco_INCLUDE_DIR}/Foundation/include
      ${Poco_INCLUDE_DIR}/Net/include
      ${Poco_INCLUDE_DIR}/Util/include
      ${Poco_INCLUDE_DIR}/XML/include
    )
    set(Poco_FOUND TRUE)
  elseif(EXISTS "${Poco_INCLUDE_DIR}/Poco/Poco.h")
    set(Poco_INCLUDE_DIRS ${Poco_INCLUDE_DIR})
    set(Poco_FOUND TRUE)
  endif()

  if(NOT Poco_LIBRARY_DIR)
    find_library(Poco_FOUNDATION_LIB
      NAMES PocoFoundation PocoFoundationd  PATH_SUFFIXES ${SUFFIX_FOR_LIBRARY_PATH} PATHS
      # Look in other places.
      ${Poco_INCLUDE_DIR}
      ${POCO_DIR_SEARCH}
      # Help the user find it if we cannot.
      DOC "The ${POCO_LIBRARY_PATH_DESCRIPTION}"
    )
    set(Poco_LIBRARY_DIR "" CACHE PATH POCO_LIBARARY_PATH_DESCRIPTION)
    get_filename_component(Poco_LIBRARY_DIR ${Poco_FOUNDATION_LIB} PATH)
    set(Poco_LIBRARIES "")
    set(Comp_List "")
    if(Poco_LIBRARY_DIR AND Poco_FOUNDATION_LIB)
      # Look for the poco binary path.
      set(Poco_BINARY_DIR ${Poco_INCLUDE_DIR})
      if(Poco_BINARY_DIR AND EXISTS "${Poco_BINARY_DIR}/bin")
        set(Poco_BINARY_DIRS ${Poco_BINARY_DIR}/bin)
      endif()
    endif()
    if(Poco_FOUNDATION_LIB)
      if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        set(DBG "d")
      else()
        set(DBG "")
      endif()
      set(Comp_List "Foundation${DBG}")
      foreach(COMPONENT ${Poco_FIND_COMPONENTS})
        find_library(LIB${COMPONENT} "Poco${COMPONENT}${DBG}" Poco_LIBRARY_DIR)
        if(LIB${COMPONENT})
          list(APPEND Poco_LIBRARIES "${LIB${COMPONENT}}")
          list(APPEND Comp_List "${COMPONENT}${DBG}")
        endif()
      endforeach()
      list(REMOVE_DUPLICATES Comp_List)
    endif()
  endif()
endif()

if(NOT Poco_FOUND)
  if(Poco_FIND_QUIETLY)
    message(STATUS "Poco was not found. ${POCO_INCLUDE_DIR_MESSAGE}")
  elseif(Poco_FIND_REQUIRED)
    message(FATAL_ERROR "Poco was not found. ${POCO_INCLUDE_DIR_MESSAGE}")
  endif()
else()
  message(STATUS "  Found Poco!")
  set(COMPONENT_STR "components found:")
  foreach(comp ${Comp_List})
    set(COMPONENT_STR "${COMPONENT_STR}, ${comp}")
  endforeach()
  string(REPLACE ":," ":" COMPONENT_LSTR ${COMPONENT_STR})
  message(STATUS "${COMPONENT_LSTR}.")
endif()

# I added this in to add "libdl" on non-Windows systems.
# Technically dl is only neded if the "Foundation" component is used,
# but i doesn't hurt to add it in anyway
if(Poco_FOUND AND NOT WIN32)
  list(APPEND Poco_LIBRARIES "dl")
endif()
