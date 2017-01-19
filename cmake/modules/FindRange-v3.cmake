# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# Global properties are used to retain state in order to avoid re-finding the
# package in the event of multiple find_package invocations, especially in
# different directories.
get_property(RANGE_V3_FOUND GLOBAL PROPERTY RANGE_V3_FOUND)
get_property(RANGE_V3_PROPERTIES GLOBAL PROPERTY RANGE_V3_PROPERTIES)
get_property(RANGE_V3_INCLUDE_DIRS GLOBAL PROPERTY RANGE_V3_INCLUDE_DIRS)
if(RANGE_V3_FOUND)
  return()
endif()

message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME}")

find_path(
  RANGE_V3_INCLUDE_DIRS "range/v3/range_concepts.hpp"
  PATHS ENV RANGE_V3_INSTALL_PATH
  PATH_SUFFIXES "/include")

# find_path sets a cached variable, but find modules shouldn't do so according
# to the cmake-developer documentation.  So, set a normal variable and unset
# the cache variable.
set(RANGE_V3_INCLUDE_DIRS ${RANGE_V3_INCLUDE_DIRS})
unset(RANGE_V3_INCLUDE_DIRS CACHE)

if(RANGE_V3_INCLUDE_DIRS)
  if(("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang") AND
     ("x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC"))
    # Clang/C2 requires -std=c++14 to parse some of the Microsoft provided
    # standard library headers.
    set(RANGE_V3_PROPERTIES
        CXX_STANDARD 14)
  else()
    set(RANGE_V3_PROPERTIES
        CXX_STANDARD 11)
  endif()
  set(RANGE_V3_PROPERTIES
      ${RANGE_V3_PROPERTIES}
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF)
  set_property(GLOBAL PROPERTY RANGE_V3_PROPERTIES ${RANGE_V3_PROPERTIES})
  set_property(GLOBAL PROPERTY RANGE_V3_INCLUDE_DIRS ${RANGE_V3_INCLUDE_DIRS})
  set(RANGE_V3_FOUND 1)
  set_property(GLOBAL PROPERTY RANGE_V3_FOUND ${RANGE_V3_FOUND})
endif()

if(RANGE_V3_FOUND)
  message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME} - found")
else()
  message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME} - not found")
  if(RANGE_V3_FIND_REQUIRED)
    message("Set the RANGE_V3_INSTALL_PATH environment variable to the"
            " location of the range-v3 installation")
    message(FATAL_ERROR "Package ${CMAKE_FIND_PACKAGE_NAME} not found")
  elseif(NOT RANGE_V3_FIND_QUIETLY)
    message(WARNING "Package ${CMAKE_FIND_PACKAGE_NAME} not found")
  endif()
endif()
