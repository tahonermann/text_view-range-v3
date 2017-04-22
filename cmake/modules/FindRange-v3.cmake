# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# Global properties are used to retain state in order to avoid re-finding the
# package in the event of multiple find_package invocations, especially in
# different directories.
get_property(RANGE_V3_FOUND GLOBAL PROPERTY RANGE_V3_FOUND)
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
  set(RANGE_V3_FOUND 1)
endif()

if(NOT RANGE_V3_FOUND)
  message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME} - not found")
  if(RANGE_V3_FIND_REQUIRED)
    message("Set the RANGE_V3_INSTALL_PATH environment variable to the"
            " location of the range-v3 installation")
    message(FATAL_ERROR "Package ${CMAKE_FIND_PACKAGE_NAME} not found")
  elseif(NOT RANGE_V3_FIND_QUIETLY)
    message(WARNING "Package ${CMAKE_FIND_PACKAGE_NAME} not found")
  endif()
  return()
endif()

message("-- Checking for package ${CMAKE_FIND_PACKAGE_NAME} - found")

# FIXME: Use of INTERFACE_COMPILE_FEATURES and target_compile_features()
# FIXME: would be preferred here so that language version requirements
# FIXME: transitively follow dependencies, but that requires CMake 3.1 for
# FIXME: target_compile_features() and CMake 3.8 to be able to require
# FIXME: specific language versions (see e.g, CMAKE_CXX_KNOWN_FEATURES
# FIXME: and cxx_std_14).  For now, language version compiler options are
# FIXME: hard-coded.
if(("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xClang") AND
   ("x${CMAKE_CXX_SIMULATE_ID}" STREQUAL "xMSVC"))
  # Clang/C2 requires -std=c++14 to parse some of the Microsoft provided
  # standard library headers.
  set(RANGE_V3_COMPILE_OPTIONS "-std=c++14")
else()
  set(RANGE_V3_COMPILE_OPTIONS "-std=c++11")
endif()

add_library(range-v3 INTERFACE IMPORTED GLOBAL)
set_target_properties(range-v3 PROPERTIES
  INTERFACE_COMPILE_OPTIONS ${RANGE_V3_COMPILE_OPTIONS}
  INTERFACE_INCLUDE_DIRECTORIES ${RANGE_V3_INCLUDE_DIRS})

set_property(GLOBAL PROPERTY RANGE_V3_INCLUDE_DIRS ${RANGE_V3_INCLUDE_DIRS})
set_property(GLOBAL PROPERTY RANGE_V3_FOUND ${RANGE_V3_FOUND})
