# Locate MongoDbs libbson library
# This module defines
#  LIBBSON_FOUND, if false, do not try to link
#  LIBBSON_LIBRARIES
#  LIBBSON_INCLUDE_DIR, where to find bson.h
#  LIBBSON_VERSION_STRING, the version of libbson found (since CMake 2.8.8)

find_path(LIBBSON_INCLUDE_DIR bson.h
  PATH_SUFFIXES libbson libbson-1.0
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

find_library(LIBBSON_LIBRARY
  NAMES bson bson-1.0
  PATH_SUFFIXES lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw
  /opt/local
  /opt/csw
  /opt
)

if(LIBBSON_LIBRARY)
    set( LIBBSON_LIBRARIES "${LIBBSON_LIBRARY}" CACHE STRING "Libbson Libraries")
endif()

if(LIBBSON_INCLUDE_DIR AND EXISTS "${LIBBSON_INCLUDE_DIR}/bson-version.h")
  file(STRINGS "${LIBBSON_INCLUDE_DIR}/bson-version.h" libbson_version_str REGEX "^#define[ \t]+BSON_VERSION_S[ \t]+\".+\"")

  string(REGEX REPLACE "^#define[ \t]+BSON_VERSION_S[ \t]+\"([^\"]+)\".*" "\\1" LIBBSON_VERSION_STRING "${libbson_version_str}")
  unset(libbson_version_str)
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBBSON_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibBson
                                  REQUIRED_VARS LIBBSON_LIBRARIES LIBBSON_INCLUDE_DIR
                                  VERSION_VAR LIBBSON_VERSION_STRING)

mark_as_advanced(LIBBSON_INCLUDE_DIR LIBBSON_LIBRARIES LIBBSON_LIBRARY)

