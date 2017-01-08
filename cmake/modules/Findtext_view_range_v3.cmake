# Copyright (c) 2016, Tom Honermann
#
# This file is distributed under the MIT License. See the accompanying file
# LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
# and conditions.

# Global properties are used to retain state in order to avoid re-finding the
# package in the event of multiple find_package invocations, especially in
# different directories.
get_property(text_view_range_v3_FOUND GLOBAL PROPERTY text_view_range_v3_FOUND)
get_property(text_view_range_v3_COMPILE_OPTIONS GLOBAL PROPERTY text_view_range_v3_COMPILE_OPTIONS)
get_property(text_view_range_v3_DEFINITIONS GLOBAL PROPERTY text_view_range_v3_DEFINITIONS)
get_property(text_view_range_v3_INCLUDE_DIRS GLOBAL PROPERTY text_view_range_v3_INCLUDE_DIRS)
if(text_view_range_v3_FOUND)
  return()
endif()

# Locate dependencies.  Prefer config mode, but fall back to module mode if
# a package isn't found.
find_package(Range-v3 QUIET NO_MODULE)
if(NOT RANGE_V3_FOUND)
  find_package(Range-v3 MODULE REQUIRED)
endif()

# FIXME: Use of the CMake compiler features support would be more appropriate
# FIXME: here.
set(text_view_range_v3_COMPILE_OPTIONS
      ${RANGE_V3_COMPILE_OPTIONS}
      -std=c++11
      -Wall -Werror -Wpedantic)
set_property(GLOBAL PROPERTY text_view_range_v3_COMPILE_OPTIONS ${text_view_range_v3_COMPILE_OPTIONS})

set(text_view_range_v3_DEFINITIONS
      ${RANGE_V3_DEFINITIONS})
set_property(GLOBAL PROPERTY text_view_range_v3_DEFINITIONS ${text_view_range_v3_DEFINITIONS})

set(text_view_range_v3_INCLUDE_DIR
      ${CMAKE_CURRENT_LIST_DIR}/../../include)
get_filename_component(
  text_view_range_v3_INCLUDE_DIR
  ${text_view_range_v3_INCLUDE_DIR}
  REALPATH)
set(text_view_range_v3_INCLUDE_DIRS
      ${RANGE_V3_INCLUDE_DIRS}
      ${text_view_range_v3_INCLUDE_DIR})
unset(text_view_range_v3_INCLUDE_DIR)
set_property(GLOBAL PROPERTY text_view_range_v3_INCLUDE_DIRS ${text_view_range_v3_INCLUDE_DIRS})

set(text_view_range_v3_FOUND 1)
set_property(GLOBAL PROPERTY text_view_range_v3_FOUND ${text_view_range_v3_FOUND})